#include "scene.h"

Scene::Scene(std::string sceneDirectory, std::string sceneJson)
{
    nlohmann::json sceneConfig;
    try {
        sceneConfig = nlohmann::json::parse(sceneJson);
    }
    catch (std::runtime_error e) {
        std::cerr << "Could not parse json." << std::endl;
        exit(1);
    }

    this->parse(sceneDirectory, sceneConfig);
}

Scene::Scene(std::string pathToJson)
{
    std::string sceneDirectory;

#ifdef _WIN32
    const size_t last_slash_idx = pathToJson.rfind('\\');
#else
    const size_t last_slash_idx = pathToJson.rfind('/');
#endif

    if (std::string::npos != last_slash_idx) {
        sceneDirectory = pathToJson.substr(0, last_slash_idx);
    }

    nlohmann::json sceneConfig;
    try {
        std::ifstream sceneStream(pathToJson.c_str());
        sceneStream >> sceneConfig;
    }
    catch (std::runtime_error e) {
        std::cerr << "Could not load scene .json file." << std::endl;
        exit(1);
    }

    this->parse(sceneDirectory, sceneConfig);
}

void Scene::parse(std::string sceneDirectory, nlohmann::json sceneConfig)
{   
    this->sceneConfig = sceneConfig;
    // Output
    try {
        auto res = sceneConfig["output"]["resolution"];
        this->imageResolution = Vector2i(res[0], res[1]);
    }
    catch (nlohmann::json::exception e) {
        std::cerr << "\"output\" field with resolution, filename & spp should be defined in the scene file." << std::endl;
        exit(1);
    }

    // Cameras
    try {
        auto cam = sceneConfig["camera"];

        this->camera = Camera(
            Vector3f(cam["from"][0], cam["from"][1], cam["from"][2]),
            Vector3f(cam["to"][0], cam["to"][1], cam["to"][2]),
            Vector3f(cam["up"][0], cam["up"][1], cam["up"][2]),
            float(cam["fieldOfView"]),
            this->imageResolution
        );
    }
    catch (nlohmann::json::exception e) {
        std::cerr << "No camera(s) defined. Atleast one camera should be defined." << std::endl;
        exit(1);
    }

    // Surface
    try {
        auto surfacePaths = sceneConfig["surface"];

        uint32_t surfaceIdx = 0;
        for (std::string surfacePath : surfacePaths) {
            surfacePath = sceneDirectory + "/" + surfacePath;

            auto surf = createSurfaces(surfacePath, /*isLight=*/false, /*idx=*/surfaceIdx);
            this->surfaces.insert(this->surfaces.end(), surf.begin(), surf.end());

            // Update scene AABB & surfaceIdxs (used for indirection in BVH)
            int c = 0;
            for (auto& s : surf) {
                this->bbox.min = Vector3f(std::min(this->bbox.min.x, s.bbox.min.x),
                    std::min(this->bbox.min.y, s.bbox.min.y),
                    std::min(this->bbox.min.z, s.bbox.min.z));

                this->bbox.max = Vector3f(std::max(this->bbox.max.x, s.bbox.max.x),
                    std::max(this->bbox.max.y, s.bbox.max.y),
                    std::max(this->bbox.max.z, s.bbox.max.z));

                // Update indirection indices for BVH
                this->surfaceIdxs.push_back(surfaceIdx + c);
                c += 1;
            }

            surfaceIdx = surfaceIdx + surf.size();
        }

        // Allocate memory for BVH based on max
        this->nodes = (BVHNode*) malloc((2 * this->surfaceIdxs.size() - 1) * sizeof(BVHNode));
        for (int i = 0; i < 2 * this->surfaceIdxs.size() - 1; i++) {
            this->nodes[i] = BVHNode();
        }
    }
    catch (nlohmann::json::exception e) {
        std::cout << "No surfaces defined." << std::endl;
    }

    // Build the BVH
    this->buildBVH();
}

void Scene::buildBVH()
{
    // Root node
    this->numBVHNodes += 1;

    BVHNode& rootNode = this->nodes[0];
    rootNode.firstPrim = 0;
    rootNode.primCount = this->surfaceIdxs.size();

    this->updateNodeBounds(0);
    this->subdivideNode(0);
}

uint32_t Scene::getIdx(uint32_t idx)
{
    return this->surfaceIdxs[idx];
}

void Scene::updateNodeBounds(uint32_t nodeIdx)
{
    BVHNode& node = this->nodes[nodeIdx];

    for (int i = 0; i < node.primCount; i++) {
        auto surf = this->surfaces[this->getIdx(i + node.firstPrim)];
        node.bbox.min = Vector3f(
            std::min(node.bbox.min.x, surf.bbox.min.x),
            std::min(node.bbox.min.y, surf.bbox.min.y),
            std::min(node.bbox.min.z, surf.bbox.min.z)
        );

        node.bbox.max = Vector3f(
            std::max(node.bbox.max.x, surf.bbox.max.x),
            std::max(node.bbox.max.y, surf.bbox.max.y),
            std::max(node.bbox.max.z, surf.bbox.max.z)
        );

        node.bbox.centroid = (node.bbox.min + node.bbox.max) / 2.f;
    }
}

void Scene::subdivideNode(uint32_t nodeIdx)
{
    BVHNode& node = this->nodes[nodeIdx];

    if (node.primCount <= 1) return;

    Vector3f extent = node.bbox.max - node.bbox.min;

    int ax = 0;
    if (extent.y > extent.x) ax = 1;
    if (extent.z > extent[ax]) ax = 2;
    float split = node.bbox.min[ax] + extent[ax] * 0.5f;

    int i = node.firstPrim;
    int j = i + node.primCount - 1;

    while(i <= j) {
        if (this->surfaces[this->getIdx(i)].bbox.centroid[ax] < split)
            i++;
        else {
            auto temp = this->surfaceIdxs[i];
            this->surfaceIdxs[i] = this->surfaceIdxs[j];
            this->surfaceIdxs[j--] = temp;
        }
    }

    int leftCount = i - node.firstPrim;
    if (leftCount == 0 || leftCount == node.primCount) return;

    uint32_t lidx = this->numBVHNodes++;
    BVHNode& left = this->nodes[lidx];
    left.firstPrim = node.firstPrim;
    left.primCount = leftCount;
    this->updateNodeBounds(lidx);
    
    uint32_t ridx = this->numBVHNodes++;
    BVHNode& right = this->nodes[ridx];
    right.firstPrim = i;
    right.primCount = node.primCount - leftCount;
    this->updateNodeBounds(ridx);

    node.left = lidx;
    node.right = ridx;
    node.primCount = 0;

    this->subdivideNode(lidx);
    this->subdivideNode(ridx);
}

void Scene::intersectBVH(uint32_t nodeIdx, Ray &ray, Interaction& si)
{
    BVHNode& node = this->nodes[nodeIdx];

    if (!node.bbox.intersects(ray)) return;

    if (node.primCount != 0) {
        // Leaf
        for (uint32_t i = 0; i < node.primCount; i++) {
            Interaction siIntermediate = this->surfaces[this->getIdx(i + node.firstPrim)].rayIntersect(ray);
            if (siIntermediate.t <= ray.t) {
                si = siIntermediate;
                ray.t = si.t;
                si.index=this->getIdx(i + node.firstPrim);
            }
        }
    }
    else {
        this->intersectBVH(node.left, ray, si);
        this->intersectBVH(node.right, ray, si);
    }
}

Interaction Scene::rayIntersect(Ray& ray)
{
    Interaction si;
    si.didIntersect = false;

    this->intersectBVH(0, ray, si);

    return si;
}

float Scene::get_area(Vector3f p1, Vector3f p2){
float a1 = p1.x;
float a2 = p1.y;
float a3 = p1.z;
float b1 = p2.x;
float b2 = p2.y;
float b3 = p2.z;

float v1 = a2*b3 - a3*b2;
float v2 = a3*b1 - a1*b3;
float v3 = a1*b2 - a2*b1;

float magnitude = v1*v1 + v2*v2 + v3*v3;
magnitude = sqrt(std::abs(magnitude));
magnitude/=2;
return magnitude;
}

void Scene::get_barycentric(Tri triangle, Vector3f intersection_point,Interaction &temp){
    float alpha = 0;
    float beta = 0;
    float gamma = 0;
    // float total_area = 0.5 * std::abs(triangle.v1.x * (triangle.v2.y - triangle.v3.y) + triangle.v2.x * (triangle.v3.y - triangle.v1.y) + triangle.v3.x * (triangle.v1.y - triangle.v2.y));
    // float alpha_area = 0.5 * std::abs(triangle.v1.x * (triangle.v2.y - intersection_point.y) + triangle.v2.x * (intersection_point.y - triangle.v1.y) + intersection_point.x * (triangle.v1.y - triangle.v2.y));
    // float beta_area = 0.5 * std::abs(intersection_point.x * (triangle.v2.y - triangle.v3.y) + triangle.v2.x * (triangle.v3.y - intersection_point.y) + triangle.v3.x * (intersection_point.y - triangle.v2.y));
    // float gamma_area = 0.5 * std::abs(triangle.v1.x * (intersection_point.y - triangle.v3.y) + intersection_point.x * (triangle.v3.y - triangle.v1.y) + triangle.v3.x * (triangle.v1.y - intersection_point.y));
    float total_area = get_area(triangle.v1-triangle.v2, triangle.v2-triangle.v3);
    float alpha_area = get_area(triangle.v1 - intersection_point,intersection_point-triangle.v2);
    float beta_area = get_area(triangle.v2 - intersection_point,intersection_point-triangle.v3);
    float gamma_area = get_area(triangle.v1 - intersection_point,intersection_point-triangle.v3);

  
   
    if(total_area == 0){
        
         temp.barycentric_coord[0] = 0;
    temp.barycentric_coord[1] = 0;
    return;

        // return;
          
    }
  
    else{
        alpha = alpha_area/total_area;
    beta = beta_area/total_area;
    gamma = gamma_area/total_area;

    gamma = 1 - alpha - beta;
    }
   

    Vector2f point;
    point.x = alpha*triangle.uv3.x + beta*triangle.uv1.x + gamma*triangle.uv2.x;
    point.y = alpha*triangle.uv3.y + beta*triangle.uv1.y + gamma*triangle.uv2.y;
    
    
    temp.barycentric_coord[0] = point[0];
    temp.barycentric_coord[1] = point[1];
    

}
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

    // Point Lights
    try {
        auto pointLights = sceneConfig["pointLights"];
        for (auto l : pointLights)
            this->lights.push_back(Light(LightType::POINT_LIGHT, l));
    }
    catch (nlohmann::json::exception e) {
        std::cerr << "No point lights defined." << std::endl;
    }

    // Directional Lights
    try {
        auto directionalLights = sceneConfig["directionalLights"];
        for (auto l : directionalLights)
            this->lights.push_back(Light(LightType::DIRECTIONAL_LIGHT, l));
    }
    catch (nlohmann::json::exception e) {
        std::cerr << "No directional lights defined." << std::endl;
    }

    // Area lights
    try {
        auto areaLights = sceneConfig["areaLights"];
        for (auto l : areaLights)
            this->lights.push_back(Light(LightType::AREA_LIGHT, l));
    }
    catch (nlohmann::json::exception e) {
        std::cerr << "No area lights defined." << std::endl;
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
            if (siIntermediate.t <= ray.t && siIntermediate.didIntersect) {
                si = siIntermediate;
                ray.t = si.t;
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

/**
 * Checks if a given ray intersects with any of the emitters in the scene.
*/
Interaction Scene::rayEmitterIntersect(Ray& ray) {
    Interaction si;
    si.didIntersect = false;

    for (Light &light : this->lights) {
        Interaction siIntermediate = light.intersectLight(&ray);
        
        if (siIntermediate.t <= ray.t && siIntermediate.didIntersect) {
            si = siIntermediate;
            ray.t = si.t;
        }
    }
   
    return si;
}

Vector3f Scene::UniformSampleHemisphere(const Vector2f &u) {
     float theta = acos(u[0]);
    float phi = 2 * M_PI * u[1];
    float val1 = sin(theta) * cos(phi);
    float val2 =  sin(theta) * sin(phi);
    float val3 = cos(theta);
    return Vector3f(val1,val2,val3);
}

Vector3f Scene::get_area(Vector3f p1, Vector3f p2){
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
Vector3f ans = Vector3f(v1,v2,v3);
return ans/magnitude;
}

std::vector<Vector3f> Scene::create_onb(Vector3f normal, Vector3f dir) {
    std::vector<Vector3f> ans(3, Vector3f(0, 0, 0));
    Vector3f obs1 = (normal);
   Vector3f obs2;
    if (std::abs(obs1.x) > 0.1) {
        obs2 = get_area(Vector3f(0, 1, 0), obs1);
    } else {
        obs2 = get_area(Vector3f(1, 0, 0), obs1);
    }

  // Vector3f obs2 = Normalize(Cross(obs1,dir));
    Vector3f obs3 =Normalize( Cross(obs2, obs1));
    

    ans[0] = obs1;
    ans[1] = obs2;
    ans[2] = obs3;

    return ans;
}



Vector2f Scene::ConcentricSampleDisk(Vector2f u) {

Vector2f uOffset = 2.f * u - Vector2f(1, 1);
    if (uOffset.x == 0 && uOffset.y == 0)
        return Vector2f(0, 0);
    float theta, r;
    if (std::abs(uOffset.x) > std::abs(uOffset.y))
    {
        r = uOffset.x;
        theta = M_PI / 4 * (uOffset.y / uOffset.x);
    }
    else
    {
        r = uOffset.y;
        theta = M_PI / 2 - M_PI / 4 * (uOffset.x / uOffset.y);
    }
    return r * Vector2f(std::cos(theta), std::sin(theta));



}
Vector3f Scene::CosineSampleHemisphere(Vector2f u) {
    Vector2f d = ConcentricSampleDisk(u);
    float z = std::sqrt(std::max((float)0, 1 - d.x * d.x - d.y * d.y));
    return Vector3f(d.x, d.y, z);
}
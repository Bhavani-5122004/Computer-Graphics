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

    // Surface
    try {
        auto surfacePaths = sceneConfig["surface"];

        uint32_t surfaceIdx = 0;
        for (std::string surfacePath : surfacePaths) {
            surfacePath = sceneDirectory + "/" + surfacePath;

            auto surf = createSurfaces(surfacePath, /*isLight=*/false, /*idx=*/surfaceIdx);
            this->surfaces.insert(this->surfaces.end(), surf.begin(), surf.end());

            surfaceIdx = surfaceIdx + surf.size();
        }
    }
    catch (nlohmann::json::exception e) {
        std::cout << "No surfaces defined." << std::endl;
    }
}

Interaction Scene::rayIntersect(Ray& ray)
{
    Interaction siFinal;

    for (auto& surface : this->surfaces) {
        Interaction si = surface.rayIntersect(ray);
        if (si.t <= ray.t) {    
            siFinal = si;
            ray.t = si.t;
        }
    }

    return siFinal;
}

//2.1

AABB AABB::create_aabb(Surface& surface) {
    AABB aabb;
    aabb.min_x = surface.vertices[0].x;
    aabb.min_y = surface.vertices[0].y;
    aabb.min_z = surface.vertices[0].z;
    aabb.max_x = surface.vertices[0].x;
    aabb.max_y = surface.vertices[0].y;
    aabb.max_z = surface.vertices[0].z;

    for (int i = 0; i < surface.vertices.size(); i++) {
        Vector3f& val = surface.vertices[i];
        aabb.min_x = std::min(aabb.min_x, val.x);
        aabb.min_y = std::min(aabb.min_y, val.y);
        aabb.min_z = std::min(aabb.min_z, val.z);

        aabb.max_x = std::max(aabb.max_x, val.x);
        aabb.max_y = std::max(aabb.max_y, val.y);
        aabb.max_z = std::max(aabb.max_z, val.z);
    }

    aabb.min.x = aabb.min_x;
    aabb.min.y = aabb.min_y;
    aabb.min.z = aabb.min_z;

    aabb.max.x = aabb.max_x;
    aabb.max.y = aabb.max_y;
    aabb.max.z = aabb.max_z;
   // aabb.surface = surface;

    return aabb;
}


bool Scene::ray_intersect_aabb(Ray ray, AABB aabb) {
    // Computing reciprocal direction components
    float rcp_x_dir = 1.0f / ray.d.x;
    float rcp_y_dir = 1.0f / ray.d.y;
    float rcp_z_dir = 1.0f / ray.d.z;

    // Slab method 
    float t_min_x = (aabb.min_x - ray.o.x) * rcp_x_dir;
    float t_max_x = (aabb.max_x - ray.o.x) * rcp_x_dir;
    float t_min_y = (aabb.min_y - ray.o.y) * rcp_y_dir;
    float t_max_y = (aabb.max_y - ray.o.y) * rcp_y_dir;
    float t_min_z = (aabb.min_z - ray.o.z) * rcp_z_dir;
    float t_max_z = (aabb.max_z - ray.o.z) * rcp_z_dir;

    float t_min = std::max(std::max(std::min(t_min_x, t_max_x), std::min(t_min_y, t_max_y)), std::min(t_min_z, t_max_z));
    float t_max = std::min(std::min(std::max(t_min_x, t_max_x), std::max(t_min_y, t_max_y)), std::max(t_min_z, t_max_z));

    return t_max > 0 && t_min <= t_max;
}





Interaction Scene::intersect_scene(Ray ray) {
    Interaction ans = this->surfaces[0].rayIntersect(ray);

    for (int i = 0; i < this->surfaces.size(); i++) {
        Surface& s = this->surfaces[i];
        AABB aabb = aabb.create_aabb(s);
        bool b = ray_intersect_aabb(ray, aabb);
        if (b) {
            Interaction i = s.rayIntersect(ray);
            if (i.t < ans.t) {
                ans = i;
            }
        }
    }

    return ans;
}




//2.2

BVH* BVH::create_node(const AABB& aabb, BVH* left, BVH* right,std::vector<Surface> surface_list) {
    BVH* bvh = new BVH();
    bvh->aabb = aabb;  
    bvh->left = left;
    bvh->right = right;
    bvh->surface_list = surface_list;
    return bvh;
}



BVH* BVH::create(std::vector<Surface>& surface_list) {
    if (surface_list.empty()) {
        return nullptr;
    }

    // Forming an AABB around the entire scene aka all the surfaces
    AABB aabb;
    aabb.min_x = aabb.max_x = surface_list[0].vertices[0].x;
    aabb.min_y = aabb.max_y = surface_list[0].vertices[0].y;
    aabb.min_z = aabb.max_z = surface_list[0].vertices[0].z;
     int x = surface_list.size();
    for (int i=0;i<x;i++) {
      Surface surface = surface_list[i];
        //printf("%d\n",i);
        AABB surface_aabb = surface_aabb.create_aabb(surface);
        aabb.min_x = std::min(aabb.min_x, surface_aabb.min_x);
        aabb.min_y = std::min(aabb.min_y, surface_aabb.min_y);
        aabb.min_z = std::min(aabb.min_z, surface_aabb.min_z);

        aabb.max_x = std::max(aabb.max_x, surface_aabb.max_x);
        aabb.max_y = std::max(aabb.max_y, surface_aabb.max_y);
        aabb.max_z = std::max(aabb.max_z, surface_aabb.max_z);
    }
   

   // printf("%d\n",surface_list.size());
    if (surface_list.size() <= 1) {
        return create_node(aabb, nullptr, nullptr,surface_list);
    } else {
        std::vector<Surface> sorted_list = surface_list; 

        if (aabb.max_x - aabb.min_x >= std::max(aabb.max_y - aabb.min_y, aabb.max_z - aabb.min_z)) {
            std::sort(sorted_list.begin(), sorted_list.end(), [](Surface& a, Surface& b) {
                AABB a_aabb = a_aabb.create_aabb(a);
                AABB b_aabb = b_aabb.create_aabb(b);
                return a_aabb.min_x < b_aabb.min_x;
            });
        } else if (aabb.max_y - aabb.min_y >= std::max(aabb.max_x - aabb.min_x, aabb.max_z - aabb.min_z)) {
            std::sort(sorted_list.begin(), sorted_list.end(), [](Surface& a, Surface& b) {
                AABB a_aabb = a_aabb.create_aabb(a);
                AABB b_aabb = b_aabb.create_aabb(b);
                return a_aabb.min_y < b_aabb.min_y;
            });
        } else {
            std::sort(sorted_list.begin(), sorted_list.end(), [](Surface& a, Surface& b) {
                AABB a_aabb = a_aabb.create_aabb(a);
                AABB b_aabb = b_aabb.create_aabb(b);
                return a_aabb.min_z < b_aabb.min_z;
            });
        }

        
        size_t middle = sorted_list.size() / 2;
        std::vector<Surface> left_list(sorted_list.begin(), sorted_list.begin() + middle);
        std::vector<Surface> right_list(sorted_list.begin() + middle, sorted_list.end());

        BVH* left_node = create(left_list);
        BVH* right_node = create(right_list);

        return create_node(aabb, left_node, right_node,surface_list);
    }
}





Interaction BVH::bvh_intersection(BVH* bvh, Ray ray, Scene& scene) {
    if (bvh == nullptr) {
        return Interaction(); 
    }

    if (bvh->left == nullptr && bvh->right == nullptr) {
        // Leaf node: check intersection with surfaces
        AABB aabb = bvh->aabb;
        bool intersectsAABB = scene.ray_intersect_aabb(ray, aabb);
        if (intersectsAABB) {
            // Interaction i = scene.intersect_scene(ray);
            Interaction i = bvh->surface_list[0].rayIntersect(ray);
            return i;
        } else {
            return Interaction(); 
        }
    }

    // Check if the ray intersects the AABB of the left subtree
    bool intersectsLeftAABB = (bvh->left && scene.ray_intersect_aabb(ray, bvh->left->aabb));

    // Check if the ray intersects the AABB of the right subtree
    bool intersectsRightAABB = (bvh->right && scene.ray_intersect_aabb(ray, bvh->right->aabb));

    if (!intersectsLeftAABB) {
        
        return bvh_intersection(bvh->right, ray, scene);
    }
    // If the right subtree is not intersected, skip the right subtree
   else if (!intersectsRightAABB) {
    
        return bvh_intersection(bvh->left, ray, scene);
    }

    else{
        // Check both left and right subtrees if the ray intersects aabbs of both left and right nodes
    Interaction i1 = bvh_intersection(bvh->left, ray, scene);
    Interaction i2 = bvh_intersection(bvh->right, ray, scene);

    return std::min(i1, i2, [](const Interaction& a, const Interaction& b) {
        return a.t < b.t;
    });
    }

    
}





//2.3

two_level_BVH* two_level_BVH::create_node_subtree(AABB& aabb, const Surface& surface, const std::vector<Vector3i>& indices) {
   
    // creating an aabb per triangle
    AABB newAABB ;
    const Vector3f& vertex_temp = surface.vertices[indices[0][0]];
    newAABB.min_x=newAABB.max_x = vertex_temp.x;
    newAABB.min_y=newAABB.max_y = vertex_temp.y;
    newAABB.min_z=newAABB.max_z = vertex_temp.z;
  

    for(int i=0;i<indices.size();i++){
        int j=0;
        while(j<3){
            const Vector3f& tri = surface.vertices[indices[i][j]];
            newAABB.min_x=std::min(newAABB.min_x,tri.x);
            newAABB.min_y=std::min(newAABB.min_y,tri.y);
            newAABB.min_z=std::min(newAABB.min_z,tri.z);
            j+=1;
        }
    }

     for(int i=0;i<indices.size();i++){
        int j=0;
        while(j<3){
            const Vector3f& tri = surface.vertices[indices[i][j]];

            newAABB.max_x=std::max(newAABB.max_x,tri.x);
            newAABB.max_y=std::max(newAABB.max_y,tri.y);
            newAABB.max_z=std::max(newAABB.max_z,tri.z);
            j+=1;
        }
    }
  //basecase
    if (indices.size() <= 1) {
        two_level_BVH* node = new two_level_BVH();
        node->aabb = newAABB; 
       // node->normals = normals;
        node->indices = indices;
        node->surface = surface;
        node->left = nullptr;
        node->right = nullptr;
        node->subtree = nullptr;
        return node;
    } else {
        //splitting indice list and recursively creating the subtree
        size_t middle = indices.size() / 2;
        std::vector<Vector3i> left_list(indices.begin(), indices.begin() + middle);
        std::vector<Vector3i> right_list(indices.begin() + middle, indices.end());

        two_level_BVH* left_node = create_node_subtree(newAABB, surface, left_list);
        two_level_BVH* right_node = create_node_subtree(newAABB, surface, right_list);

   

        two_level_BVH* node = new two_level_BVH();
        node->aabb = newAABB;
       // node->normals = normals;
        node->indices = indices;
        node->left = left_node;
        node->right = right_node;
        node->subtree = nullptr;
        return node;
    }
}




two_level_BVH* two_level_BVH::create_two(std::vector<Surface>& surface_list) {
    if (surface_list.empty()) {
        return nullptr;
    }
// creating aabb around all the surfaces
    AABB aabb;
    aabb.min_x = aabb.max_x = surface_list[0].vertices[0].x;
    aabb.min_y = aabb.max_y = surface_list[0].vertices[0].y;
    aabb.min_z = aabb.max_z = surface_list[0].vertices[0].z;

    for (int i = 0; i < surface_list.size(); i++) {
        Surface surface = surface_list[i];
        AABB surface_aabb = surface_aabb.create_aabb(surface);

        aabb.min_x = std::min(aabb.min_x, surface_aabb.min_x);
        aabb.min_y = std::min(aabb.min_y, surface_aabb.min_y);
        aabb.min_z = std::min(aabb.min_z, surface_aabb.min_z);

        aabb.max_x = std::max(aabb.max_x, surface_aabb.max_x);
        aabb.max_y = std::max(aabb.max_y, surface_aabb.max_y);
        aabb.max_z = std::max(aabb.max_z, surface_aabb.max_z);
    }

    

    if (surface_list.size() == 1) {
        two_level_BVH* node = new two_level_BVH;
        node->aabb = aabb;
       // node->normals = surface_list[0].normals;
        node->indices = surface_list[0].indices;
      //  node->surface = surface_list[0];
        node->left = nullptr;
        node->right = nullptr;
        node->subtree = create_node_subtree(aabb, surface_list[0], surface_list[0].indices);
        return node;
    } else {
        //splitting aabb based on longest axis
        std::vector<Surface> sorted_list = surface_list;

        if (aabb.max_x - aabb.min_x >= std::max(aabb.max_y - aabb.min_y, aabb.max_z - aabb.min_z)) {
            std::sort(sorted_list.begin(), sorted_list.end(), [](Surface& a, Surface& b) {
                AABB a_aabb = a_aabb.create_aabb(a);
                AABB b_aabb = b_aabb.create_aabb(b);
                return a_aabb.min_x < b_aabb.min_x;
            });
        } else if (aabb.max_y - aabb.min_y >= std::max(aabb.max_x - aabb.min_x, aabb.max_z - aabb.min_z)) {
            std::sort(sorted_list.begin(), sorted_list.end(), [](Surface& a, Surface& b) {
                AABB a_aabb = a_aabb.create_aabb(a);
                AABB b_aabb = b_aabb.create_aabb(b);
                return a_aabb.min_y < b_aabb.min_y;
            });
        } else {
            std::sort(sorted_list.begin(), sorted_list.end(), [](Surface& a, Surface& b) {
                AABB a_aabb = a_aabb.create_aabb(a);
                AABB b_aabb = b_aabb.create_aabb(b);
                return a_aabb.min_z < b_aabb.min_z;
            });
        }

        size_t middle = sorted_list.size() / 2;
        std::vector<Surface> left_list(sorted_list.begin(), sorted_list.begin() + middle);
        std::vector<Surface> right_list(sorted_list.begin() + middle, sorted_list.end());

        two_level_BVH* left_node = create_two(left_list);
        two_level_BVH* right_node = create_two(right_list);

        two_level_BVH* node = new two_level_BVH;
        node->aabb = aabb;
       // node->normals = surface_list[0].normals;
        node->indices = surface_list[0].indices;
        node->surface = surface_list[0];
        node->left = left_node;
        node->right = right_node;
        node->subtree = nullptr;
        return node;
    }
}



// bool two_level_BVH::traversal(two_level_BVH* bvh){
//     if(bvh==nullptr){
//         return false;
//     }
//     return traversal(bvh->left) || traversal(bvh->right);
//     return false;
// }

Interaction two_level_BVH::closest_interaction(two_level_BVH* bvh, Ray ray, Scene& scene) {
    if (bvh == nullptr) {
        return Interaction();
    }

    Interaction current = Interaction();

    // Check if the ray intersects the AABB of the current node
    bool intersectsAABB = scene.ray_intersect_aabb(ray, bvh->aabb);

    if (!intersectsAABB) {
     
        return Interaction(); 
    }

    if (bvh->left == nullptr && bvh->right == nullptr) {
     
        float tmin = ray.t;
       // printf("---%d\n", bvh->indices.size());
      
         auto face = bvh->indices[0];
        
            Vector3f p1 = bvh->surface.vertices[face.x];
            Vector3f p2 = bvh->surface.vertices[face.y];
            Vector3f p3 = bvh->surface.vertices[face.z];

            Vector3f n1 = bvh->surface.normals[face.x];
            Vector3f n2 = bvh->surface.normals[face.y];
            Vector3f n3 = bvh->surface.normals[face.z];
            Vector3f n = Normalize(n1 + n2 + n3);

            Interaction si = bvh->surface.rayTriangleIntersect(ray, p1, p2, p3, n);
            if (si.t <= tmin && si.didIntersect) {
                current = si;
                tmin = si.t;
            }
       // }
     //   current = bvh->surface.rayIntersect(ray);

        return current;
    } else {

          bool intersectsLeftAABB = (bvh->left && scene.ray_intersect_aabb(ray, bvh->left->aabb));

    // Check if the ray intersects the AABB of the right subtree
    bool intersectsRightAABB = (bvh->right && scene.ray_intersect_aabb(ray, bvh->right->aabb));

    if (!intersectsLeftAABB) {
        
        return closest_interaction(bvh->right, ray, scene);
    }
    // If the right subtree is not intersected, skip the right subtree
   else if (!intersectsRightAABB) {
    
        return closest_interaction(bvh->left, ray, scene);
    }
       else{
         // Non-leaf node: Check both left and right subtrees if the ray intersects both the left and right nodes
        Interaction leftInteraction = closest_interaction(bvh->left, ray, scene);
        Interaction rightInteraction = closest_interaction(bvh->right, ray, scene);

        return std::min(leftInteraction, rightInteraction, [](const Interaction& a, const Interaction& b) {
            return a.t < b.t;
        });
       }
    }
}



Interaction two_level_BVH::bvh_intersection_two(two_level_BVH* bvh, Ray ray, Scene& scene){
 if (bvh == nullptr) {
        return Interaction(); 
    }

    if (bvh->left == nullptr && bvh->right == nullptr) {
        // Leaf node: check intersection with surfaces
        AABB aabb = bvh->aabb;
        bool intersectsAABB = scene.ray_intersect_aabb(ray, aabb);
        if (intersectsAABB) {
            // Interaction i = scene.intersect_scene(ray);
            Interaction i = closest_interaction(bvh->subtree,ray,scene);
            //bvh->surface_list[0].rayIntersect(ray);
            return i;
        } else {
            return Interaction(); 
        }
    }

    // Check if the ray intersects the AABB of the left subtree
    bool intersectsLeftAABB = (bvh->left && scene.ray_intersect_aabb(ray, bvh->left->aabb));

    // Check if the ray intersects the AABB of the right subtree
    bool intersectsRightAABB = (bvh->right && scene.ray_intersect_aabb(ray, bvh->right->aabb));

    if (!intersectsLeftAABB) {
        
        return bvh_intersection_two(bvh->right, ray, scene);
    }
    // If the right subtree is not intersected, skip the right subtree
   else if (!intersectsRightAABB) {
    
        return bvh_intersection_two(bvh->left, ray, scene);
    }

    else{
        // Check both left and right subtrees if the ray intersects the aabbs of both the left and right nodes
    Interaction i1 = bvh_intersection_two(bvh->left, ray, scene);
    Interaction i2 = bvh_intersection_two(bvh->right, ray, scene);

    return std::min(i1, i2, [](const Interaction& a, const Interaction& b) {
        return a.t < b.t;
    });
    }
}


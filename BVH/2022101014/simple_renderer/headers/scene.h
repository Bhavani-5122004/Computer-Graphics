#pragma once

#include "camera.h"
#include "surface.h"


struct AABB {
    float max_x;
    float max_y;
    float max_z;
    float min_x;
    float min_y;
    float min_z;
    Vector3f min;
    Vector3f max;
    // Surface surface;

    AABB create_aabb(Surface& surface);
};

struct Scene {
    std::vector<Surface> surfaces;
    Camera camera;
    Vector2i imageResolution;

    Scene() {};
    Scene(std::string sceneDirectory, std::string sceneJson);
    Scene(std::string pathToJson);
    
    void parse(std::string sceneDirectory, nlohmann::json sceneConfig);

    Interaction rayIntersect(Ray& ray);
    bool ray_intersect_aabb(Ray ray, AABB aabb);
    Interaction intersect_scene(Ray ray);
};



struct BVH{
    AABB aabb;
    struct BVH* left;
    struct BVH* right;
    std::vector<Surface> surface_list;

    BVH* create_node(const AABB& aabb, BVH* left, BVH* right,std::vector<Surface> surface_list);
    BVH* create(std::vector<Surface>& surface_list);
   Interaction bvh_intersection(BVH* bvh, Ray ray, Scene& scene);
};

struct two_level_BVH{
AABB aabb;
Surface surface;
std::vector<Vector3i> indices;
struct two_level_BVH* left;
struct two_level_BVH* right;
struct two_level_BVH* subtree;

two_level_BVH* create_node_subtree(AABB& aabb, const Surface& surface, const std::vector<Vector3i>& indices);
two_level_BVH* create_two(std::vector<Surface>& surface_list);
Interaction bvh_intersection_two(two_level_BVH* bvh, Ray ray, Scene& scene);
Interaction closest_interaction(two_level_BVH* bvh,Ray ray,Scene& scene);
//bool traversal(two_level_BVH* bvh);

};

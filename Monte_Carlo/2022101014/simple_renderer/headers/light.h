#pragma once

#include "common.h"

enum LightType {
    POINT_LIGHT=0,
    DIRECTIONAL_LIGHT=1,
    AREA_LIGHT=2,
    NUM_LIGHT_TYPES
};

struct LightSample {
    // Position of the light sample
    Vector3f p;

    // Outgoing direction in world coordinates
    Vector3f wo;

    // Distance to the sampled point
    float d;

    
};

class Light {
    public:
        Light(LightType type, nlohmann::json config);

        /**
         * Samples the light to return the outgoing direction along with
         * the appropriately scaled radiance.
         *
         * \param si
         * The interaction struct of the shading point
         *
         * \return scaled_radiance
         * The incoming radiance from the light source scaled by the PDF of sampling
         * \return wo
         * The sampled direction in world space
         */
        std::pair<Vector3f, LightSample> sample(Interaction *si);
        bool is_area;

        /**
         * Checks whether a ray intersects with the light.
         * The method returns immediately for POINT/DIRECTIONAL, with si.didIntersect = false
         *
         * \param ray
         * The ray to test for intersections
         *
         * \return si
         * The interaction struct describing intersection with the light if any
         */
        Vector3f center, vx, vy, normal;
        Interaction intersectLight(Ray *ray);
        Vector3f get_area(Vector3f p1, Vector3f p2);
        
        

    private:
        LightType type;

        // Applicable only for point lights.
        Vector3f position;

        // Applicable only for directional lights
        Vector3f direction;

        // Applicable only for area lights
        

        // Radiance of the emitter
        Vector3f radiance;
};
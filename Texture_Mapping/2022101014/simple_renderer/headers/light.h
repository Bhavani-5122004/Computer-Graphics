#pragma once

#include "common.h"


enum LightType {
	POINT_LIGHT=0,
	DIRECTIONAL_LIGHT=1,
	NUM_LIGHT_TYPES
};

struct Light {
	std::vector<Vector3f> direction;
	std::vector<Vector3f> radiance;
    int light_type;
	std::vector<Light> Load(nlohmann::json sceneConfig);
};
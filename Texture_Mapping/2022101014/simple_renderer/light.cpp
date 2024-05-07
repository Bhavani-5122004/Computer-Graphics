#include "light.h"

std::vector<Light> Light::Load(nlohmann::json sceneConfig) {
    std::vector<Light> answer;
 nlohmann::json temp_vector = sceneConfig["directionalLights"];
    if (sceneConfig.contains("directionalLights") && temp_vector.size()>=1) {
        nlohmann::json light_vector = sceneConfig["directionalLights"];

        for (int i = 0; i < light_vector.size(); i++) {
            Light temp_light;
            // temp_light.direction = static_cast<std::vector<Vector3f>>(light_vector[i]["direction"]);
            // temp_light.radiance = static_cast<std::vector<Vector3f>>(light_vector[i]["radiance"]);
           temp_light.direction.push_back({
                light_vector[i]["direction"][0],
                light_vector[i]["direction"][1],
                light_vector[i]["direction"][2]
            });

            temp_light.radiance.push_back({
                light_vector[i]["radiance"][0],
                light_vector[i]["radiance"][1],
                light_vector[i]["radiance"][2]
            });
            temp_light.light_type = 1;
            answer.push_back(temp_light);
        }
    }

     if (sceneConfig.contains("pointLights")) {
       
        
        nlohmann::json light_vector = sceneConfig["pointLights"];

        for (int i = 0; i < light_vector.size(); i++) {
            Light temp_light;
            // temp_light.direction = static_cast<std::vector<Vector3f>>(light_vector[i]["direction"]);
            // temp_light.radiance = static_cast<std::vector<Vector3f>>(light_vector[i]["radiance"]);
           temp_light.direction.push_back({
                light_vector[i]["location"][0],
                light_vector[i]["location"][1],
                light_vector[i]["location"][2]
            });

            temp_light.radiance.push_back({
                light_vector[i]["radiance"][0],
                light_vector[i]["radiance"][1],
                light_vector[i]["radiance"][2]
            });
            temp_light.light_type = 0;
            answer.push_back(temp_light);
        }
    }

    return answer;
}

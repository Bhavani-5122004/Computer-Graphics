#include "light.h"

Light::Light(LightType type, nlohmann::json config) {
    switch (type) {
        case LightType::POINT_LIGHT:
        this->is_area = 0;
            this->position = Vector3f(config["location"][0], config["location"][1], config["location"][2]);
            break;
        case LightType::DIRECTIONAL_LIGHT:
        this->is_area = 0;
            this->direction = Vector3f(config["direction"][0], config["direction"][1], config["direction"][2]);
            break;
        case LightType::AREA_LIGHT:
        this->is_area = 1;
            this->center = Vector3f(config["center"][0], config["center"][1], config["center"][2]);
            this->vx = Vector3f(config["vx"][0], config["vx"][1], config["vx"][2]);
            this->vy = Vector3f(config["vy"][0], config["vy"][1], config["vy"][2]);
            this->normal = Vector3f(config["normal"][0], config["normal"][1], config["normal"][2]);
          
        break;
        default:
            std::cout << "WARNING: Invalid light type detected";
            break;
    }

    this->radiance = Vector3f(config["radiance"][0], config["radiance"][1], config["radiance"][2]);
    this->type = type;
}

std::pair<Vector3f, LightSample> Light::sample(Interaction *si) {
    LightSample ls;
    memset(&ls, 0, sizeof(ls));

    Vector3f radiance;
    switch (type) {
        case LightType::POINT_LIGHT:
            ls.wo = (position - si->p);
            ls.d = ls.wo.Length();
            ls.wo = Normalize(ls.wo);
            radiance = (1.f / (ls.d * ls.d)) * this->radiance;
            break;
        case LightType::DIRECTIONAL_LIGHT:
            ls.wo = Normalize(direction);
            ls.d = 1e10;
            radiance = this->radiance;
            break;
        case LightType::AREA_LIGHT:
      
        Vector3f sample_point = center + 2*(next_float() - 0.5)*vx +  2*(next_float() - 0.5)*vy;
           ls.wo =(sample_point - si->p);
            ls.d = ls.wo.Length();
            ls.wo = Normalize(ls.wo);
            radiance = this->radiance;
         
            break;
    }
    return { radiance, ls };
}

Vector3f Light::get_area(Vector3f p1, Vector3f p2){
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

Interaction Light::intersectLight(Ray *ray) {
    Interaction si;
   
    memset(&si, 0, sizeof(si));

    if (type == LightType::AREA_LIGHT) {
   

         Interaction temp;
         temp.didIntersect = false;

   float dDotN = Dot(ray->d,normal);
if (dDotN < 0.f) {
    float t = -Dot(ray->o - center, normal) / dDotN;

        if (t >= 0.f) {
            temp.didIntersect = true;
            temp.t = t;
            temp.n = normal;
            temp.p = (ray->o) + ray->d * temp.t;
        }
    }
 
 // If the ray intersects the plane
          if(temp.didIntersect){
            float u = Dot(temp.p - center,vx);
            float v = Dot(temp.p - center,vy);
              u = u/vx.LengthSquared();
              v = v/vy.LengthSquared();
           
            // If the intersection point is within the area light itself it will be a ratio of the area in both x and y directions
           
            if (u >= -1 && u <= 1 && v >= -1 && v<= 1) {
              
                si.didIntersect = true;
                si.p = temp.p;
                si.n = normal;
                si.emissiveColor = radiance;
                si.t = temp.t; 
                
                return si; 
            }
          }
           //  }
        }
         
          si.didIntersect = false;
    si.p = Vector3f(0,0,0);
    si.n = Vector3f(0,0,0);
    si.emissiveColor = Vector3f(0,0,0);
    // printf("%f %f %f\n",si.emissiveColor[0],si.emissiveColor[1],si.emissiveColor[2]);
    return si;
    }
   





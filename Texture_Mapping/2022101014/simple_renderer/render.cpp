#include "render.h"
#include "light.cpp"


Integrator::Integrator(Scene &scene)
{
    this->scene = scene;
    this->outputImage.allocate(TextureType::UNSIGNED_INTEGER_ALPHA, this->scene.imageResolution);
}

long long Integrator::render()
{
    auto startTime = std::chrono::high_resolution_clock::now();
    // std::string sceneJson;
    // nlohmann::json sceneConfig;
    // nlohmann::json::parse(this->scene);

    // sceneConfig = this->scene.sceneConfig;
    // // this->scene.parse(sceneJson,sceneConfig);
    // nlohmann::json sceneConfig = nlohmann::json::parse(this->scene);

    for (int x = 0; x < this->scene.imageResolution.x; x++)
    {
        for (int y = 0; y < this->scene.imageResolution.y; y++)
        {

            Ray cameraRay = this->scene.camera.generateRay(x, y);
            Interaction si = this->scene.rayIntersect(cameraRay);
            Vector3f shade_color = (Vector3f(0.f, 0.f, 0.f));
            

            if (si.didIntersect)
            {

                //   this->outputImage.writePixelColor(0.5f * (si.n + Vector3f(1.f, 1.f, 1.f)), x, y);
                nlohmann::json sceneConfig = this->scene.sceneConfig;
                Light temp_light;
                std::vector<Light> light_vector = temp_light.Load(sceneConfig);

                for (int i = 0; i < light_vector.size(); i++)
                {

                     float magnitude = 0;
                            magnitude += (light_vector[i].direction[0].x - si.p[0]) * (light_vector[i].direction[0].x - si.p[0]);
                            magnitude += (light_vector[i].direction[0].y - si.p[1]) * (light_vector[i].direction[0].y - si.p[1]);
                            magnitude += (light_vector[i].direction[0].z - si.p[2]) * (light_vector[i].direction[0].z - si.p[2]);
                            float magnitude_bef_sq = magnitude;
                            magnitude = sqrt(magnitude);
                  //  printf("%f %f %f\n",light_vector[i].direction[0].x,light_vector[i].direction[0].y,light_vector[i].direction[0].z);        
                    //  Ray shadow_ray(si.p + 1e-3 * si.n, light_vector[i].direction[0]);
                    Ray shadow_ray = Ray(Vector3f(0.f, 0.f, 0.f), Vector3f(1.f, 0.f, 0.f));
                  //  nlohmann::json temp_vector = sceneConfig["directionalLights"];
                    
                    if (light_vector[i].light_type==1)
                    {
                        shadow_ray.o = si.p + 1e-5 * si.n;
                        shadow_ray.d = light_vector[i].direction[0];
                    }
                    else
                    {
                        shadow_ray.o = si.p + 1e-3 * si.n;
                        shadow_ray.d = (light_vector[i].direction[0] - si.p)/magnitude;
                    }
                    Interaction shadow_ray_intersection = this->scene.rayIntersect(shadow_ray);

                     if (1)
                     {

                        if (light_vector[i].light_type==1)
                        {
                            // if(Dot(si.n,light_vector[i].direction[0]) < 0){
                            //     si.n = -1 * si.n;
                            // }
                          
                            if(!shadow_ray_intersection.didIntersect)
                            shade_color += (Vector3f(1.f, 1.f, 1.f) / M_PI) * (si.n.x * light_vector[i].direction[0].x + si.n.y * light_vector[i].direction[0].y + si.n.z * light_vector[i].direction[0].z) * light_vector[i].radiance[0];
                        }

                        else
                        {
                            if(Dot(si.n,Normalize(light_vector[i].direction[0] - si.p)) < 0){
                                si.n = -1 * si.n;
                            }
                            
                           
                            if (shadow_ray_intersection.t >(light_vector[i].direction[0] - si.p).Length())
                            shade_color += ((Vector3f(1.f, 1.f, 1.f) / M_PI) * ((si.n.x * (light_vector[i].direction[0].x - si.p[0]) + si.n.y * (light_vector[i].direction[0].y - si.p[1]) + si.n.z * (light_vector[i].direction[0].z - si.p[2])) / magnitude) * light_vector[i].radiance[0]) / (magnitude_bef_sq);
                        }
                    }
                }
                this->outputImage.writePixelColor(shade_color, x, y);
            }

            else
                this->outputImage.writePixelColor(Vector3f(0.f, 0.f, 0.f), x, y);
        }
    }
    auto finishTime = std::chrono::high_resolution_clock::now();

    return std::chrono::duration_cast<std::chrono::microseconds>(finishTime - startTime).count();
}


long long Integrator::render2()
{
    auto startTime = std::chrono::high_resolution_clock::now();
    for (int x = 0; x < this->scene.imageResolution.x; x++) {
        for (int y = 0; y < this->scene.imageResolution.y; y++) {
            Ray cameraRay = this->scene.camera.generateRay(x, y);
            Interaction si = this->scene.rayIntersect(cameraRay);

            if (si.didIntersect){
                Vector3f point = si.p;
                // std::vector<Surface> surface_list;
                 Surface intersect_surface = this->scene.surfaces[0];
                // // float min_dist = 10000000;
                  Vector3f texture_color = (Vector3f(1.f, 1.f, 1.f));
                for(int k=0;k<this->scene.surfaces.size();k++){
                    Surface temp_surface = this->scene.surfaces[k];
                    Interaction I = temp_surface.rayIntersect(cameraRay);
                    if(k == si.index){
                      intersect_surface = this->scene.surfaces[si.index];
                    }
          

                }
           
               if(intersect_surface.hasDiffuseTexture()){
                 
                Tri intersection_triangle;
                for(int k = 0;k<intersect_surface.tris.size();k++){
                   Tri temp_triangle = intersect_surface.tris[k];
                   if(temp_triangle.uv1 == si.triangle.uv1 && temp_triangle.uv2 == si.triangle.uv2 && temp_triangle.uv3 == si.triangle.uv3){
                    if(temp_triangle.v1 == si.triangle.v1 && temp_triangle.v2 == si.triangle.v2 && temp_triangle.v3 == si.triangle.v3){
                        if(temp_triangle.normal == si.triangle.normal && temp_triangle.centroid == si.triangle.centroid){
                            intersection_triangle = temp_triangle;
                        }
                    }
                   }
                }
               

    // float min_x = std::min({si.v1.x, si.v2.x, si.v3.x});
    // float min_y = std::min({si.v1.y, si.v2.y, si.v3.y});
    // float min_z = std::min({si.v1.z, si.v2.z, si.v3.z});

    // float max_x = std::max({si.v1.x, si.v2.x, si.v3.x});
    // float max_y = std::max({si.v1.y, si.v2.y, si.v3.y});
    // float max_z = std::max({si.v1.z, si.v2.z, si.v3.z});

    // Vector3f max_vector(max_x,max_y,max_z);
    // Vector3f min_vector(min_x,min_y,min_z);

    // Interaction C = intersect_surface.rayTriangleIntersect(cameraRay,si.triangle.v1,si.triangle.v2,si.v3,si.n);
  
   

        Tri temp_triangle;
        temp_triangle = intersection_triangle;
    
        
         this->scene.get_barycentric(temp_triangle, si.p, si);

        //  if(si.barycentric_coord[0]*(intersect_surface.diffuseTexture.resolution.x-1) > (intersect_surface.diffuseTexture.resolution.x-1) || si.barycentric_coord[1]*(intersect_surface.diffuseTexture.resolution.y-1) > (intersect_surface.diffuseTexture.resolution.y-1)){
        //     texture_color = Vector3f(0,0,0);
        //  }
        // else
         texture_color = intersect_surface.diffuseTexture.nearestNeighbourFetch(si.barycentric_coord,intersect_surface.diffuseTexture.resolution.x-1,intersect_surface.diffuseTexture.resolution.y-1);
       // printf("%f %f %f %f\n",si.barycentric_coord[0],si.barycentric_coord[1],round(si.barycentric_coord[0]),round(1 -si.barycentric_coord[1]));
        // if(si.barycentric_coord[0] == 0 && si.barycentric_coord[1] == 0){
        //     printf("Here\n");
        //     texture_color = this->scene.surfaces[si.index].diffuseTexture.loadPixelColor(intersect_surface.diffuseTexture.resolution.x/2,intersect_surface.diffuseTexture.resolution.y/2);
        // }
      //  break;
  //  }



               }
               else{
                texture_color = intersect_surface.diffuse;
                 
               }

               
               //printf("%f %f %f\n",texture_color.x,texture_color.y,texture_color.z);
                  nlohmann::json sceneConfig = this->scene.sceneConfig;
                Light temp_light;
                std::vector<Light> light_vector = temp_light.Load(sceneConfig);
                Vector3f shade_color = (Vector3f(0.f, 0.f, 0.f));
                for (int i = 0; i < light_vector.size(); i++)
                {

                     float magnitude = 0;
                            magnitude += (light_vector[i].direction[0].x - si.p[0]) * (light_vector[i].direction[0].x - si.p[0]);
                            magnitude += (light_vector[i].direction[0].y - si.p[1]) * (light_vector[i].direction[0].y - si.p[1]);
                            magnitude += (light_vector[i].direction[0].z - si.p[2]) * (light_vector[i].direction[0].z - si.p[2]);
                            float magnitude_bef_sq = magnitude;
                            magnitude = sqrt(magnitude);
                            
                    //  Ray shadow_ray(si.p + 1e-3 * si.n, light_vector[i].direction[0]);
                    Ray shadow_ray = Ray(Vector3f(0.f, 0.f, 0.f), Vector3f(1.f, 0.f, 0.f));
                  //  nlohmann::json temp_vector = sceneConfig["directionalLights"];

                    if (light_vector[i].light_type==1)
                    {
                        shadow_ray.o = si.p + 1e-5 * si.n;
                        shadow_ray.d = light_vector[i].direction[0];
                    }
                    else
                    {
                        shadow_ray.o = si.p + 1e-3 * si.n;
                        shadow_ray.d = (light_vector[i].direction[0] - si.p)/magnitude;
                    }
                    Interaction shadow_ray_intersection = this->scene.rayIntersect(shadow_ray);

                      if (1)
                     {

                        if (light_vector[i].light_type == 1)
                        {
                            if(!shadow_ray_intersection.didIntersect)
                            shade_color += (texture_color / M_PI) * (si.n.x * light_vector[i].direction[0].x + si.n.y * light_vector[i].direction[0].y + si.n.z * light_vector[i].direction[0].z) * light_vector[i].radiance[0];
                        }

                        else
                        {
                            if(Dot(si.n,Normalize(light_vector[i].direction[0] - si.p)) < 0){
                                si.n = -1 * si.n;
                            }
                           
                            if (shadow_ray_intersection.t >(light_vector[i].direction[0] - si.p).Length())
                            shade_color += ((texture_color / M_PI) * ((si.n.x * (light_vector[i].direction[0].x - si.p[0]) + si.n.y * (light_vector[i].direction[0].y - si.p[1]) + si.n.z * (light_vector[i].direction[0].z - si.p[2])) / magnitude) * light_vector[i].radiance[0]) / (magnitude_bef_sq);
                        }
                    }
                }
                this->outputImage.writePixelColor(shade_color, x, y);
             
            }
               
            else
                this->outputImage.writePixelColor(Vector3f(0.f, 0.f, 0.f), x, y);
        }
    }
    auto finishTime = std::chrono::high_resolution_clock::now();

    return std::chrono::duration_cast<std::chrono::microseconds>(finishTime - startTime).count();
}


long long Integrator::render3()
{
    auto startTime = std::chrono::high_resolution_clock::now();
    for (int x = 0; x < this->scene.imageResolution.x; x++) {
        for (int y = 0; y < this->scene.imageResolution.y; y++) {
            Ray cameraRay = this->scene.camera.generateRay(x, y);
            Interaction si = this->scene.rayIntersect(cameraRay);

            if (si.didIntersect){
                Vector3f point = si.p;
                // std::vector<Surface> surface_list;
                 Surface intersect_surface = this->scene.surfaces[0];
                // // float min_dist = 10000000;
                  Vector3f texture_color = (Vector3f(1.f, 1.f, 1.f));
                for(int k=0;k<this->scene.surfaces.size();k++){
                    Surface temp_surface = this->scene.surfaces[k];
                    Interaction I = temp_surface.rayIntersect(cameraRay);
                    if(k == si.index){
                      intersect_surface = this->scene.surfaces[si.index];
                    }
          

                }
           
               if(intersect_surface.hasDiffuseTexture()){
                 
                Tri intersection_triangle;
                for(int k = 0;k<intersect_surface.tris.size();k++){
                   Tri temp_triangle = intersect_surface.tris[k];
                   if(temp_triangle.uv1 == si.triangle.uv1 && temp_triangle.uv2 == si.triangle.uv2 && temp_triangle.uv3 == si.triangle.uv3){
                    if(temp_triangle.v1 == si.triangle.v1 && temp_triangle.v2 == si.triangle.v2 && temp_triangle.v3 == si.triangle.v3){
                        if(temp_triangle.normal == si.triangle.normal && temp_triangle.centroid == si.triangle.centroid){
                            intersection_triangle = temp_triangle;
                        }
                    }
                   }
                }
               

  
   

        Tri temp_triangle;
        temp_triangle = intersection_triangle;
    
        
         this->scene.get_barycentric(temp_triangle, si.p, si);

        texture_color = intersect_surface.diffuseTexture.bilinearFetch(si.barycentric_coord,intersect_surface.diffuseTexture.resolution.x-1,intersect_surface.diffuseTexture.resolution.y-1);
       // printf("%f %f %f %f\n",si.barycentric_coord[0],si.barycentric_coord[1],round(si.barycentric_coord[0]),round(1 -si.barycentric_coord[1]));
        // if(si.barycentric_coord[0] == 0 && si.barycentric_coord[1] == 0){
        //     printf("Here\n");
        //     texture_color = this->scene.surfaces[si.index].diffuseTexture.loadPixelColor(intersect_surface.diffuseTexture.resolution.x/2,intersect_surface.diffuseTexture.resolution.y/2);
        // }
      //  break;
  //  }



               }
               else{
                texture_color = intersect_surface.diffuse;
                 
               }
               //printf("%f %f %f\n",texture_color.x,texture_color.y,texture_color.z);
                  nlohmann::json sceneConfig = this->scene.sceneConfig;
                Light temp_light;
                std::vector<Light> light_vector = temp_light.Load(sceneConfig);
                Vector3f shade_color = (Vector3f(0.f, 0.f, 0.f));
                for (int i = 0; i < light_vector.size(); i++)
                {

                     float magnitude = 0;
                            magnitude += (light_vector[i].direction[0].x - si.p[0]) * (light_vector[i].direction[0].x - si.p[0]);
                            magnitude += (light_vector[i].direction[0].y - si.p[1]) * (light_vector[i].direction[0].y - si.p[1]);
                            magnitude += (light_vector[i].direction[0].z - si.p[2]) * (light_vector[i].direction[0].z - si.p[2]);
                            float magnitude_bef_sq = magnitude;
                            magnitude = sqrt(magnitude);
                            
                    //  Ray shadow_ray(si.p + 1e-3 * si.n, light_vector[i].direction[0]);
                    Ray shadow_ray = Ray(Vector3f(0.f, 0.f, 0.f), Vector3f(1.f, 0.f, 0.f));
                  //  nlohmann::json temp_vector = sceneConfig["directionalLights"];

                    if (light_vector[i].light_type==1)
                    {
                        shadow_ray.o = si.p + 1e-5 * si.n;
                        shadow_ray.d = light_vector[i].direction[0];
                    }
                    else
                    {
                        shadow_ray.o = si.p + 1e-3 * si.n;
                        shadow_ray.d = (light_vector[i].direction[0] - si.p)/magnitude;
                    }
                    Interaction shadow_ray_intersection = this->scene.rayIntersect(shadow_ray);

                     if (1)
                     {

                        if (light_vector[i].light_type==1)
                        {
                            if(!shadow_ray_intersection.didIntersect)
                            shade_color += (texture_color / M_PI) * (si.n.x * light_vector[i].direction[0].x + si.n.y * light_vector[i].direction[0].y + si.n.z * light_vector[i].direction[0].z) * light_vector[i].radiance[0];
                        }

                        else
                        {
                            if(Dot(si.n,Normalize(light_vector[i].direction[0] - si.p)) < 0){
                                si.n = -1 * si.n;
                            }
                           
                            if (shadow_ray_intersection.t >(light_vector[i].direction[0] - si.p).Length())
                            shade_color += ((texture_color / M_PI) * ((si.n.x * (light_vector[i].direction[0].x - si.p[0]) + si.n.y * (light_vector[i].direction[0].y - si.p[1]) + si.n.z * (light_vector[i].direction[0].z - si.p[2])) / magnitude) * light_vector[i].radiance[0]) / (magnitude_bef_sq);
                        }
                    }
                }
                this->outputImage.writePixelColor(shade_color, x, y);
             
            }
               
            else
                this->outputImage.writePixelColor(Vector3f(0.f, 0.f, 0.f), x, y);
        }
    }
    auto finishTime = std::chrono::high_resolution_clock::now();

    return std::chrono::duration_cast<std::chrono::microseconds>(finishTime - startTime).count();
}

int main(int argc, char **argv)
{
   if (argc != 4) {
        std::cerr << "Usage: ./render <scene_config> <out_path> <intersection_variant>";
        return 1;
    }
    Scene scene(argv[1]);
    int arg3 = std::stoi(argv[3]);

    Integrator rayTracer(scene);
    auto renderTime = 0;
    if(arg3==2){
        renderTime = rayTracer.render();
    }
    if(arg3==0){
        renderTime = rayTracer.render2();
    }
    if(arg3==1){
        renderTime = rayTracer.render3();
    }

    std::cout << "Render Time: " << std::to_string(renderTime / 1000.f) << " ms" << std::endl;
    rayTracer.outputImage.save(argv[2]);

    return 0;
}

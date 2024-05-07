#include "render.h"

Integrator::Integrator(Scene &scene)
{
    this->scene = scene;
    this->outputImage.allocate(TextureType::UNSIGNED_INTEGER_ALPHA, this->scene.imageResolution);
}

long long Integrator::render(int pixel_num)
{
    auto startTime = std::chrono::high_resolution_clock::now();
    for (int x = 0; x < this->scene.imageResolution.x; x++)
    {
        for (int y = 0; y < this->scene.imageResolution.y; y++)
        {
            Vector3f color(0, 0, 0);

            for (int k = 0; k < pixel_num; k++)
            {
                float val = next_float();

                Ray cameraRay = this->scene.camera.generateRay(x, y, val);
                Interaction si = this->scene.rayIntersect(cameraRay);
                Vector3f result(0, 0, 0);

                Vector3f term(0, 0, 0);
                if (si.didIntersect)
                {
                    Vector3f radiance;
                    LightSample ls;
                    for (Light &light : this->scene.lights)
                    {
                        std::tie(radiance, ls) = light.sample(&si);

                        Ray shadowRay(si.p + 1e-3f * si.n, ls.wo);
                        Interaction siShadow = this->scene.rayIntersect(shadowRay);

                        if (!siShadow.didIntersect || siShadow.t > ls.d)
                        {
                            result += si.bsdf->eval(&si, si.toLocal(ls.wo)) * radiance * std::abs(Dot(si.n, ls.wo));
                        }
                    }
                    //  result -= 0.81 * (si.bsdf->eval(&si, si.toLocal(ls.wo)) * radiance * std::abs(Dot(si.n, ls.wo)));
                    color += result;
                    term += si.emissiveColor;
                }
                // color += term;
                color += term / pixel_num;
            }
            color = color / pixel_num;
            this->outputImage.writePixelColor(color, x, y);
        }
    }
    auto finishTime = std::chrono::high_resolution_clock::now();

    return std::chrono::duration_cast<std::chrono::microseconds>(finishTime - startTime).count();
}

long long Integrator::render2(int pixel_num)
{
    auto startTime = std::chrono::high_resolution_clock::now();
    for (int x = 0; x < this->scene.imageResolution.x; x++)
    {
        for (int y = 0; y < this->scene.imageResolution.y; y++)
        {
            Vector3f color(0, 0, 0);
            //  int c = 0;

            for (int k = 0; k < pixel_num; k++)
            {
                float val = next_float();

                Ray cameraRay = this->scene.camera.generateRay(x, y, val);
                Interaction si = this->scene.rayIntersect(cameraRay);

                Vector3f result(0, 0, 0);
                Vector3f term(0, 0, 0);

                if (si.didIntersect)
                {
                    Vector3f radiance = Vector3f(0, 0, 0);
                    LightSample ls;

                    for (Light &light : this->scene.lights)
                    {
                        std::tie(radiance, ls) = light.sample(&si);

                        Ray shadowRay(si.p + 1e-3f * si.n, ls.wo);
                        Interaction siShadow = this->scene.rayIntersect(shadowRay);

                        if (!siShadow.didIntersect || siShadow.t > ls.d)
                        {

                            result += si.bsdf->eval(&si, si.toLocal(ls.wo)) * radiance * std::abs(Dot(si.n, ls.wo));
                           
                        }
                    }

                    color += result;
                    term += si.emissiveColor;
                }

                Interaction temp = this->scene.rayEmitterIntersect(cameraRay);
                if (temp.didIntersect)
                {
                    color += temp.emissiveColor;
                }
                // color += term/pixel_num;
            }
            //  if(c>0)
            //        printf("%d\n",c);

            color = color / pixel_num;

            this->outputImage.writePixelColor(color, x, y);
            // if(countser > 0)
            // printf("%d\n",countser);
        }
    }
    auto finishTime = std::chrono::high_resolution_clock::now();

    return std::chrono::duration_cast<std::chrono::microseconds>(finishTime - startTime).count();
}

long long Integrator::render3(int pixel_num)
{
    auto startTime = std::chrono::high_resolution_clock::now();
    for (int x = 0; x < this->scene.imageResolution.x; x++)
    {
        for (int y = 0; y < this->scene.imageResolution.y; y++)
        {

             
 Vector3f result(0, 0, 0);
              Vector3f radiance(0, 0, 0);
            for (int pix_num = 0; pix_num < pixel_num; pix_num++)
            {
                 
            
           // for(int pix_num = 0 ; pix_num < pixel_num ; pix_num++){
                
            //  int c = 0;

         //   float val = next_float();

            Ray cameraRay = this->scene.camera.generateRay(x, y, 0.5);
            Interaction si = this->scene.rayIntersect(cameraRay);

            Vector3f term(0, 0, 0);
            Interaction temp = this->scene.rayEmitterIntersect(cameraRay);
            if (si.didIntersect && (si.t < temp.t || !temp.didIntersect))
            {

                 Vector3f color(0, 0, 0);
               // Vector3f result(0, 0, 0);
                for (int k = 0; k < 1; k++)
                {
                    
                   
                    LightSample ls;

                    
                    

                    Vector3f dir = this->scene.UniformSampleHemisphere(Vector2f(next_float(), next_float()));
                  // dir = Normalize(dir);
                 Vector3f transformed_dir = si.toWorld(dir);
                

                        Ray shadowRay(si.p + 1e-3f * si.n, transformed_dir);
                        Interaction siShadow = this->scene.rayIntersect(shadowRay);
                        Interaction color_interaction = this->scene.rayEmitterIntersect(shadowRay);

                        if((!siShadow.didIntersect || siShadow.t > color_interaction.t) && color_interaction.didIntersect){
                            color += si.bsdf->eval(&si, dir) * color_interaction.emissiveColor * std::abs(Dot(si.n, transformed_dir));
                        }
                            
                   
                     }

                   // color += result;
                    term += si.emissiveColor;
                     color = color * 2 * M_PI;
             result += color / pixel_num;
              //  }
            }
            
            
           // else if(temp.didIntersect){
               else if (temp.didIntersect)
            {
                result += temp.emissiveColor;
            }
            //}
            }

            this->outputImage.writePixelColor(result, x, y);
        }
    }
    auto finishTime = std::chrono::high_resolution_clock::now();

    return std::chrono::duration_cast<std::chrono::microseconds>(finishTime - startTime).count();
}

long long Integrator::render4(int pixel_num)
{
    auto startTime = std::chrono::high_resolution_clock::now();
    for (int x = 0; x < this->scene.imageResolution.x; x++)
    {
        for (int y = 0; y < this->scene.imageResolution.y; y++)
        {
          Vector3f result(0, 0, 0);
              Vector3f radiance(0, 0, 0);
            for (int pix_num = 0; pix_num < pixel_num; pix_num++)
            {
                 
            
           // for(int pix_num = 0 ; pix_num < pixel_num ; pix_num++){
                
            //  int c = 0;

         //   float val = next_float();

            Ray cameraRay = this->scene.camera.generateRay(x, y, 0.5);
            Interaction si = this->scene.rayIntersect(cameraRay);

            Vector3f term(0, 0, 0);
            Interaction temp = this->scene.rayEmitterIntersect(cameraRay);
            if (si.didIntersect && (si.t < temp.t || !temp.didIntersect))
            {

                 Vector3f color(0, 0, 0);
               // Vector3f result(0, 0, 0);
                for (int k = 0; k < 1; k++)
                {
                   
                    LightSample ls;
                    

                    Vector3f dir = this->scene.CosineSampleHemisphere(Vector2f(next_float(), next_float()));
                  // dir = Normalize(dir);
                 Vector3f transformed_dir = si.toWorld(dir);
                

                        Ray shadowRay(si.p + 1e-3f * si.n, transformed_dir);
                        Interaction siShadow = this->scene.rayIntersect(shadowRay);
                        Interaction color_interaction = this->scene.rayEmitterIntersect(shadowRay);
                        if((!siShadow.didIntersect || siShadow.t > color_interaction.t) && color_interaction.didIntersect){
                            color += si.bsdf->eval(&si, dir) * color_interaction.emissiveColor;
                        }
                   
                     }

                   // color += result;
                    term += si.emissiveColor;
                     color = color * M_PI;         
             result += color / pixel_num;
              //  }
            }
            
            
          //  else if(temp.didIntersect){
               else if (temp.didIntersect)
            {
                result += temp.emissiveColor;
            }
           // }
            
            }

            this->outputImage.writePixelColor(result , x, y);
        }
    }
    auto finishTime = std::chrono::high_resolution_clock::now();

    return std::chrono::duration_cast<std::chrono::microseconds>(finishTime - startTime).count();
}

long long Integrator::render5(int pixel_num)
{
    auto startTime = std::chrono::high_resolution_clock::now();
    for (int x = 0; x < this->scene.imageResolution.x; x++)
    {
        for (int y = 0; y < this->scene.imageResolution.y; y++)
        {
           Vector3f color(0, 0, 0);
            //  int c = 0;
            
            for (int k = 0; k < pixel_num; k++)
            {
                float val = next_float();

                Ray cameraRay = this->scene.camera.generateRay(x, y, val);
                Interaction si = this->scene.rayIntersect(cameraRay);

                Vector3f result(0, 0, 0);
                Vector3f term(0, 0, 0);
                Interaction temp = this->scene.rayEmitterIntersect(cameraRay);
                if (si.didIntersect && si.t < temp.t)
                {
                    Vector3f radiance = Vector3f(0, 0, 0);
                    LightSample ls;

                    for (Light &light : this->scene.lights)
                    {
                        if(light.is_area == 1){
                            std::tie(radiance, ls) = light.sample(&si);

                        Ray shadowRay(si.p + 1e-3f * si.n, ls.wo);
                        Interaction siShadow = this->scene.rayIntersect(shadowRay);
                        Interaction rad = this->scene.rayEmitterIntersect(shadowRay);

                       float area = 4 * Cross(light.vx, light.vy).Length();
                       float mag = ls.d*ls.d;
                      float mul_term = area/mag;


                                    if ((!siShadow.didIntersect || siShadow.t > ls.d) && Dot(ls.wo, light.normal) <= 0)
                                    {
                                        result += si.bsdf->eval(&si, si.toLocal(ls.wo)) * radiance * std::abs(Dot(si.n, ls.wo)) * std::abs(Dot(light.normal, ls.wo)) * mul_term;
                                    }
                        }
                    }

                    color += result;
                    term += si.emissiveColor;
                }
               // color += term / pixel_num;
                
                else{
                    if (temp.didIntersect)
                {
                    color += temp.emissiveColor;
                }
                }
                // color += term/pixel_num;
            }
            //  if(c>0)
            //        printf("%d\n",c);

            color = color / pixel_num;
            // color = color * this->scene.lights.size();

            this->outputImage.writePixelColor(color, x, y);
        }
    }
    auto finishTime = std::chrono::high_resolution_clock::now();

    return std::chrono::duration_cast<std::chrono::microseconds>(finishTime - startTime).count();
}

int main(int argc, char **argv)
{
    if (argc != 5)
    {
        std::cerr << "Usage: ./render <scene_config> <out_path> <num_samples> <sampling_strategy>";
        return 1;
    }
    Scene scene(argv[1]);

    Integrator rayTracer(scene);
    int spp = atoi(argv[3]);
    auto renderTime = 0;
    if (atoi(argv[4]) == 3)
    {
        renderTime = rayTracer.render(spp);
    }
    if (atoi(argv[4]) == 4)
    {
        renderTime = rayTracer.render2(spp);
    }
    if (atoi(argv[4]) == 0)
    {
        renderTime = rayTracer.render3(spp);
    }
    if (atoi(argv[4]) == 1)
    {
        renderTime = rayTracer.render4(spp);
    }
    if (atoi(argv[4]) == 2)
    {
        renderTime = rayTracer.render5(spp);
    }
    std::cout << "Render Time: " << std::to_string(renderTime / 1000.f) << " ms" << std::endl;
    rayTracer.outputImage.save(argv[2]);

    return 0;
}
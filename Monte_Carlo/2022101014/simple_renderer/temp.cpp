//  auto startTime = std::chrono::high_resolution_clock::now();
//     for (int x = 0; x < this->scene.imageResolution.x; x++)
//     {
//         for (int y = 0; y < this->scene.imageResolution.y; y++)
//         {
            // Vector3f color(0, 0, 0);
            
            // for (int k = 0; k < pixel_num; k++)
            // {
            //     float val = next_float();

            //     Ray cameraRay = this->scene.camera.generateRay(x, y, val);
            //     Interaction si = this->scene.rayIntersect(cameraRay);
            //     Vector3f result(0, 0, 0);

            //     if (si.didIntersect)
            //     {
            //        // Vector3f radiance;
            //         LightSample ls;
            //         // for (Light &light : this->scene.lights)
            //         // {
            //           //  std::tie(radiance, ls) = light.sample(&si);
                       
                        
                         
            //             // std::vector<Vector3f> onb = this->scene.create_onb(si.n);
            //            // std::vector<Vector3f> onb(3, Vector3f(0, 0, 0));
            //             Vector3f onb1 = Normalize(si.n);
            //             Vector3f onb2 = Normalize(Cross(std::abs(onb1.x) > 0.1 ? Vector3f(0,1,0) : Vector3f(1,0,0), onb1));
            //             Vector3f onb3 = Cross(onb1, onb2);
            //            Vector3f dir = this->scene.UniformSampleHemisphere(Vector2f(next_float(),next_float()));
            //            Vector3f create_dir = onb2*dir.x + onb3*dir.y + onb1*dir.z;
                        
            //             Ray shoot_ray(si.p + 1e-3f * si.n, create_dir);
            //             Interaction temp1 = this->scene.rayEmitterIntersect(shoot_ray);
            //             Interaction temp2 = this->scene.rayIntersect(shoot_ray);
                         
            //             // Ray shadowRay(si.p + 1e-3f * si.n, ls.wo);
            //             // Interaction siShadow = this->scene.rayIntersect(shadowRay);

            //             //if (!siShadow.didIntersect || siShadow.t > ls.d)
            //             if( temp2.didIntersect && temp1.emissiveColor != Vector3f(0.f, 0.f, 0.f))
            //             {   
            //                 Vector3f f = si.bsdf->eval(&si, si.toLocal(create_dir));
            //                 Vector3f radiance = temp1.emissiveColor;
            //                 result += (f * radiance * std::abs(Dot(si.n, create_dir)));
                            
            //             }
            //        // }
            //     //    result -= 0.81 * (si.bsdf->eval(&si, si.toLocal(ls.wo)) * radiance * std::abs(Dot(si.n, ls.wo)));
            //        color += result * 2 * M_PI;
            //     }
            // }
            // color = color / pixel_num;
            // this->outputImage.writePixelColor(color, x, y);
//         }
//     }
//     auto finishTime = std::chrono::high_resolution_clock::now();

//     return std::chrono::duration_cast<std::chrono::microseconds>(finishTime - startTime).count();


//  Vector3f color(0, 0, 0);
//             for (int k = 0; k < pixel_num; k++)
//             {
//                 Vector3f result(0, 0, 0);
            
//                 Ray cameraRay = this->scene.camera.generateRay(x, y, next_float());
//                 Interaction si = this->scene.rayIntersect(cameraRay);
//                 if (si.didIntersect)
//                 {
//                     // Vector3f radiance;
//                     // LightSample ls;
//                     // for (Light &light : this->scene.lights)
//                     // {
//                       //  std::tie(radiance, ls) = light.sample(&si);
//                     Vector3f dir = this->scene.UniformSampleHemisphere(Vector2f(next_float(), next_float()));

//                     std::vector<Vector3f> ans = this->scene.create_onb(si.n,dir);                                                                  
//                     Vector3f onb1 = ans[0];
//                     Vector3f onb2 = ans[1];
//                   // Vector3f onb2 = Normalize(Cross(onb1,cameraRay.o + cameraRay.t * cameraRay.d));
//                  // Vector3f onb2 = Normalize(Cross(onb1,dir));
//                     Vector3f onb3 = ans[2];
                   
                    

                   
//                     Vector3f create_dir = dir.x * onb2 + dir.y * onb3 + dir.z * onb1;

//                     Ray shoot_ray(si.p + 1e-3f * si.n, create_dir);

//                     Interaction temp1 = this->scene.rayIntersect(shoot_ray);
                    

//                     // Ray shadowRay(si.p + 1e-3f * si.n, ls.wo);
//                         // Interaction siShadow = this->scene.rayIntersect(shadowRay);

//                         //if (!siShadow.didIntersect || siShadow.t > ls.d)

//                     if (temp1.didIntersect)
//                     {

//                         Interaction temp2 = this->scene.rayEmitterIntersect(shoot_ray);
//                         result += (si.bsdf->eval(&si, si.toLocal(create_dir))* temp2.emissiveColor * std::abs(Dot(si.n, create_dir)));
//                     }
//                     // }
//                 //    result -= 0.81 * (si.bsdf->eval(&si, si.toLocal(ls.wo)) * radiance * std::abs(Dot(si.n, ls.wo)));
//                     color += result;
//                 }
//             }
//             color = color / pixel_num;
//             color = color * 2 * M_PI;
//             this->outputImage.writePixelColor(color, x, y);
// Vector3f color(0, 0, 0);
//             for (int k = 0; k < pixel_num; k++)
//             {
//                 Vector3f result(0, 0, 0);
            
//                 Ray cameraRay = this->scene.camera.generateRay(x, y, next_float());
//                 Interaction si = this->scene.rayIntersect(cameraRay);
//                 if (si.didIntersect)
//                 {
//                   //  Vector3f radiance;
//                   //  LightSample ls;
//                     // for (Light &light : this->scene.lights)
//                     // {
//                       // std::tie(radiance, ls) = light.sample(&si);
//                     Vector3f dir = this->scene.UniformSampleHemisphere(Vector2f(next_float(), next_float()));
//                     dir = Normalize(dir);
//                     std::vector<Vector3f> ans = this->scene.create_onb(si.n,dir);                                                                  
//                     Vector3f onb1 = ans[0];
//                     Vector3f onb2 = ans[1];
//                   // Vector3f onb2 = Normalize(Cross(onb1,cameraRay.o + cameraRay.t * cameraRay.d));
//                  // onb2 = Normalize(Cross(onb1,dir));
//                     Vector3f onb3 = ans[2];
                   
                    

                   
//                     Vector3f create_dir = dir.x * onb2 + dir.y * onb3 + dir.z * onb1;
//                //  Vector3f create_dir= Vector3f(Dot(dir, onb1), Dot(dir, onb2), Dot(dir, onb3));

//                     create_dir = Normalize(create_dir);

//                     Ray shoot_ray(si.p + 1e-3f * si.n, create_dir);

//                     Interaction temp1 = this->scene.rayIntersect(shoot_ray);
//                     Interaction temp2 = this->scene.rayEmitterIntersect(shoot_ray);
                    

//                     // Ray shadowRay(si.p + 1e-3f * si.n, ls.wo);
//                     //    Interaction siShadow = this->scene.rayIntersect(shadowRay);

//                      //   if ((!temp1.didIntersect || temp1.t > ls.d) && temp2.didIntersect)

//                      if (temp2.didIntersect || !temp1.didIntersect)
//                     {

                        
//                         result += (si.bsdf->eval(&si, si.toLocal(create_dir)) * temp2.emissiveColor * std::abs(Dot(si.n, create_dir)));
//                     }
//                  //   }
//                 //    result -= 0.81 * (si.bsdf->eval(&si, si.toLocal(ls.wo)) * radiance * std::abs(Dot(si.n, ls.wo)));
//                     color += result;
//                 }
//                 Interaction temp = this->scene.rayEmitterIntersect(cameraRay);
//                 if(temp.didIntersect){
//                     color += temp.emissiveColor;
//                 }
                
//             }
//             color = color / pixel_num;
//             color = color * 2 * M_PI;
//             this->outputImage.writePixelColor(color, x, y);






























//  Vector3f result(0, 0, 0);
//               Vector3f radiance(0, 0, 0);
//             for (int pix_num = 0; pix_num < 100; pix_num++)
//             {
                 
            
//            // for(int pix_num = 0 ; pix_num < pixel_num ; pix_num++){
                
//             //  int c = 0;

//          //   float val = next_float();

//             Ray cameraRay = this->scene.camera.generateRay(x, y, 0.5);
//             Interaction si = this->scene.rayIntersect(cameraRay);

//             Vector3f term(0, 0, 0);
//             Interaction temp = this->scene.rayEmitterIntersect(cameraRay);
//             if (si.didIntersect && (si.t < temp.t || !temp.didIntersect))
//             {

//                  Vector3f color(0, 0, 0);
//                // Vector3f result(0, 0, 0);
//                 for (int k = 0; k < pixel_num; k++)
//                 {
                   
//                     LightSample ls;
                    

//                     Vector3f dir = this->scene.UniformSampleHemisphere(Vector2f(next_float(), next_float()));
//                   // dir = Normalize(dir);
//                  Vector3f transformed_dir = si.toWorld(dir);
                

//                         Ray shadowRay(si.p + 1e-3f * si.n, transformed_dir);
//                         Interaction siShadow = this->scene.rayIntersect(shadowRay);
//                         Interaction color_interaction = this->scene.rayEmitterIntersect(shadowRay);
//                         if((!siShadow.didIntersect || siShadow.t > color_interaction.t) && color_interaction.didIntersect){
//                             color += si.bsdf->eval(&si, dir) * color_interaction.emissiveColor * std::abs(Dot(si.n, transformed_dir));
//                         }
                   
//                      }

//                    // color += result;
//                     term += si.emissiveColor;
//                      color = color * 2 * M_PI;
//              result += color / pixel_num;
//               //  }
//             }
            
            
//             else if(temp.didIntersect){
//                 if (temp.didIntersect)
//             {
//                 result += temp.emissiveColor;
//             }
//             }
//             // color += term/pixel_num;
//             //   }
//             //  if(c>0)
//             //        printf("%d\n",c);

           

//         //    }
//            // this->outputImage.writePixelColor(color/100, x, y);
//             }

//             this->outputImage.writePixelColor(result / 100, x, y);
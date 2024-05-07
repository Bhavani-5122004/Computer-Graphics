#pragma once

#include "scene.h"

struct Integrator {
    Integrator(Scene& scene);

    long long render(int pixel_num);
    long long render2(int pixel_num);
    long long render3(int pixel_num);
    long long render4(int pixel_num);
    long long render5(int pixel_num);

    Scene scene;
    Texture outputImage;
};
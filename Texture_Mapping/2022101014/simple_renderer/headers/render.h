#pragma once

#include "scene.h"

struct Integrator {
    Integrator(Scene& scene);

    long long render();
    long long render2();
    long long render3();

    Scene scene;
    Texture outputImage;
};
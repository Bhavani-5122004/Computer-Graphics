#include "texture.h"


#define STB_IMAGE_IMPLEMENTATION
#include "stb/stb_image.h"
#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "stb/stb_image_write.h"

#define TINYEXR_IMPLEMENTATION
#include "tinyexr/tinyexr.h"

Texture::Texture(std::string pathToImage)
{
    size_t pos = pathToImage.find(".exr");

    if (pos > pathToImage.length()) {
        this->type = TextureType::UNSIGNED_INTEGER_ALPHA;
        pos = pathToImage.find(".png");

        if (pos > pathToImage.length()) 
            this->loadJpg(pathToImage);
        else
            this->loadPng(pathToImage);
    }
    else {
        this->type = TextureType::FLOAT_ALPHA;
        this->loadExr(pathToImage);
    }
}

void Texture::allocate(TextureType type, Vector2i resolution)
{
    this->resolution = resolution;
    this->type = type;

    if (this->type == TextureType::UNSIGNED_INTEGER_ALPHA) {
        uint32_t* dpointer = (uint32_t*) malloc(this->resolution.x * this->resolution.y * sizeof(uint32_t));
        this->data = (uint64_t)dpointer;
    }
    else if (this->type == TextureType::FLOAT_ALPHA) {
        float* dpointer = (float*)malloc(this->resolution.x * this->resolution.y * 4 * sizeof(float));
        this->data = (uint64_t)dpointer;
    }
}

void Texture::writePixelColor(Vector3f color, int x, int y)
{
    if (this->type == TextureType::UNSIGNED_INTEGER_ALPHA) {
        uint32_t* dpointer = (uint32_t*)this->data;

        uint32_t r = static_cast<uint32_t>(std::min(color.x * 255.0f, 255.f));
        uint32_t g = static_cast<uint32_t>(std::min(color.y * 255.0f, 255.f)) << 8;
        uint32_t b = static_cast<uint32_t>(std::min(color.z * 255.0f, 255.f)) << 16;
        uint32_t a = 255 << 24;

        uint32_t final = r | g | b | a;

        dpointer[y * this->resolution.x + x] = final;
    }
}

/*
Reads the color defined at integer coordinates 'x,y'.
The top left corner of the texture is mapped to '0,0'.
*/
Vector3f Texture::loadPixelColor(int x, int y) {
    Vector3f rval(0.f, 0.f, 0.f);
    if (this->type == TextureType::UNSIGNED_INTEGER_ALPHA) {
        uint32_t* dpointer = (uint32_t*)this->data;

        uint32_t val = dpointer[y * this->resolution.x + x];
        uint32_t r = (val >> 0) & 255u;
        uint32_t g = (val >> 8) & 255u;
        uint32_t b = (val >> 16) & 255u;

        rval.x = r / 255.f;
        rval.y = g / 255.f;
        rval.z = b / 255.f;
    }

    return rval;
}

void Texture::loadJpg(std::string pathToJpg)
{
    Vector2i res;
    int comp;
    unsigned char* image = stbi_load(pathToJpg.c_str(), &res.x, &res.y, &comp, STBI_rgb_alpha);
    int textureID = -1;
    if (image) {
        this->resolution = res;
        this->data = (uint64_t)image;

        /* iw - actually, it seems that stbi loads the pictures
            mirrored along the y axis - mirror them here */
        for (int y = 0; y < res.y / 2; y++) {
            uint32_t* line_y = (uint32_t*)this->data + y * res.x;
            uint32_t* mirrored_y = (uint32_t*)this->data + (res.y - 1 - y) * res.x;
            int mirror_y = res.y - 1 - y;
            for (int x = 0; x < res.x; x++) {
                std::swap(line_y[x], mirrored_y[x]);
            }
        }
    }
    else {
        std::cerr << "Could not load .jpg texture from " << pathToJpg << std::endl;
        std::cerr << stbi_failure_reason() << std::endl;
        exit(1);
    }
}

void Texture::loadPng(std::string pathToPng)
{
    Vector2i res;
    int comp;
    unsigned char* image = stbi_load(pathToPng.c_str(), &res.x, &res.y, &comp, STBI_rgb_alpha);
    int textureID = -1;
    if (image) {
        this->resolution = res;
        this->data = (uint64_t)image;

        /* iw - actually, it seems that stbi loads the pictures
            mirrored along the y axis - mirror them here */
        for (int y = 0; y < res.y / 2; y++) {
            uint32_t* line_y = (uint32_t*)this->data + y * res.x;
            uint32_t* mirrored_y = (uint32_t*)this->data + (res.y - 1 - y) * res.x;
            int mirror_y = res.y - 1 - y;
            for (int x = 0; x < res.x; x++) {
                std::swap(line_y[x], mirrored_y[x]);
            }
        }
    }
    else {
        std::cerr << "Could not load .png texture from " << pathToPng << std::endl;
        std::cerr << stbi_failure_reason() << std::endl;
        exit(1);
    }
}

void Texture::loadExr(std::string pathToExr)
{
    int width;
    int height;
    const char* err = nullptr; // or nullptr in C++11
    
    float* data;
    int ret = LoadEXR(&data, &width, &height, pathToExr.c_str(), &err);
    this->data = (uint64_t)data;

    if (ret != TINYEXR_SUCCESS) {
        std::cerr << "Could not load .exr texture map from " << pathToExr << std::endl;
        exit(1);
    }
    else {
        this->resolution = Vector2i(width, height);
    }
}

void Texture::save(std::string path)
{
    size_t pos = path.find(".png");

    if (pos > path.length()) {
        this->saveExr(path);
    }
    else {
        this->savePng(path);
    }
}

void Texture::saveExr(std::string path)
{
    if (this->type == TextureType::FLOAT_ALPHA) {
        uint64_t hostData = this->data;

        const char* err = nullptr;
        SaveEXR((float*)hostData, this->resolution.x, this->resolution.y, 4, 0, path.c_str(), &err);
        
        if (err == nullptr)
            std::cout << "Saved EXR: " << path << std::endl;
        else
            std::cerr << "Could not save EXR: " << err << std::endl;
    }
    else {
        std::cerr << "Cannot save to EXR: texture is not of type float." << std::endl;
    }
}

void Texture::savePng(std::string path) 
{
    if (this->type == TextureType::UNSIGNED_INTEGER_ALPHA) {
        uint64_t hostData = this->data;
        const uint32_t* data = (const uint32_t*)hostData;

        std::vector<uint32_t> pixels;
        for (int y = 0; y < this->resolution.y; y++) {
            const uint32_t* line = data + (this->resolution.y - 1 - y) * this->resolution.x;
            for (int x = 0; x < this->resolution.x; x++) {
                pixels.push_back(line[x] | (0xff << 24));
            }
        }

        stbi_write_png(path.c_str(), this->resolution.x, this->resolution.y, 4, data, this->resolution.x * sizeof(uint32_t));

        std::cout << "Saved PNG: " << path << std::endl;
    }
    else {
        std::cerr << "Cannot save to PNG: texture is not of type uint32." << std::endl;
    }
}


 Vector3f Texture::nearestNeighbourFetch(Vector2f point, float p1, float p2){
    float x_coord = round( p1* point.x);
    float y_coord = round( p2*point.y);
   
    if(x_coord > p1 || y_coord > p2 || x_coord < 0 || y_coord < 0){
        return Vector3f(0,0,0);
    }
    return loadPixelColor(x_coord,y_coord);

 }


 Vector3f Texture::bilinearFetch(Vector2f point, float f1, float f2){
    
     float x_coord = round(f1* point.x);
    float y_coord = round( f2* point.y);

   

    float x_round_down = floor(f1*point.x);
    float y_round_down = floor( f2*point.y);
    float x_round_up = ceil(f1*point.x);
    float y_round_up = ceil( f2*point.y);

    Vector2f p1 = Vector2f(x_round_down,y_round_up);
    Vector2f p2 = Vector2f(x_round_up,y_round_up);
    Vector2f p3 = Vector2f(x_round_down,y_round_down);
    Vector2f p4 = Vector2f(x_round_up,y_round_down);
    
    if(p1.x > f1 || p1.x < 0 || p1.y > f2 || p1.y < 0){
        return Vector3f (0,0,0);
    }
    if(p2.x > f1 || p2.x < 0 || p2.y > f2 || p2.y < 0){
        return Vector3f (0,0,0);
    }
    if(p3.x > f1 || p3.x < 0 || p3.y > f2 || p3.y < 0){
        return Vector3f (0,0,0);
    }
    if(p4.x > f1 || p4.x < 0 || p4.y > f2 || p4.y < 0){
        return Vector3f (0,0,0);
    }
    
    Vector3f c1 = loadPixelColor(p1.x,p1.y);
    Vector3f c2 = loadPixelColor(p2.x,p2.y);
    Vector3f c3 = loadPixelColor(p3.x,p3.y);
    Vector3f c4 = loadPixelColor(p4.x,p4.y);

    Vector3f cu = (p2.x - x_coord)*c1 + (x_coord - p1.x)*c2;
    Vector3f cl = (p4.x - x_coord)*c3 + (x_coord - p3.x)*c4;

    Vector3f final_color = (y_coord - p3.y)*cu + (p1.y - y_coord)*cl;
    return final_color;

   


    
 }
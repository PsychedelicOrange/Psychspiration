#pragma once
#include <stb_image.h>
#include <FileIO.h>
#include <assimp/texture.h>
#include <gli/gli.hpp>
#include <string>
#include <iostream>
#include <glad/glad.h>
#include <thread>
class Texture {
public:
    unsigned int id;
    std::string type;
    std::string path;
    static std::thread* TextureTransferCPU;
    static unsigned int pbo[2];
    static GLsync is_pbo_free[2];
    static bool first_pbo;
    Texture() {}
    Texture(std::string path) {
    
        id = TextureFromFile(path);
        this->path = path;
    }
    Texture(const aiTexture* tex,std::string typeName)
    {
        this->type = typeName;
        id = TextureEmbedded(tex,typeName);
        this->path = "embedded";
    }
    unsigned int TextureFromFile(std::string path);
    unsigned int uploadTextureCubeMap(std::vector<std::string> facePaths);
    unsigned int HdrTextureFromFile(std::string path);
    void uploadTexture(gli::texture Texture, gli::gl::format Format, GLenum Target);
    void uploadTextureDMA(gli::texture Texture, gli::gl::format Format, GLenum Target);
    unsigned int TextureEmbedded(const aiTexture* texture, std::string typeName);

};

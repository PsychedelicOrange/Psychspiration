#pragma once
#include <stb_image.h>
#include <FileIO.h>
#include <assimp/texture.h>
#include <string>
#include <iostream>
class Texture {
public:
    unsigned int id;
    std::string type;
    std::string path;
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
    unsigned int TextureEmbedded(const aiTexture* texture, std::string typeName);

};

#pragma once
#include <RTexture.h>
#include <assimp/texture.h>
#include <Buff.h>
#include <string>
class RTextureBuilder
{
public:
    RTexture* build(std::string path);
    RTexture build(const aiTexture* texture);
private:
    Buff load(std::string Absolutepath, std::string format);
    Buff decompress(Buff compressed_buffer);
    Buff readIntoBuffer(std::string Absolutepath);
    RTexture* uploadTexture(Buff TextureData, std::string format, RTexture* texture);
    unsigned int HdrTextureFromFile(std::string path);
};
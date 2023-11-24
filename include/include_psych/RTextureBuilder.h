#pragma once
#include <FileIO.h>
#include <RTexture.h>
#include <assimp/texture.h>
#include <glad/glad.h>
#include <zstd.h>
#include <fstream>
#include <STBI_load.h> 
#include <GLI_load.h>  
#include <common.h> // zstd
#include <regex>
#include <vector>

using std::vector;
class RTextureBuilder
{
public:
    RTexture* build(string path, TextureType type);
    RTexture build(const aiTexture* texture);
private:
    static GLI_load gLI_load;
    static STBI_load sTBI_load;

    vector<char> load(string Absolutepath, string format);
    vector<char> load_compressed(string Absolutepath);
    vector<char> load_uncompressed(string Absolutepath);
    RTexture* uploadTexture(const vector<char>& TextureData, string format, RTexture* texture);
};
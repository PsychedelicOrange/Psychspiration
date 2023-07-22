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
	RTexture* build(string path,TextureType type)
	{
        RTexture* texture = new RTexture;
        texture->path = path;
        string Absolutepath = pathResource + '\\' + path.substr(3, path.size() - 3);
        Absolutepath = std::regex_replace(Absolutepath, std::regex("%20"), " ");
        string format = Absolutepath.substr(Absolutepath.find_last_of('.'), Absolutepath.length());
        if (!checkFileExists(Absolutepath))
        {
            std::cout << "Warning : Texture not found -> " << Absolutepath << std::endl;
            Absolutepath = Absolutepath.substr(0, Absolutepath.size() - 3) + "png";
            format = ".png";
            if (!checkFileExists(Absolutepath))
            {
                texture->ID = 0;
                return texture;
            }
        }
        std::vector<char> TextureData = load(Absolutepath,format); // read from file
        return uploadTexture(TextureData,format,texture); // create and assign
	}
	RTexture build (const aiTexture* texture)
	{
		// create and assign gl texture
        sTBI_load(texture);
	}
private:
    static GLI_load gLI_load;
    static STBI_load sTBI_load;

    vector<char> load(string Absolutepath, string format);
    vector<char> load_compressed(string Absolutepath);
    vector<char> load_uncompressed(string Absolutepath);
    RTexture* uploadTexture(const vector<char>& TextureData, string format, RTexture* texture);
};
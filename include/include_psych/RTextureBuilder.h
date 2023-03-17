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
#include <vector>

using std::vector;
class RTextureBuilder
{
public:
	RTexture* build(string path,TextureType type)
	{
        RTexture* texture = new RTexture;
        texture->path = path;
        string Absolutepath = pathResource + '\\' + path.substr(2, path.size() - 2);
        string format = Absolutepath.substr(Absolutepath.find_last_of('.'), Absolutepath.length());
        
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

    vector<char> load(string Absolutepath,string format)
    {
        if (format == ".zstd") // Texture is compressed , decompress 
        {
            return load_compressed(Absolutepath);
        }
        else
        {
            return load_uncompressed(Absolutepath);
        }
    }
    vector<char> load_compressed(string Absolutepath)
    {
        vector<char> ogVector = load_uncompressed(Absolutepath);
        unsigned long long const resultSize = ZSTD_getFrameContentSize(ogVector.data(), ogVector.size());
        CHECK(resultSize != ZSTD_CONTENTSIZE_ERROR, "%s: not compressed by zstd!", Absolutepath);
        CHECK(resultSize != ZSTD_CONTENTSIZE_UNKNOWN, "%s: original size unknown!", Absolutepath);

        std::vector<char> resultVec((size_t)resultSize);

        size_t const dSize = ZSTD_decompress(resultVec.data(), resultSize, ogVector.data(), ogVector.size());
        CHECK_ZSTD(dSize);
        /* When zstd knows the content size, it will error if it doesn't match. */
        CHECK(dSize == resultSize, "Impossible because zstd will check this condition!");

        /* success */
        printf("%25s : %6u -> %7u \n", Absolutepath, (unsigned)ogVector.size(), (unsigned)resultSize);
        return resultVec;
    }
    vector<char> load_uncompressed(string Absolutepath)
    {
        std::ifstream file(Absolutepath, std::ios::binary);
        file.unsetf(std::ios::skipws);

        std::streampos file_size;
        file.seekg(0, std::ios::end);
        file_size = file.tellg();
        file.seekg(0, std::ios::beg);

        std::vector<char> vec;
        vec.reserve(file_size);
        vec.insert(vec.begin(),
            std::istream_iterator<char>(file),
            std::istream_iterator<char>());
        return (vec);
    }
    RTexture* uploadTexture(const vector<char>& TextureData,string format,RTexture* texture)
    {
        if (format == ".dds" || format == ".zstd")
        {
            texture->ID = gLI_load(TextureData);
        }
        else
        {
            texture->ID = sTBI_load(TextureData);
        }
        return texture;
    }
};
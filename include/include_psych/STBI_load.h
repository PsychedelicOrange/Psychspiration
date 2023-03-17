#pragma once
#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>
#include <assimp/texture.h>
#include <iostream>
#include <glad/glad.h>
#include <string>
using std::string;
class STBI_load
{
	int width, height, nrComponents,format;
	unsigned char* data;
	void upload(unsigned int textureID)
	{
        if (data)
        {
            GLenum format = -1;
            if (nrComponents == 1)
                format = GL_RED;
            else if (nrComponents == 3)
                format = GL_RGB;
            else if (nrComponents == 4)
                format = GL_RGBA;
            else
                std::cout << "Couldn't understand image format!";
            glBindTexture(GL_TEXTURE_2D, textureID);
            glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
            glGenerateMipmap(GL_TEXTURE_2D);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
            if (GLAD_GL_EXT_texture_filter_anisotropic)
                glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAX_ANISOTROPY, 4);
        }
        else
        {
            std::cout << "Texture failed to load " << std::endl;
        }
        stbi_image_free(data);
	}

public:
    unsigned int operator()(const vector<char>& TextureData)
    {
        unsigned int textureID;
        glGenTextures(1, &textureID);
        data = stbi_load_from_memory((unsigned char *)TextureData.data(),TextureData.size(), &width, &height, &nrComponents, 0);
        upload(textureID);
    }
    unsigned int operator()(const aiTexture* texture)
    {
        unsigned int textureID;
        glGenTextures(1, &textureID);
        data = stbi_load_from_memory((unsigned char*)texture->pcData, texture->mHeight*texture->mWidth, &width, &height, &nrComponents, 0);
        upload(textureID);
    }
};

#include <RTextureBuilder.h>
#include <filesystem>
#include <FileIO.h>
#include <fstream>
#include <glad/glad.h>
#include <STBI_load.h> 
#include <GLI_load.h>
#include <zstd.h>
#include <common.h> // zstd
RTexture* RTextureBuilder::build(string path)
{
    RTexture* texture = new RTexture;
    texture->path = path;
    string Absolutepath = pathResource + path;
    std::cout << "Trying to load texture at : " << Absolutepath << std::endl;
    string extension = path.substr(path.find_last_of('.')+1);
    //Absolutepath = std::regex_replace(Absolutepath, std::regex("%20"), " ");
    string path_wo_extension = Absolutepath.substr(0, Absolutepath.find_last_of('.') + 1);
    std::cout <<"Path w/o extension: " << path_wo_extension;
    string format = extension;
    if (checkFileExists(path_wo_extension+"zstd"))
    {
        std::cout << "Texture ZSTD found." << std::endl;
        format = "zstd"; 
    }else{
        std::cout << "Warning : Texture ZSTD not found, using given extension ." << extension << std::endl;
    }
    if(format == "zstd"){
        Buff buff = readIntoBuffer(path_wo_extension+format);
        Buff textureData = decompress(buff);
        GLI_load gli = GLI_load(textureData.data,textureData.size);
        texture->ID = gli.upload();
    }else if(format == "dds"){
        GLI_load gli = GLI_load(path_wo_extension+"dds");
        texture->ID = gli.upload();
    }else if(format == "hdr"){
        texture->ID = HdrTextureFromFile(path_wo_extension+"hdr");
    }
    else{
        STBI_load stb = STBI_load(Absolutepath);
        texture->ID = stb.upload();
    }
    return texture;
}
//RTexture RTextureBuilder::build(const aiTexture* texture)
//{
//    // create and assign gl texture
//    sTBI_load(texture);
//}
unsigned int RTextureBuilder::HdrTextureFromFile(std::string path)
{
    //stbi_set_flip_vertically_on_load(true);
    int width, height, nrComponents;
    float* data = stbi_loadf(path.c_str(), &width, &height, &nrComponents, 0);
    unsigned int hdrTexture;
    if (data)
    {
        glGenTextures(1, &hdrTexture);
        glBindTexture(GL_TEXTURE_2D, hdrTexture);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB16F, width, height, 0, GL_RGB, GL_FLOAT, data);
        glGenerateMipmap(GL_TEXTURE_2D);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        stbi_image_free(data);
    }
    else
    {
        std::cout << "Failed to load HDR image." << std::endl;
    }
    return hdrTexture;
}
Buff RTextureBuilder::decompress(Buff compressed_buffer)
{
    std::cout << "File Read." << std::flush;
    unsigned long long const resultSize = ZSTD_getFrameContentSize(compressed_buffer.data, compressed_buffer.size);
    CHECK(resultSize != ZSTD_CONTENTSIZE_ERROR, "%s: not compressed by zstd!");
    CHECK(resultSize != ZSTD_CONTENTSIZE_UNKNOWN, "%s: original size unknown!");

    Buff result((size_t)resultSize);
    //std::vector<char> resultVec((size_t)resultSize);

    size_t const dSize = ZSTD_decompress(result.data, resultSize, compressed_buffer.data, compressed_buffer.size);
    CHECK_ZSTD(dSize);
    /* When zstd knows the content size, it will error if it doesn't match. */
    CHECK(dSize == resultSize, "Impossible because zstd will check this condition!");

    /* success */
    //printf("%25s : %6u -> %7u \n", Absolutepath, (unsigned)ogVector.size(), (unsigned)resultSize);
    return result;
}

Buff RTextureBuilder::readIntoBuffer(string Absolutepath)
{   
    std::ifstream file(Absolutepath, std::ios::in | std::ios::binary);
    file.unsetf(std::ios::skipws);
    std::uintmax_t filesize = std::filesystem::file_size(Absolutepath);
    Buff inputfile(filesize);
    std::cout << "File size detected : " << filesize << std::flush;
    file.read(inputfile.data,inputfile.size);
    return inputfile;
}

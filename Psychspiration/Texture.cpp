#include <Texture.h>
#include <gli/gli.hpp>
unsigned int Texture::TextureFromFile(std::string path)
{
    path = relativePath + '\\' + "Resources" + '\\' + path.substr(2, path.size() - 2);
    //std::cout << "\nPATH: " << path;
    unsigned int textureID;
    glGenTextures(1, &textureID);
    //stbi_set_flip_vertically_on_load(true);
    int width, height, nrComponents;
    std::string format = path.substr(path.find_last_of('.'), path.length());

    if (format == ".dds") {

        gli::texture Texture = gli::load(path);

        if (Texture.empty())
            return 0;

        //std::cout << "hello";
        gli::gl GL(gli::gl::PROFILE_GL33);
        gli::gl::format const Format = GL.translate(Texture.format(), Texture.swizzles());
        GLenum Target = GL.translate(Texture.target());
        std::cout << Texture.levels() << " mips, " << Texture.faces() << " faces, " << Texture.layers() << "\n";
        GLuint TextureName = 0;
        glGenTextures(1, &TextureName);
        glBindTexture(Target, TextureName);
        glTexParameteri(Target, GL_TEXTURE_BASE_LEVEL, Texture.base_level());
        glTexParameteri(Target, GL_TEXTURE_MAX_LEVEL, Texture.max_level());
        glTexParameteri(Target, GL_TEXTURE_SWIZZLE_R, Format.Swizzles[0]);
        glTexParameteri(Target, GL_TEXTURE_SWIZZLE_G, Format.Swizzles[1]);
        glTexParameteri(Target, GL_TEXTURE_SWIZZLE_B, Format.Swizzles[2]);
        glTexParameteri(Target, GL_TEXTURE_SWIZZLE_A, Format.Swizzles[3]);
        glTexParameteri(Target, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
        glTexParameteri(Target, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTexParameteri(Target, GL_TEXTURE_MAX_ANISOTROPY, 4);
        glm::tvec3<GLsizei> const Extent(Texture.extent());
        GLsizei const FaceTotal = static_cast<GLsizei>(Texture.layers() * Texture.faces());

        switch (Texture.target())
        {
        case gli::TARGET_1D:
            glTexStorage1D(
                Target, static_cast<GLint>(Texture.levels()), Format.Internal, Extent.x);
            break;
        case gli::TARGET_1D_ARRAY:
        case gli::TARGET_2D:
        case gli::TARGET_CUBE:
            glTexStorage2D(
                Target, static_cast<GLint>(Texture.levels()), Format.Internal,
                Extent.x, Texture.target() == gli::TARGET_2D ? Extent.y : FaceTotal);
            break;
        case gli::TARGET_2D_ARRAY:
        case gli::TARGET_3D:
        case gli::TARGET_CUBE_ARRAY:
            glTexStorage3D(
                Target, static_cast<GLint>(Texture.levels()), Format.Internal,
                Extent.x, Extent.y,
                Texture.target() == gli::TARGET_3D ? Extent.z : FaceTotal);
            break;
        default:
            assert(0);
            break;
        }

        for (std::size_t Layer = 0; Layer < Texture.layers(); ++Layer)
            for (std::size_t Face = 0; Face < Texture.faces(); ++Face)
                for (std::size_t Level = 0; Level < Texture.levels(); ++Level)
                {
                    GLsizei const LayerGL = static_cast<GLsizei>(Layer);
                    glm::tvec3<GLsizei> Extent(Texture.extent(Level));
                    Target = gli::is_target_cube(Texture.target())
                        ? static_cast<GLenum>(GL_TEXTURE_CUBE_MAP_POSITIVE_X + Face)
                        : Target;

                    switch (Texture.target())
                    {
                    case gli::TARGET_1D:
                        if (gli::is_compressed(Texture.format()))
                            glCompressedTexSubImage1D(
                                Target, static_cast<GLint>(Level), 0, Extent.x,
                                Format.Internal, static_cast<GLsizei>(Texture.size(Level)),
                                Texture.data(Layer, Face, Level));
                        else
                            glTexSubImage1D(
                                Target, static_cast<GLint>(Level), 0, Extent.x,
                                Format.External, Format.Type,
                                Texture.data(Layer, Face, Level));
                        break;
                    case gli::TARGET_1D_ARRAY:
                    case gli::TARGET_2D:
                    case gli::TARGET_CUBE:
                        if (gli::is_compressed(Texture.format()))
                            glCompressedTexSubImage2D(
                                Target, static_cast<GLint>(Level),
                                0, 0,
                                Extent.x,
                                Texture.target() == gli::TARGET_1D_ARRAY ? LayerGL : Extent.y,
                                Format.Internal, static_cast<GLsizei>(Texture.size(Level)),
                                Texture.data(Layer, Face, Level));
                        else
                            glTexSubImage2D(
                                Target, static_cast<GLint>(Level),
                                0, 0,
                                Extent.x,
                                Texture.target() == gli::TARGET_1D_ARRAY ? LayerGL : Extent.y,
                                Format.External, Format.Type,
                                Texture.data(Layer, Face, Level));
                        break;
                    case gli::TARGET_2D_ARRAY:
                    case gli::TARGET_3D:
                    case gli::TARGET_CUBE_ARRAY:
                        if (gli::is_compressed(Texture.format()))
                            glCompressedTexSubImage3D(
                                Target, static_cast<GLint>(Level),
                                0, 0, 0,
                                Extent.x, Extent.y,
                                Texture.target() == gli::TARGET_3D ? Extent.z : LayerGL,
                                Format.Internal, static_cast<GLsizei>(Texture.size(Level)),
                                Texture.data(Layer, Face, Level));
                        else
                            glTexSubImage3D(
                                Target, static_cast<GLint>(Level),
                                0, 0, 0,
                                Extent.x, Extent.y,
                                Texture.target() == gli::TARGET_3D ? Extent.z : LayerGL,
                                Format.External, Format.Type,
                                Texture.data(Layer, Face, Level));
                        break;
                    default: assert(0); break;
                    }
                }
        return TextureName;

    }
    else {
        unsigned char* data = stbi_load(path.c_str(), &width, &height, &nrComponents, 0);

        if (data)
        {
            GLenum format;
            if (nrComponents == 1)
                format = GL_RED;
            else if (nrComponents == 3)
                format = GL_RGB;
            else if (nrComponents == 4)
                format = GL_RGBA;

            glBindTexture(GL_TEXTURE_2D, textureID);
            glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
            glGenerateMipmap(GL_TEXTURE_2D);

            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAX_ANISOTROPY, 4);
            stbi_image_free(data);
        }
        else
        {
            // std::cout << "Texture failed to load at path: " << path << std::endl;
            stbi_image_free(data);
        }

        return textureID;
    }
}
unsigned int Texture::TextureEmbedded(const aiTexture* texture, std::string typeName)
{

    unsigned char* image_data = nullptr;
    unsigned char* read_image_data = nullptr;
    int  width, height, components_per_pixel;
    unsigned int textureID;
    glGenTextures(1, &textureID);
    glBindTexture(GL_TEXTURE_2D, textureID);
    //glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
    if (texture->mHeight == 0)
    {

        image_data = stbi_load_from_memory(reinterpret_cast<unsigned char*>(texture->pcData), texture->mWidth, &width, &height, &components_per_pixel, 0);

    }


    else
    {
        image_data = stbi_load_from_memory(reinterpret_cast<unsigned char*>(texture->pcData), texture->mWidth * texture->mHeight, &width, &height, &components_per_pixel, 0);
    }

    if (components_per_pixel == 3)
    {
        // std::cout << "hello";
        if (typeName == "texture_normal" || typeName == "texture_specular" || typeName == "texture_roughmetal")
        {
            glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, image_data);
        }
        else
        {
            glTexImage2D(GL_TEXTURE_2D, 0, GL_SRGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, image_data);
        }
    }
    else if (components_per_pixel == 4)
    {
        if (typeName == "texture_normal" || typeName == "texture_specular" || typeName == "texture_roughmetal")
        {
            glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, image_data);
        }
        else
        {
            glTexImage2D(GL_TEXTURE_2D, 0, GL_SRGB_ALPHA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, image_data);
        }
    }
    glGenerateMipmap(GL_TEXTURE_2D);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    if (GLAD_GL_EXT_texture_filter_anisotropic)
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAX_ANISOTROPY, 4);
    stbi_image_free(image_data);
    return textureID;

}

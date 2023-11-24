#pragma once
#include <gli/gli.hpp> 
#include <glad/glad.h>
#include <vector>
using std::vector;
class GLI_load{

	gli::gl GL = gli::gl((gli::gl::PROFILE_GL33));
    gli::gl::format Format;
    unsigned int Target;
    gli::texture Texture;

    void uploadTexture()
    {
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
    }
public:
	unsigned int operator()(const vector<char>& TextureData)
	{

		Texture = gli::load(TextureData.data(), TextureData.size());
		Format = GL.translate(Texture.format(), Texture.swizzles());
		Target = GL.translate(Texture.target());
        std::cout << "\nTexture Format: " << Texture.format();
		// Generate TextureName and set tex parameters
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

		// glTexStorage Call
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

        // without PBO
        uploadTexture();
        return TextureName;
        // reference for with PBO  check commit 904f8690bacb88c00b8e7392d118d529b0c7819b Texture.cpp
	}
    
};
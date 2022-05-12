#include <Model.h>
#include <FileIO.h>
#include <assimp/Importer.hpp>
#include <assimp/postprocess.h>
#include <assimp/pbrmaterial.h>
#include <gli/gli.hpp>
#include <stb_image.h>
#include <glad/glad.h>
#include <iostream>
Model::Model(std::string const& path,unsigned int* pbo, bool gamma ) : gammaCorrection(gamma)
{
    loadModel(path);
}
Model::Model()
{
    
}
const aiScene* Model::getpath(std::string path)
{
    //std::cout << "inside getpath()" << std::endl;
    return loadModel(path);
}
// draws the model, and thus all its meshes
void Model::Draw(Shader& shader)
{
    for (unsigned int i = 0; i < meshes.size(); i++)
        meshes[i].Draw(shader);
}
const aiScene* Model::loadModel(const std::string& path)
{
    //std::cout << "inside loadmodel()\t no of meshes: " << std::endl;
    // read file via ASSIMP
    std::string relativePath = getRelativePath();
    Assimp::Importer importer;
    //std::cout << relativePath + path;
    const aiScene* scene = importer.ReadFile(relativePath+ path, aiProcess_Triangulate | aiProcess_FlipUVs | aiProcess_CalcTangentSpace );
    // check for errors
    if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode) // if is Not Zero
    {
        std::cout << "ERROR::ASSIMP:: " << importer.GetErrorString() << std::endl;
        return scene;
    }
    // retrieve the directory path of the filepath
    directory = relativePath + path.substr(0, path.find_last_of('\\'));
    format = path.substr(path.find_last_of('.'), path.length());
    // process ASSIMP's root node recursively
    processNode(scene->mRootNode, scene);
    return scene;
}

// processes a node in a recursive fashion. Processes each individual mesh located at the node and repeats this process on its children nodes (if any).
void Model::processNode(aiNode* node, const aiScene* scene)
{
    // process each mesh located at the current node
    for (unsigned int i = 0; i < node->mNumMeshes; i++)
    {
        // the node object only contains indices to index the actual objects in the scene. 
        // the scene contains all the data, node is just to keep stuff organized (like relations between nodes).
        aiMesh* mesh = scene->mMeshes[node->mMeshes[i]];
        meshes.push_back(processMesh(mesh, scene));
    }
    // after we've processed all of the meshes (if any) we then recursively process each of the children nodes
    for (unsigned int i = 0; i < node->mNumChildren; i++)
    {
        processNode(node->mChildren[i], scene);
    }

}

Mesh Model::processMesh(aiMesh* mesh, const aiScene* scene)
{
    // std::cout << "(inside processMesh) " << std::endl;

     // data to fill
    std::vector<Vertex> vertices;
    std::vector<unsigned int> indices;
    std::vector<Texture> textures;
    // walk through each of the mesh's vertices
    for (unsigned int i = 0; i < mesh->mNumVertices; i++)
    {
        Vertex vertex;
        glm::vec3 vector; // we declare a placeholder vector since assimp uses its own vector class that doesn't directly convert to glm's vec3 class so we transfer the data to this placeholder glm::vec3 first.
        // positions
        vector.x = mesh->mVertices[i].x;
        vector.y = mesh->mVertices[i].y;
        vector.z = mesh->mVertices[i].z;
        vertex.Position = vector;
        // normals
        if (mesh->HasNormals())
        {
            vector.x = mesh->mNormals[i].x;
            vector.y = mesh->mNormals[i].y;
            vector.z = mesh->mNormals[i].z;
            vertex.Normal = vector;
        }
        // texture coordinates
        if (mesh->mTextureCoords[0]) // does the mesh contain texture coordinates?
        {
            glm::vec2 vec;
            // a vertex can contain up to 8 different texture coordinates. We thus make the assumption that we won't 
            // use models where a vertex can have multiple texture coordinates so we always take the first set (0).
            vec.x = mesh->mTextureCoords[0][i].x;
            vec.y = mesh->mTextureCoords[0][i].y;

            vertex.TexCoords = vec;
            // tangent
            vector.x = mesh->mTangents[i].x;
            vector.y = mesh->mTangents[i].y;
            vector.z = mesh->mTangents[i].z;
            vertex.Tangent = vector;
            // bitangent

            vector.x = mesh->mBitangents[i].x;
            vector.y = mesh->mBitangents[i].y;
            vector.z = mesh->mBitangents[i].z;
            vertex.Bitangent = vector;

        }
        else
            vertex.TexCoords = glm::vec2(0.0f, 0.0f);

        vertices.push_back(vertex);

    }

    // now walk through each of the mesh's faces (a face is a mesh its triangle) and retrieve the corresponding vertex indices.
    for (unsigned int i = 0; i < mesh->mNumFaces; i++)
    {
        aiFace face = mesh->mFaces[i];
        // retrieve all indices of the face and store them in the indices vector
        for (unsigned int j = 0; j < face.mNumIndices; j++)
            indices.push_back(face.mIndices[j]);
    }
    /*
    for (int i = 0; i < vertices.size(); i++)
    {
        std::cout << "For Vertex " << i;
        std::cout <<"-Position :" << vertices[i].Position.x <<" " << vertices[i].Position.y <<" " << vertices[i].Position.z;
        std::cout << "normals :"<<vertices[i].Normal.x << " " << vertices[i].Normal.y << " " << vertices[i].Normal.z ;
        std::cout << "texcoords :"<<vertices[i].TexCoords.x << " " << vertices[i].TexCoords.y ;
        std::cout << "Tangents :" << vertices[i].Tangent.x << " " << vertices[i].Tangent.y << " " << vertices[i].Tangent.z;
        std::cout << "biTangents :" << vertices[i].Bitangent.x << " " << vertices[i].Bitangent.y << " " << vertices[i].Bitangent.z << std::endl;
    }
    */
    // process materials
    aiMaterial* material = scene->mMaterials[mesh->mMaterialIndex];

    // we assume a convention for sampler names in the shaders. Each diffuse texture should be named
    // as 'texture_diffuseN' where N is a sequential number ranging from 1 to MAX_SAMPLER_NUMBER. 
    // Same applies to other texture as the following list summarizes:
    // diffuse: texture_diffuseN
    // specular: texture_specularN
    // normal: texture_normalN

    if (this->format == ".obj" || this->format == ".gltf")
    {

        // 1. diffuse maps
        std::vector<Texture> diffuseMaps = loadMaterialTextures(material, aiTextureType_DIFFUSE, "texture_diffuse");
        textures.insert(textures.end(), diffuseMaps.begin(), diffuseMaps.end());
        // 2. specular maps
        std::vector<Texture> specularMaps = loadMaterialTextures(material, aiTextureType_SPECULAR, "texture_specular");
        textures.insert(textures.end(), specularMaps.begin(), specularMaps.end());
        // 3. normal maps
        std::vector<Texture> normalMaps = loadMaterialTextures(material, aiTextureType_NORMALS, "texture_normal");
        textures.insert(textures.end(), normalMaps.begin(), normalMaps.end());
        // 4. height maps
        std::vector<Texture> heightMaps = loadMaterialTextures(material, aiTextureType_AMBIENT, "texture_ao");
        textures.insert(textures.end(), heightMaps.begin(), heightMaps.end());

        std::vector<Texture> roughmetalMaps = loadMaterialTextures(material, aiTextureType_UNKNOWN, "texture_roughmetal");
        textures.insert(textures.end(), roughmetalMaps.begin(), roughmetalMaps.end());

    }
    //
    if (this->format == ".glb")
    {

        // 1. diffuse maps
        std::vector<Texture> diffuseMaps = loadMaterialTexturesEmbedded(material, scene, aiTextureType_DIFFUSE, "texture_diffuse");
        textures.insert(textures.end(), diffuseMaps.begin(), diffuseMaps.end());
        // 2. specular maps
        std::vector<Texture> specularMaps = loadMaterialTexturesEmbedded(material, scene, aiTextureType_SPECULAR, "texture_specular");
        textures.insert(textures.end(), specularMaps.begin(), specularMaps.end());
        // 3. normal maps
        std::vector<Texture> normalMaps = loadMaterialTexturesEmbedded(material, scene, aiTextureType_NORMALS, "texture_normal");
        textures.insert(textures.end(), normalMaps.begin(), normalMaps.end());
        // 4. height maps
        std::vector<Texture> heightMaps = loadMaterialTexturesEmbedded(material, scene, aiTextureType_AMBIENT, "texture_height");
        textures.insert(textures.end(), heightMaps.begin(), heightMaps.end());
        // 5. roughness + metalness maps
        std::vector<Texture> roughmetalMaps = loadMaterialTexturesEmbedded(material, scene, aiTextureType_UNKNOWN, "texture_roughmetal");
        textures.insert(textures.end(), roughmetalMaps.begin(), roughmetalMaps.end());
        // return a mesh object created from the extracted mesh data

    }
    

    return Mesh(vertices, indices, textures);

}

// checks all material textures of a given type and loads the textures if they're not loaded yet.
// the required info is returned as a Texture struct.
std::vector<Texture> Model::loadMaterialTextures(aiMaterial* mat, aiTextureType type, std::string typeName)
{
    std::vector<Texture> textures;
    for (unsigned int i = 0; i < mat->GetTextureCount(type); i++)
    {
        aiString str;
        mat->GetTexture(type, i, &str);

        // check if texture was loaded before and if so, continue to next iteration: skip loading a new texture
        bool skip = false;
        for (unsigned int j = 0; j < textures_loaded.size(); j++)
        {
            if (std::strcmp(textures_loaded[j].path.data(), str.C_Str()) == 0)
            {
                textures.push_back(textures_loaded[j]);
                skip = true; // a texture with the same filepath has already been loaded, continue to next one. (optimization)
                break;
            }
        }
        if (!skip)
        {   // if texture hasn't been loaded already, load it
            Texture texture;
            texture.id = TextureFromFile(str.C_Str(), this->directory);
            texture.type = typeName;
            //std::cout << std::endl << "Type : " << texture.type;
            texture.path = str.C_Str();
            textures.push_back(texture);
            textures_loaded.push_back(texture);  // store it as texture loaded for entire model, to ensure we won't unnecesery load duplicate textures.
        }
    }
    for (int i = 0; i < textures.size(); i++)
    {
        std::cout << i << "'st Texture id , path , type = ";
        std::cout << textures[i].id << " ";
        std::cout << textures[i].type << " ";
        std::cout << textures[i].path << std::endl;
    }
    return textures;
}



std::vector<Texture> Model::loadMaterialTexturesEmbedded(aiMaterial* mat, const aiScene* scene, aiTextureType type, std::string typeName)
{
    std::vector<Texture> textures;
    Texture textured;
    int NrTexture = mat->GetTextureCount(type);
    //std::cout << NrTexture;
    if (NrTexture == 2)
    {
        NrTexture = 1;
    }


    //std::cout << "NrTexture for "<<typeName<<":" << NrTexture<<std::endl;
    for (unsigned int i = 0; i < (NrTexture); i++)
    {

        aiString texture_file;

        mat->Get(AI_MATKEY_TEXTURE(type, i), texture_file);

        if (auto texture = scene->GetEmbeddedTexture(texture_file.C_Str())) {

            textured.id = TextureEmbedded(texture, typeName);
            //textured.id = hi();
            textured.type = typeName;
            //std::cout << std::endl << "Type : " << textured.type;
            textured.path = "embedded";
            textures.push_back(textured);

        }
        else {
            //regular file, check if it exists and read it
            std::cout << "cant recognize embedded texture";
        }


    }

   /* for (int i = 0; i < textures.size(); i++)
    {
        std::cout << i << "'st Texture id , path , type = ";
        std::cout << textures[i].id << " ";
        std::cout << textures[i].type << " ";
        std::cout << textures[i].path << std::endl;
    }*/


    return textures;
}

unsigned int Model::TextureFromFile(const char* path, const std::string& directory)
{
    std::string filename = std::string(path);
    filename = directory + '\\' + filename;
    unsigned int textureID;
    glGenTextures(1, &textureID);
    //stbi_set_flip_vertically_on_load(true);
    int width, height, nrComponents;
    std::string format = filename.substr(filename.find_last_of('.'), filename.length());
    
    //copy data into pbo 
    //Model* helmet = new Model();

    //glBufferData(GL_PIXEL_UNPACK_BUFFER, sizeoftexture, pointer_to_data, GL_STREAM_DRAW);
    //GLubyte* ptr = (GLubyte*)glMapBuffer(GL_PIXEL_UNPACK_BUFFER, GL_WRITE_ONLY);
    if (format == ".dds") {
        //glGenBuffers(1, pbo);
        //glBindBuffer(GL_PIXEL_UNPACK_BUFFER, *pbo);
        gli::texture Texture = gli::load(filename);
        //glUnmapBuffer(GL_PIXEL_UNPACK_BUFFER);
        // creating pbo 
        // unsigned int pbo;
    
        //glBufferData(GL_PIXEL_UNPACK_BUFFER, sizeoftexture, pointer_to_data, GL_STREAM_DRAW);
        //GLubyte* ptr = (GLubyte*)glMapBuffer(GL_PIXEL_UNPACK_BUFFER, GL_WRITE_ONLY);

        if (Texture.empty())
            return 0;
        
        //std::cout << "hello";
        gli::gl GL(gli::gl::PROFILE_GL33);
        gli::gl::format const Format = GL.translate(Texture.format(), Texture.swizzles());
        GLenum Target = GL.translate(Texture.target());
        std::cout << Texture.levels()<<" mips, "<< Texture.faces()<<" faces, "<<Texture.layers()<<"\n";
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
        unsigned char* data = stbi_load(filename.c_str(), &width, &height, &nrComponents, 0);

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
unsigned int Model::TextureEmbedded(const aiTexture * texture, std::string typeName)
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
    if(GLAD_GL_EXT_texture_filter_anisotropic)
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAX_ANISOTROPY, 4);
    stbi_image_free(image_data);
    return textureID;

}

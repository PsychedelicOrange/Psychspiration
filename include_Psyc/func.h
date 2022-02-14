#pragma once
#include<glad/glad.h>
#include <iostream>
#include <string>
#include <fstream>
#include <sstream>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <vector>
#include <stb_image.h>
#include <assimp/Importer.hpp>
#include <assimp/version.h>
#include <assimp/scene.h>
#include <assimp/postprocess.h>
#include <math.h>

void glfwwindowinnit();
int errorhandle(GLFWwindow* window);
int gladinnit();
unsigned int TextureFromFile(const char* path, const std::string& directory, bool gamma);
unsigned int TextureEmbedded(const aiTexture* texture, std::string typeName);
std::vector<std::string> split(const std::string& s, char delim);

struct PointLight {
    std::string name;
    glm::vec3 position;
    glm::vec3 color;
    float power;
    float constant;
    float linear;
    float quadratic;
    float size;
    float use_shadows;
};
struct GPULight {
    glm::vec4 position;
    glm::vec4 color;
    unsigned int enabled;
    float intensity;
    float range;
    float padding;
};
struct DirectionalLight {
    glm::vec3 direction;
    glm::vec3 color;
};
struct SpotLight {
    glm::vec3 position;
    glm::vec3 direction;
    float angleInnerCone;
    float angleOuterCone;
    float constant;
    float linear;
    float quadratic;
    glm::vec3 color;
};

class Shader
{
public:
    unsigned int ID;
    static std::string shaderPath;
    // constructor generates the shader on the fly
    // ------------------------------------------------------------------------
    Shader(std::string vertexPath , std::string fragmentPath, std::string geometryPath );
    void use();
    void delete_();
    void setBool(const std::string& name, bool value) const;   
    void setInt(const std::string& name, int value) const;
    void setFloat(const std::string& name, float value) const;
    void setMat4(const std::string& name, const glm::mat4 value) const;
    void setVec3(const std::string& name, const glm::vec3& value) const;
    void setVec3(const std::string& name, float x, float y, float z) const;
    
private:
    // utility function for checking shader compilation/linking errors.
    // ------------------------------------------------------------------------
    void checkCompileErrors(unsigned int shader, std::string type,std::string path)
    {
        int success;
        char infoLog[1024];
        if (type != "PROGRAM")
        {
            glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
            if (!success)
            {
                glGetShaderInfoLog(shader, 1024, NULL, infoLog);
                std::cout << "ERROR IN SHADER" <<path<<"::SHADER_COMPILATION_ERROR of type : " << type << "\n" << infoLog << "\n-- -------------------------------------------------- - -- " << std::endl;
            }
        }
        else
        {
            glGetProgramiv(shader, GL_LINK_STATUS, &success);
            if (!success)
            {
                glGetProgramInfoLog(shader, 1024, NULL, infoLog);
                std::cout << "ERROR::PROGRAM_LINKING_ERROR of type: " << type << "\n" << infoLog << "\n -- --------------------------------------------------- -- " << std::endl;
            }
        }
    }
};

// Defines several possible options for camera movement. Used as abstraction to stay away from window-system specific input methods
enum Camera_Movement {
    FORWARD,
    BACKWARD,
    LEFT,
    RIGHT
};

// Default camera values
const float YAW = -90.0f;
const float PITCH = 0.0f;
const float SPEED = 2.5f;
const float SENSITIVITY = 0.1f;
const float ZOOM = 45.0f;


// An abstract camera class that processes input and calculates the corresponding Euler Angles, Vectors and Matrices for use in OpenGL
class Camera
{
public:
    // camera Attributes
    glm::vec3 Position;
    glm::vec3 Front;
    glm::vec3 Up;
    glm::vec3 Right;
    glm::vec3 WorldUp;
    // euler Angles
    float Yaw;
    float Pitch;
    // camera options
    float MovementSpeed;
    float MouseSensitivity;
    float Zoom;

    // constructor with vectors
    Camera(glm::vec3 position = glm::vec3(-9.0f, 1.0f, 1.0f), glm::vec3 up = glm::vec3(0.0f, 1.0f, 0.0f), float yaw = YAW, float pitch = PITCH) : Front(glm::vec3(0.0f, 0.0f, -1.0f)), MovementSpeed(SPEED), MouseSensitivity(SENSITIVITY), Zoom(ZOOM)
    {
        Position = position;
        WorldUp = up;
        Yaw = yaw;
        Pitch = pitch;
        updateCameraVectors();
    }
    // constructor with scalar values
    Camera(float posX, float posY, float posZ, float upX, float upY, float upZ, float yaw, float pitch) : Front(glm::vec3(0.0f, 0.0f, -1.0f)), MovementSpeed(SPEED), MouseSensitivity(SENSITIVITY), Zoom(ZOOM)
    {
        Position = glm::vec3(posX, posY, posZ);
        WorldUp = glm::vec3(upX, upY, upZ);
        Yaw = yaw;
        Pitch = pitch;
        updateCameraVectors();
    }

    // returns the view matrix calculated using Euler Angles and the LookAt Matrix
    glm::mat4 GetViewMatrix()
    {
        return glm::lookAt(Position, Position + Front, Up);
    }

    // processes input received from any keyboard-like input system. Accepts input parameter in the form of camera defined ENUM (to abstract it from windowing systems)
    void ProcessKeyboard(Camera_Movement direction, float deltaTime)
    {
        float velocity = MovementSpeed * deltaTime;
        if (direction == FORWARD)
            Position += Front * velocity;
        if (direction == BACKWARD)
            Position -= Front * velocity;
        if (direction == LEFT)
            Position -= Right * velocity;
        if (direction == RIGHT)
            Position += Right * velocity;
    }

    // processes input received from a mouse input system. Expects the offset value in both the x and y direction.
    void ProcessMouseMovement(float xoffset, float yoffset, GLboolean constrainPitch = true)
    {
        xoffset *= MouseSensitivity;
        yoffset *= MouseSensitivity;

        Yaw += xoffset;
        Pitch += yoffset;

        // make sure that when pitch is out of bounds, screen doesn't get flipped
        if (constrainPitch)
        {
            if (Pitch > 89.0f)
                Pitch = 89.0f;
            if (Pitch < -89.0f)
                Pitch = -89.0f;
        }

        // update Front, Right and Up Vectors using the updated Euler angles
        updateCameraVectors();
    }

    // processes input received from a mouse scroll-wheel event. Only requires input on the vertical wheel-axis
    void ProcessMouseScroll(float yoffset)
    {
        Zoom -= (float)yoffset;
        if (Zoom < 1.0f)
            Zoom = 1.0f;
        if (Zoom > 45.0f)
            Zoom = 45.0f;
    }

private:
    // calculates the front vector from the Camera's (updated) Euler Angles
    void updateCameraVectors()
    {
        // calculate the new Front vector
        glm::vec3 front;
        front.x = cos(glm::radians(Yaw)) * cos(glm::radians(Pitch));
        front.y = sin(glm::radians(Pitch));
        front.z = sin(glm::radians(Yaw)) * cos(glm::radians(Pitch));
        Front = glm::normalize(front);
        // also re-calculate the Right and Up vector
        Right = glm::normalize(glm::cross(Front, WorldUp));  // normalize the vectors, because their length gets closer to 0 the more you look up or down which results in slower movement.
        Up = glm::normalize(glm::cross(Right, Front));
    }
};

unsigned int TextureFromFile(const char* path, const std::string& directory, bool gamma = false);

struct Vertex {
    // position
    glm::vec3 Position;
    // normal
    glm::vec3 Normal;
    // texCoords
    glm::vec2 TexCoords;
    // tangent
    glm::vec3 Tangent;
    // bitangent
    glm::vec3 Bitangent;
};

struct Texture {
    unsigned int id;
    std::string type;
    std::string path;
};

class Mesh {
public:
    // mesh Data
    std::vector<Vertex>       vertices;
    std::vector<unsigned int> indices;
    std::vector<Texture>      textures;
    unsigned int VAO;

    // constructor
    Mesh(std::vector<Vertex> vertices, std::vector<unsigned int> indices, std::vector<Texture> textures)
    {
        this->vertices = vertices;
        this->indices = indices;
        this->textures = textures;
        
        // now that we have all the required data, set the vertex buffers and its attribute pointers.
        setupMesh();
    }

    // render the mesh
    
    void Draw(Shader& shader)
    {
        // bind appropriate textures
        unsigned int diffuseNr = 1;
        unsigned int specularNr = 1;
        unsigned int normalNr = 1;
        unsigned int heightNr = 1;
        unsigned int roughmetalNr = 1;

        
        for (unsigned int i = 0; i < (textures.size()); i++)
        {
            
            // retrieve texture number (the N in diffuse_textureN)
            std::string number;
            std::string name = textures[i].type;
            if (name == "texture_diffuse")
            {
                number = std::to_string(diffuseNr++);
            }
            else if (name == "texture_specular")
            {
                number = std::to_string(specularNr++); 
            }
            else if (name == "texture_normal")
            {
                number = std::to_string(normalNr++);   
            }
            else if (name == "texture_height")
            {
                number = std::to_string(heightNr++); // transfer unsigned int to stream
            }
            else if (name == "texture_roughmetal")
            {
                number = std::to_string(roughmetalNr++);
            }
            // now set the sampler to the correct texture unit 
            // glUniform1i(glGetUniformLocation(shader.ID, (name + number).c_str()), i);
            shader.setInt(("material." + name + number).c_str(), i);
            glActiveTexture(GL_TEXTURE0 + i);
           //std::cout << " " << (name + number).c_str() << " " << i << std::endl;
            if (normalNr > 1)
            {
                shader.setBool("existnormals", 1);
              /*shader.setInt("material.texture_normal1", 1);
                glActiveTexture(GL_TEXTURE1);
                glBindTexture(GL_TEXTURE_2D, textures[i].id);*/
            }
            else
            {
                shader.setBool("existnormals", 0);
            }
            //shader.setInt((name + number).c_str(), i);
            //std::cout << (GL_TEXTURE0);
            // and finally bind the texture
            glBindTexture(GL_TEXTURE_2D, textures[i].id );
        }
        
       
        // draw mesh
        glBindVertexArray(VAO);
        glDrawElements(GL_TRIANGLES, indices.size(), GL_UNSIGNED_INT, 0);
        glBindVertexArray(0);
        glBindTexture(GL_TEXTURE_2D, 0); 
        // always good practice to set everything back to defaults once configured.
        glActiveTexture(GL_TEXTURE0);
    }

private:
    // render data 
    unsigned int VBO, EBO;

    // initializes all the buffer objects/arrays
    void setupMesh()
    {
        // create buffers/arrays
        glGenVertexArrays(1, &VAO);
        glGenBuffers(1, &VBO);
        glGenBuffers(1, &EBO);

        glBindVertexArray(VAO);
        // load data into vertex buffers
        glBindBuffer(GL_ARRAY_BUFFER, VBO);
        // A great thing about structs is that their memory layout is sequential for all its items.
        // The effect is that we can simply pass a pointer to the struct and it translates perfectly to a glm::vec3/2 array which
        // again translates to 3/2 floats which translates to a byte array.
        glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(Vertex), &vertices[0], GL_STATIC_DRAW);

        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int), &indices[0], GL_STATIC_DRAW);

        // set the vertex attribute pointers
        // vertex Positions
        glEnableVertexAttribArray(0);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)0);
        // vertex normals
        glEnableVertexAttribArray(1);
        glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, Normal));
        // vertex texture coords
        glEnableVertexAttribArray(2);
        glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, TexCoords));
        // vertex tangent
        glEnableVertexAttribArray(3);
        glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, Tangent));
        // vertex bitangent
        glEnableVertexAttribArray(4);
        glVertexAttribPointer(4, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, Bitangent));
        

        glBindVertexArray(0);
    }
};
class Model
{
public:
    // model data 
    std::vector<Texture> textures_loaded;	// stores all the textures loaded so far, optimization to make sure textures aren't loaded more than once.
    std::vector<Mesh> meshes;
    std::string directory;
    std::string format;
    bool gammaCorrection;

    // constructor, expects a filepath to a 3D model.
    Model(std::string const& path, bool gamma = false) : gammaCorrection(gamma)
    {
        loadModel(path);
    }
    Model()
    {

    }
    const aiScene* getpath(std::string path)
    {
        std::cout << "inside getpath()" << std::endl;
        return loadModel(path);
    }
    // draws the model, and thus all its meshes
    void Draw(Shader& shader)
    {
        for (unsigned int i = 0; i < meshes.size(); i++)
            meshes[i].Draw(shader);
    }

private:
    // loads a model with supported ASSIMP extensions from file and stores the resulting meshes in the meshes vector.
    const aiScene* loadModel(const std::string& path)
    {
        std::cout << "inside loadmodel()" << std::endl;
        // read file via ASSIMP
        Assimp::Importer importer;
        const aiScene* scene = importer.ReadFile(path, aiProcess_Triangulate | aiProcess_FlipUVs | aiProcess_CalcTangentSpace);
        // check for errors
        std::cout << "Scene has lights : " << scene ->HasCameras();
        if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode) // if is Not Zero
        {
            std::cout << "ERROR::ASSIMP:: " << importer.GetErrorString() << std::endl;
            return scene;
        }
        // retrieve the directory path of the filepath
        directory = path.substr(0, path.find_last_of('\\'));
        format = path.substr(path.find_last_of('.'), path.length());
        // process ASSIMP's root node recursively
        processNode(scene->mRootNode, scene);
        return scene;
    }

    // processes a node in a recursive fashion. Processes each individual mesh located at the node and repeats this process on its children nodes (if any).
    void processNode(aiNode* node, const aiScene* scene)
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

    Mesh processMesh(aiMesh* mesh, const aiScene* scene)
    {
        std::cout << "(inside processMesh) " << std::endl;

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
        
        if (this->format == ".obj"|| this->format == ".gltf")
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
            textures.insert(heightMaps.end(), roughmetalMaps.begin(), roughmetalMaps.end());

        }
        //
        if (this->format == ".glb" )
        {
            
            // 1. diffuse maps
            std::vector<Texture> diffuseMaps = loadMaterialTexturesEmbedded(material,scene, aiTextureType_DIFFUSE, "texture_diffuse");
            textures.insert(textures.end(), diffuseMaps.begin(), diffuseMaps.end());
            // 2. specular maps
            std::vector<Texture> specularMaps = loadMaterialTexturesEmbedded(material,scene, aiTextureType_SPECULAR, "texture_specular");
            textures.insert(textures.end(), specularMaps.begin(), specularMaps.end());
            // 3. normal maps
            std::vector<Texture> normalMaps = loadMaterialTexturesEmbedded(material,scene , aiTextureType_NORMALS, "texture_normal");
            textures.insert(textures.end(), normalMaps.begin(), normalMaps.end());
            // 4. height maps
            std::vector<Texture> heightMaps = loadMaterialTexturesEmbedded(material,scene, aiTextureType_AMBIENT, "texture_height");
            textures.insert(textures.end(), heightMaps.begin(), heightMaps.end());
            // 5. roughness + metalness maps
            std::vector<Texture> roughmetalMaps = loadMaterialTexturesEmbedded(material,scene, aiTextureType_UNKNOWN, "texture_roughmetal");
            textures.insert(textures.end(), roughmetalMaps.begin(), roughmetalMaps.end());
            
          
            // return a mesh object created from the extracted mesh data
            
        }
        return Mesh(vertices, indices, textures);
    }

    unsigned int hi() {
        return 1;
    };

    // checks all material textures of a given type and loads the textures if they're not loaded yet.
    // the required info is returned as a Texture struct.
    std::vector<Texture> loadMaterialTextures(aiMaterial* mat,aiTextureType type, std::string typeName)
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


    std::vector<Texture> loadMaterialTexturesEmbedded(aiMaterial* mat,const aiScene *scene, aiTextureType type, std::string typeName)
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
                   
                    textured.id = TextureEmbedded(texture,typeName);
                    //textured.id = hi();
                    textured.type = typeName;
                    std::cout << std::endl << "Type : " << textured.type;
                    textured.path = "embedded";
                    textures.push_back(textured);
                    
                }
                else {
                    //regular file, check if it exists and read it
                    std::cout << "cant recognize embedded texture";
                }
               
                
        }
        
        for (int i = 0; i < textures.size(); i++)
        {
            std::cout << i << "'st Texture id , path , type = " ;
            std::cout << textures[i].id<< " ";
            std::cout << textures[i].type<< " ";
            std::cout << textures[i].path << std::endl;
        }
        
        
        return textures;
    }
};



class Scene
{
public:
    std::vector<std::string> propvec;
    std::vector<std::string> name;
    std::vector<glm::vec3> location;
    std::vector<glm::vec3> scale;
    std::vector<glm::vec4> rotation;
    std::vector<std::string> lightvec;
    std::vector<PointLight> lightList;
    unsigned int numLights;
    //std::vector<glm::vec4> rotation_axis;
    std::string scenePath;
    //Model* models;
    Scene(std::string scenePath)
    {
        scenePath = scenePath + "\\";
        this->scenePath = scenePath;
        std::string csvpath = scenePath + "scene_prop.csv";
        //std::cout << csvpath;
        std::ifstream inf{ csvpath };
        std::string prop;
        if (!inf)
        {
            // Print an error and exit
            std::cerr << "Uh oh, scene_prop.csv could not be opened for reading!" << std::endl;
            
        }
        while (inf)
        {
            std::getline(inf, prop);
        }
        //std::cout << prop;
        propvec = split(prop, ',');
        std::cout << "Read number of models as " << propvec.size() / 10;
        for (int i = 0; i < (propvec.size()-10); i = i + 11)
        {

            //std::cout << i << " ";
            name.push_back(propvec[i]);
            rotation.push_back(glm::vec4(std::stof(propvec[i + 1]), std::stof(propvec[i + 2]), -1*std::stof(propvec[i + 3]), std::stof(propvec[i + 4])));
            location.push_back(glm::vec3(std::stof(propvec[i + 5]), std::stof(propvec[i + 7]), -1 * std::stof(propvec[i + 6])));
            scale.push_back(glm::vec3(std::stof(propvec[i + 8]), std::stof(propvec[i + 9]), std::stof(propvec[i + 10])));

        }
        inf.close();
        std::string lightPath = scenePath + "scene_lights.csv";
        std::ifstream lightsFile{ lightPath };
        std::string light_string;
        if (!lightsFile)
        {
            std::cerr << "Uh oh, scene_lights.csv could not be opened for reading!" << std::endl;
        }
        while (lightsFile)
        {
            std::getline(lightsFile, light_string);
        }
        lightvec = split(light_string, ',');
        std::cout << "Read number of models as " << lightvec.size() / 9;
        for (int i = 0; i < (lightvec.size() - 9); i = i + 10)
        {
            PointLight temp{};
            temp.name = lightvec[i];
            temp.position = glm::vec3(std::stof(lightvec[i + 1]), std::stof(lightvec[i + 3]),-1* std::stof(lightvec[i + 2]));
            temp.color = glm::vec3(std::stof(lightvec[i + 4]), std::stof(lightvec[i + 5]), std::stof(lightvec[i + 6]));
            temp.power = std::stof(lightvec[i + 7]);
            temp.size = std::stof(lightvec[i + 8]);
            temp.use_shadows = std::stof(lightvec[i + 9]);
            lightList.push_back(temp);
        }
        numLights = lightList.size();
     
        /*
        for (int i = 0; i < rotation.size(); i++)
        {

            rotation_axis.push_back(rotate(rotation[i]));
        }
        */
        /*
        for (int i = 0; i < rotation.size(); i++)
        {
            for (int j = 0; j < 4; j++)
            {
                std::cout << rotation[i][j]<<" ";
            }
            std::cout << std::endl;
        }
        for (int i = 0; i < location.size(); i++)
        {
            for (int j = 0; j < 3; j++)
            {
                std::cout << location[i][j] << " ";
            }
            std::cout << std::endl;
        }
        for (int i = 0; i < scale.size(); i++)
        {
            for (int j = 0; j < 3; j++)
            {
                std::cout << scale[i][j] << " ";
            }
            std::cout << std::endl;
        }*/
        
        /*
        Model* models{ new Model[name.size()] };
        this->models = models;
        for (int i = 0; i < sizeof(models); i++)
        {
            //load model
            models[i].getpath(scenePath + name[i] + ".glb");
        }
        */
        // objetc transforms
    }
    

};
void drawScene(Scene scene, Shader ourShader,Model* models);
Model* loadScene(Scene scene);
//glm::vec4 rotate(glm::vec3 rotation);
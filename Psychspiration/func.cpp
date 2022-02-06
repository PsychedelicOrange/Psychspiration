#include<glad/glad.h>
#include<GLFW/glfw3.h>
#include<iostream>
#include <string>
#include<func.h>
#include <fstream>
#include <sstream>
#include <iostream>
#include <fstream>

void glfwwindowinnit()
{
	glfwInit();
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
}
int errorhandle(GLFWwindow* window)
{
	if (window == NULL)
	{
		std::cout << "Failed to create GLFW window" << std::endl;
		glfwTerminate();
		return -1;
	}
}

int gladinnit()
{
	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
	{
		std::cout << "Failed to initialize GLAD" << std::endl;
		return -1;
	}
	return 0;
}

// defines the static member variable
std::string Shader::shaderPath{ "lol" };

    Shader::Shader(std::string vertexPath , std::string fragmentPath , std::string geometryPath )
    {
        // 1. retrieve the vertex/fragment source code from filePath
        std::string vertexCode;
        std::string fragmentCode;
        std::string geometryCode;
        //const char* finalvertexPath = shaderPath.c_str() + vertexPath.c_str();
        //const char* finalfragmentPath = 
        //const char* finalgeometryPath;
        std::ifstream vShaderFile;
        std::ifstream fShaderFile;
        std::ifstream gShaderFile;
        // ensure ifstream objects can throw exceptions:
        vShaderFile.exceptions(std::ifstream::failbit | std::ifstream::badbit);
        fShaderFile.exceptions(std::ifstream::failbit | std::ifstream::badbit);
        gShaderFile.exceptions(std::ifstream::failbit | std::ifstream::badbit);
        try
        {
            // open files
            vShaderFile.open(Shader::shaderPath+vertexPath);
            fShaderFile.open(Shader::shaderPath +fragmentPath);
            std::stringstream vShaderStream, fShaderStream;
            // read file's buffer contents into streams
            vShaderStream << vShaderFile.rdbuf();
            fShaderStream << fShaderFile.rdbuf();
            // close file handlers
            vShaderFile.close();
            fShaderFile.close();
            // convert stream into string
            vertexCode = vShaderStream.str();
            fragmentCode = fShaderStream.str();
            // if geometry shader path is present, also load a geometry shader
            if (geometryPath!="0")
            {
                gShaderFile.open(Shader::shaderPath+geometryPath);
                std::stringstream gShaderStream;
                gShaderStream << gShaderFile.rdbuf();
                gShaderFile.close();
                geometryCode = gShaderStream.str();
            }
        }
        catch (std::ifstream::failure& e)
        {
            std::cout << "ERROR::SHADER::FILE_NOT_SUCCESFULLY_READ" << std::endl;
        }
        const char* vShaderCode = vertexCode.c_str();
        const char* fShaderCode = fragmentCode.c_str();
        // 2. compile shaders
        unsigned int vertex, fragment;
        // vertex shader
        vertex = glCreateShader(GL_VERTEX_SHADER);
        glShaderSource(vertex, 1, &vShaderCode, NULL);
        glCompileShader(vertex);
        checkCompileErrors(vertex, "VERTEX", Shader::shaderPath +vertexPath);
        
        // fragment Shader
        fragment = glCreateShader(GL_FRAGMENT_SHADER);
        glShaderSource(fragment, 1, &fShaderCode, NULL);
        glCompileShader(fragment);
        checkCompileErrors(fragment, "FRAGMENT", Shader::shaderPath +fragmentPath);
        // if geometry shader is given, compile geometry shader
        unsigned int geometry;
        if (geometryPath != "0")
        {
            const char* gShaderCode = geometryCode.c_str();
            geometry = glCreateShader(GL_GEOMETRY_SHADER);
            glShaderSource(geometry, 1, &gShaderCode, NULL);
            glCompileShader(geometry);
            checkCompileErrors(geometry, "GEOMETRY", Shader::shaderPath +geometryPath);
        }
        // shader Program
        ID = glCreateProgram();
        glAttachShader(ID, vertex);
        glAttachShader(ID, fragment);
        if (geometryPath!="0")
            glAttachShader(ID, geometry);
        glLinkProgram(ID);
        checkCompileErrors(ID, "PROGRAM","program");
        // delete the shaders as they're linked into our program now and no longer necessery
        glDeleteShader(vertex);
        glDeleteShader(fragment);
        if (geometryPath != "0")
            glDeleteShader(geometry);

    }
  
    void Shader::use() 
    { 
        glUseProgram(ID); 
    }
    // utility uniform functions
    // ------------------------------------------------------------------------
    void Shader::setBool(const std::string &name, bool value) const
    {         
        glUniform1i(glGetUniformLocation(ID, name.c_str()), (int)value); 
    }
    // ------------------------------------------------------------------------
    void Shader::setInt(const std::string &name, int value) const
    { 
        glUniform1i(glGetUniformLocation(ID, name.c_str()), value); 
    }
    // ------------------------------------------------------------------------
    void Shader::setFloat(const std::string &name, float value) const
    { 
        glUniform1f(glGetUniformLocation(ID, name.c_str()), value); 
    }
    void Shader::setMat4(const std::string& name,const glm::mat4 value) const
    {
        glUniformMatrix4fv(glGetUniformLocation(ID, name.c_str()),1,GL_FALSE, &value[0][0]);
    }
    void Shader::setVec3(const std::string& name, const glm::vec3& value) const
    {
        glUniform3fv(glGetUniformLocation(ID, name.c_str()), 1, &value[0]);
    }
    void Shader::setVec3(const std::string& name, float x, float y, float z) const
    {
        glUniform3f(glGetUniformLocation(ID, name.c_str()), x, y, z);
    }

    void Shader::delete_()
    {
        glDeleteProgram(ID);
    }

    unsigned int TextureFromFile(const char* path, const std::string& directory, bool gamma)
    {
        std::string filename = std::string(path);
        filename = directory + '\\' + filename;
        unsigned int textureID;
        glGenTextures(1, &textureID);
        //stbi_set_flip_vertically_on_load(true);
        int width, height, nrComponents;
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

            stbi_image_free(data);
        }
        else
        {
           // std::cout << "Texture failed to load at path: " << path << std::endl;
            stbi_image_free(data);
        }

        return textureID;
    }
    unsigned int TextureEmbedded(const aiTexture *texture,std::string typeName)
    {

        unsigned char* image_data = nullptr;
        unsigned char* read_image_data = nullptr;
        int  width,height,components_per_pixel;
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
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAX_ANISOTROPY, 4);
       
        stbi_image_free(image_data);
        return textureID;

    }
    std::vector<std::string> split(const std::string& s, char delim) {
        std::vector<std::string> result;
        std::stringstream ss(s);
        std::string item;

        while (getline(ss, item, delim)) {
            result.push_back(item);
        }

        return result;
    }
    void drawScene(Scene scene, Shader ourShader, Model* models)
    {
        std::vector<glm::mat4> model;
        for (int i = 0; i < scene.name.size(); i++)
        {
            model.push_back(glm::mat4(1.0f));
            //model[i] = glm::rotate(model[i], scene.rotation_axis[i].w, glm::vec3(scene.rotation_axis[i].x, scene.rotation_axis[i].y, scene.rotation_axis[i].z));
            //std::cout << scene.rotation_axis[i].w;
            float scale = 0.1;
            model[i] = glm::translate(model[i], scene.location[i]);
            model[i] = glm::rotate(model[i], scene.rotation[i].x, glm::vec3(scene.rotation[i].y, scene.rotation[i].w, scene.rotation[i].z));
            model[i] = glm::scale(model[i], scene.scale[i]);
            model[i] = glm::translate(model[i], glm::vec3(0, 0, 0));
            ourShader.use();
            ourShader.setMat4("model", model[i]);
            models[i].Draw(ourShader);
        }
    }
    Model* loadScene(Scene scene)
    {
        Model* models{ new Model[scene.name.size()] };
        for (int i = 0; i < sizeof(models); i++)
        {
            //load model
            models[i].getpath(scene.scenePath + scene.name[i] + ".glb");
        }
        return models;
    }

 
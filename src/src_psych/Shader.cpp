#include <Shader.h>
#include <FileIO.h>
#include <glad/glad.h>
#include <string>
#include <fstream>
#include <sstream>
#include <iostream>
Shader::Shader(std::string computePath)
{
    std::string computeCode;
    std::ifstream cShaderFile;
    // ensure ifstream objects can throw exceptions:
    cShaderFile.exceptions(std::ifstream::failbit | std::ifstream::badbit);
    try
    {
        // open files
        cShaderFile.open(pathResource + "/Shaders/"+computePath);

        std::stringstream cShaderStream;
        // read file's buffer contents into streams
        cShaderStream << cShaderFile.rdbuf();
        // close file handlers
        cShaderFile.close();
        // convert stream into string
        computeCode = cShaderStream.str();
    }
    catch (std::ifstream::failure& e)
    {
        std::cout << "ERROR::SHADER::FILE_NOT_SUCCESFULLY_READ: " << e.what() << std::endl;
    }
    const char* cShaderCode = computeCode.c_str();
    // 2. compile shaders
    unsigned int compute;
    // compute shader
    compute = glCreateShader(GL_COMPUTE_SHADER);
    glShaderSource(compute, 1, &cShaderCode, NULL);
    glCompileShader(compute);
    checkCompileErrors(compute, "COMPUTE", computePath);

    // shader Program
    ID = glCreateProgram();
    glAttachShader(ID, compute);
    glLinkProgram(ID);
    checkCompileErrors(ID, "PROGRAM",computePath);
    // delete the shaders as they're linked into our program now and no longer necessery
    glDeleteShader(compute);

};
Shader::Shader(std::string vertexPath, std::string fragmentPath, std::string geometryPath)
{
    // 1. retrieve the vertex/fragment source code from filePath
    std::string vertexCode;
    std::string fragmentCode;
    std::string geometryCode;

    std::ifstream vShaderFile;
    std::ifstream fShaderFile;
    std::ifstream gShaderFile;
    std::string relativeShaderPath = pathResource + "/Shaders/";
    // std::cout<< relativeShaderPath+vertexPath;
    // ensure ifstream objects can throw exceptions:
    vShaderFile.exceptions(std::ifstream::failbit | std::ifstream::badbit);
    fShaderFile.exceptions(std::ifstream::failbit | std::ifstream::badbit);
    gShaderFile.exceptions(std::ifstream::failbit | std::ifstream::badbit);
    try
    {
        // open files

        vShaderFile.open(relativeShaderPath + vertexPath);
        //std::cout << relativeShaderPath + vertexPath << std::endl;
        fShaderFile.open(relativeShaderPath + fragmentPath);
        std::stringstream vShaderStream, fShaderStream;
        // read file's buffer contents into streams
        vShaderStream << vShaderFile.rdbuf();
        fShaderStream << fShaderFile.rdbuf();
        // close file handlers
        vShaderFile.close();
        fShaderFile.close();
        // convert stream into string
        vertexCode = vShaderStream.str();
        //std::cout << vertexCode;
        fragmentCode = fShaderStream.str();
        // if geometry shader path is present, also load a geometry shader
        if (geometryPath != "0")
        {
            gShaderFile.open(relativeShaderPath + geometryPath);
            std::stringstream gShaderStream;
            gShaderStream << gShaderFile.rdbuf();
            gShaderFile.close();
            geometryCode = gShaderStream.str();
        }
        else
        {
            geometryCode = "0";
        }
    }
    catch (std::ifstream::failure& e)
    {
        std::cout << "ERROR::SHADER::FILE_NOT_SUCCESFULLY_READ::" << relativeShaderPath << std::endl;
    }
    
    build(vertexCode, fragmentCode, geometryCode);
    std::cout << fragmentPath << std::endl;
}
void Shader::buildPermutations(std::string vertexShader, std::string fragmentShader, std::string geometryShader)
{
    //
    ////settings
    //std::string doNormalsString = "const int doNormals = " + std::to_string(donormals) + "\n;";
    //std::string versionString = "#version 430 core\n";
    //vertexShader = doNormalsString + vertexShader;
    //vertexShader = versionString + vertexShader;
    //fragmentShader = versionString + fragmentShader;
    //build(vertexShader, fragmentShader, geometryShader);
}

void Shader::build(std::string vertexShader, std::string fragmentShader, std::string geometryShader)
{    
    const char* vShaderCode = vertexShader.c_str();
    const char* fShaderCode = fragmentShader.c_str();
    
    // 2. compile shaders
    unsigned int vertex, fragment;
    // vertex shader
    vertex = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertex, 1, &vShaderCode, NULL);
    glCompileShader(vertex);
    checkCompileErrors(vertex, "VERTEX",vertexShader);

    // fragment Shader
    fragment = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragment, 1, &fShaderCode, NULL);
    glCompileShader(fragment);
    checkCompileErrors(fragment, "FRAGMENT", fragmentShader);
    
    // if geometry shader is given, compile geometry shader
    unsigned int geometry;
    if (geometryShader != "0")
    {
        const char* gShaderCode = geometryShader.c_str();
        geometry = glCreateShader(GL_GEOMETRY_SHADER);
        glShaderSource(geometry, 1, &gShaderCode, NULL);
        glCompileShader(geometry);
        checkCompileErrors(geometry, "GEOMETRY",geometryShader);
    }
    // shader Program
    ID = glCreateProgram();
    glAttachShader(ID, vertex);
    glAttachShader(ID, fragment);
    if (geometryShaderCode != "0")
        glAttachShader(ID, geometry);
    glLinkProgram(ID);
    checkCompileErrors(ID, "PROGRAM", "program");
    // delete the shaders as they're linked into our program now and no longer necessery
    glDeleteShader(vertex);
    glDeleteShader(fragment);
    if (geometryShaderCode != "0")
        glDeleteShader(geometry);
    std::cout << std::flush;
}
void Shader::use()
{
    glUseProgram(ID);
}
// utility uniform functions
// ------------------------------------------------------------------------
void Shader::setBool(const std::string& name, bool value) const
{
    glUniform1i(glGetUniformLocation(ID, name.c_str()), (int)value);
}
// ------------------------------------------------------------------------
void Shader::setInt(const std::string& name, int value) const
{
    glUniform1i(glGetUniformLocation(ID, name.c_str()), value);
}
// ------------------------------------------------------------------------
void Shader::setFloat(const std::string& name, float value) const
{
    glUniform1f(glGetUniformLocation(ID, name.c_str()), value);
}
void Shader::setMat4(const std::string& name, const glm::mat4 value) const
{
    glUniformMatrix4fv(glGetUniformLocation(ID, name.c_str()), 1, GL_FALSE, &value[0][0]);
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
void Shader::checkCompileErrors(unsigned int shader, std::string type, std::string path)
{
    int success;
    char infoLog[1024];
    if (type != "PROGRAM")
    {
        glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
        if (!success)
        {
            glGetShaderInfoLog(shader, 1024, NULL, infoLog);
            std::cout << "ERROR IN SHADER" << path << "::SHADER_COMPILATION_ERROR of type : " << type << "\n" << infoLog << "\n-- -------------------------------------------------- - -- " << std::endl;
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
#pragma once
#include <glm/glm.hpp>
#include <string>
class Shader
{
public:
    unsigned int ID;
    std::string vertexShaderCode;
    std::string fragmentShaderCode;
    std::string geometryShaderCode;
    // constructor generates the shader on the fly
    // ------------------------------------------------------------------------
    Shader();
    void buildPermutations(std::string vertexShader, std::string fragmentShader, std::string geometryShader);
    Shader(std::string computePath);
    Shader(std::string vertexPath, std::string fragmentPath, std::string geometryPath);
    void build(std::string vertexShader, std::string fragmentShader, std::string geometryShader);
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
    void checkCompileErrors(unsigned int shader, std::string type, std::string path);
    
};
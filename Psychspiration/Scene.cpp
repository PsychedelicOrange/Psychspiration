#include<Scene.h>
#include<FileIO.h>
#include <glm/gtc/matrix_transform.hpp>
#include <fstream>
#include <sstream>
#include <iostream>
Scene::Scene(std::string sceneName)
{
    this->sceneName = sceneName;
    propvec = parse("resource\\" + sceneName + "\\scene_prop.csv");
    std::cout <<std::endl<< "Read number of models as " << propvec.size() / 10;
    for (int i = 0; i < (propvec.size() - 10); i = i + 11)
    {

        //std::cout << i << " ";
        name.push_back(propvec[i]);
        rotation.push_back(glm::vec4(std::stof(propvec[i + 1]), std::stof(propvec[i + 2]), -1 * std::stof(propvec[i + 3]), std::stof(propvec[i + 4])));
        location.push_back(glm::vec3(std::stof(propvec[i + 5]), std::stof(propvec[i + 7]), -1 * std::stof(propvec[i + 6])));
        scale.push_back(glm::vec3(std::stof(propvec[i + 8]), std::stof(propvec[i + 9]), std::stof(propvec[i + 10])));

    }
   
    lightvec = parse("resource\\" + sceneName+"\\scene_lights.csv");
    std::cout << std::endl << "Read number of lights as " << lightvec.size() / 9;
    for (int i = 0; i < (lightvec.size() - 9); i = i + 10)
    {
        PointLight temp{};
        temp.name = lightvec[i];
        temp.position = glm::vec3(std::stof(lightvec[i + 1]), std::stof(lightvec[i + 3]), -1 * std::stof(lightvec[i + 2]));
        temp.color = glm::vec3(std::stof(lightvec[i + 4]), std::stof(lightvec[i + 5]), std::stof(lightvec[i + 6]));
        temp.power = std::stof(lightvec[i + 7]);
        temp.size = std::stof(lightvec[i + 8]);
        temp.use_shadows = std::stof(lightvec[i + 9]);
        lightList.push_back(temp);
    }
    numLights = lightList.size();
}
void Scene::draw(Shader ourShader)
{
    std::vector<glm::mat4> model;
    for (int i = 0; i < name.size(); i++)
    {
        model.push_back(glm::mat4(1.0f));
        model[i] = glm::translate(model[i], location[i]);
        model[i] = glm::rotate(model[i], rotation[i].x, glm::vec3(rotation[i].y, rotation[i].w, rotation[i].z));
        model[i] = glm::scale(model[i], scale[i]);
        model[i] = glm::translate(model[i], glm::vec3(0, 0, 0));
        ourShader.use();
        ourShader.setMat4("model", model[i]);
        (*models[i]).Draw(ourShader);
    }
}
Model* Scene::load()
{
    //Model* models{ new Model[scene.name.size()] };
    for (int i = 0; i < name.size(); i++)
    {   
        models.push_back(new Model("resource\\" + sceneName + "\\" + name[i] + ".glb"));
    }
}

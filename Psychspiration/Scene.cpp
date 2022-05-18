#include <Scene.h>
#include <FileIO.h>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtx/string_cast.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <fstream>
#include <sstream>
#include <iostream>
#include <map>
#include <thread>
#include <json.hpp>
using Json = nlohmann::json;
glm::mat4 getmat4_json(Json temp)
{
    float temp_arr[16];
    for (int i = 0; i < 4; i++)
    {
        for (int j = 0; j < 4; j++)
        {
            temp_arr[4*i+j] = temp[i][j].get<float>();
        }
    }
    return glm::make_mat4(temp_arr);
}
glm::vec3 getvec3_json(Json temp)
{
    float temp_arr[3];
    for (int i = 0; i < 3; i++)
    {
        temp_arr[i] = temp[i].get<float>();
    }
    return glm::make_vec3(temp_arr);
}
glm::vec3 convvec3_blender(glm::vec3 temp)
{
    float tempo = -1 * temp.y;
    temp.y = temp.z;
    temp.z = tempo;
    return temp;
}
Scene::Scene(std::string sceneName, Physics* physics,EventHandler* eventHandler,ModelManager* modelManager)
{
    this->sceneName = sceneName;
    this->physics = physics;
    this->eventHandler = eventHandler;
    this->modelManager = modelManager;
    
     
    Json sceneData = Json::parse(getStringFromDisk("Resources\\Scenes\\" + sceneName + ".scene"));
    // yet to implemnt hull loading yet
    for(auto& object : sceneData["objects"])
    { 
            objects.push_back(new Object(object["name"].get<std::string>(),object["export_name"].get<std::string>(),modelManager,getmat4_json(object["transform"])));
    }

    for (auto& light : sceneData["lights"])
    {
        lightList.push_back( PointLight(light["name"].get<std::string>(),convvec3_blender(getvec3_json(light["location"])),
            light["power"].get<float>(),light["range"].get<float>(), getvec3_json(light["color"]),light["use_shadow"].get<bool>()));
    }
    numLights = lightList.size();
    std::cout << glm::to_string(lightList[0].position);
}

void Scene::getInstanceCount()
{
        std::string temp = objects[0]->path;
        //int indexCount = 0;
        int count = 1;
        for (int i = 0; i < objects.size(); i++)
        {
            if (temp == objects[i]->path)
            {
                count++;
            }
            else
            {
                temp == objects[i]->path;
                objects[i - 1]->model->instanceCount = count;
                objectInstances.push_back({ i - 1});
            }
        }
}
void Scene::makeHAB()
{
    glm::mat4** instancedTransforms = new glm::mat4 * [objectInstances.size()];
    for (int i = 0; i < objectInstances.size(); i++)
    {
        instancedTransforms[i] = (glm::mat4*)std::vector<glm::mat4>(transforms.begin(), transforms.begin() + objectInstances[i]).data();
    }
}
void Scene::loadObjects()
{
    for (int i = 0; i < objects.size(); i++)
    {
        objects[i]->load();
        //set dynamic of object when doing this outside of scene
        physics->setObject(objects[i]);

    }

}
void Scene::drawObjects(Shader ourShader)
{
    for (int i = 0; i < objects.size(); i++)
    {
        objects[i]->draw(ourShader);
    }
}
void Scene::drawObjectsInstanced(Shader ourShader)
{
    
}
void Scene::drawHulls(Shader ourShader)
{
    for (int i = 0; i < objects.size(); i++)
    {
        objects[i]->drawHulls(ourShader);
    }
}
void Scene::setPhysics()
{
    for (int i = 0; i < objects.size(); i++)
    {
        physics->setObject(objects[i]);
    }
}
void Scene::updatePhysics()
{
    for (int i = 0; i < objects.size(); i++)
    {
        physics->setTransforms(objects[i]);
    }
}

void Scene::printdetail()
{
    std::cout << std::endl << "Scene read: " << sceneName << "\n   Number of Object: " << objects.size();
    for (int i = 0; i < objects.size(); i++)
    {
        objects[i]->printobj();
    }

}


int Scene::find(std::string t)
{
    for (int i = 0; i < objects.size(); i++)
    {
        if (t == objects[i]->name)
            return i;
    }
    std::cout << "ERROR:: HULL COULD'NT FIND GRAPHIC MODEL";
}

void Scene::setScale(float scale)
{
    for (int i = 0; i < objects.size(); i++)
    {
        objects[i]->transform = glm::scale(objects[i]->transform, glm::vec3(scale));
    }
}
void Scene::setUpEvents(EventHandler* eventHandler)
{
    //eventHandler->registerCallback("SpawnObject", &Scene::loadObject,this);
}

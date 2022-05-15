#include <Scene.h>
#include <FileIO.h>
#include <glm/gtc/matrix_transform.hpp>
#include <fstream>
#include <sstream>
#include <iostream>
#include <thread>
Scene::Scene(std::string sceneName, Physics* physics,EventHandler* eventHandler)
{
    this->sceneName = sceneName;
    this->physics = physics;
    this->eventHandler = eventHandler;
    propvec = parse("Resources\\Scenes\\" + sceneName + ".csv");
    for (int i = 0; i < (propvec.size() - 17); i = i + 18)
    {
        std::cout << propvec[i]<<"\n";
    }

    for (int i = 0; i < (propvec.size() - 17); i = i + 18)
    {
        name.push_back(propvec[i]);
        model_paths.push_back(propvec[i + 1]);
        transforms.push_back(getmat4_csv(i+1));
    }
    
    lightvec = parse("Resources\\Scenes\\" + sceneName+"_lights.csv");
    std::cout << "Number of lights: " << lightvec.size() / 9<<std::endl;

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
    this->populateObjects();
}

void Scene::populateObjects()
{
    for (int i = 0; i < name.size(); i++)
    {
        if (name[i][0] != '_')
            objects.push_back(new Object(name[i], model_paths[i], transforms[i]));
    }
    populateHulls();
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

glm::mat4 Scene::getmat4_csv(int i){
    //glm::mat4* mat = new glm::mat4(0.0f);
    
    glm::mat4 mat =  glm::mat4(stof(propvec[i+1]), stof(propvec[i + 2]), stof(propvec[i + 3]), stof(propvec[i + 4]),
        stof(propvec[i + 5]),stof(propvec[i+6]), stof(propvec[i + 7]), stof(propvec[i + 8]),
        stof(propvec[i + 9]), stof(propvec[i + 10]), stof(propvec[i + 11]), stof(propvec[i + 12]),
        stof(propvec[i + 13]), stof(propvec[i + 14]), stof(propvec[i + 15]), stof(propvec[i + 16]));
    
    return mat;
}
void Scene::populateHulls()
{
    std::vector<std::string> temp;
    for (int i = 0; i < name.size(); i++)
    {
        if (name[i][0] == '_')
        {
            temp = split(name[i], '_');
            int k = find(temp[1]);
            objects[k]->dynamic = true;
            objects[k]->hulls.push_back(new hull(model_paths[i], transforms[i]));
        }
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

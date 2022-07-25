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
glm::vec4 getvec4_json(Json temp)
{
    float temp_arr[4];
    for (int i = 0; i < 4; i++)
    {
        temp_arr[i] = temp[i].get<float>();
    }
    return glm::make_vec4(temp_arr);
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
    parseScene(getStringFromDisk("Resources\\Scenes\\" + sceneName + ".scene"));
}
Scene::Scene(char* path, Physics* physics, EventHandler* eventHandler, ModelManager* modelManager)
{
    this->sceneName = sceneName;
    this->physics = physics;
    this->eventHandler = eventHandler;
    this->modelManager = modelManager;
    parseScene(getStringFromDisk_direct(path));
}
void Scene::parseScene(std::string data)
{
    Json sceneData = Json::parse(data);
    // yet to implement hull loading yet
    for (auto& object : sceneData["objects"])
    {
        Object* ob = new Object(object["name"].get<std::string>(), object["export_name"].get<std::string>(), modelManager, getmat4_json(object["transform"]));
        if (object["hulls"].empty())
            ob->dynamic = 0;
        else
        {
            ob->dynamic = 1;
        }
        ob->localScale = getvec3_json(object["scale"]);
        glm::mat4 trans(1.0f);
        //trans = glm::scale(trans, ob->localScale);
        trans = glm::rotate(trans, glm::radians(object["rotate"][0].get<float>()), glm::vec3(object["rotate"][0].get<float>(), object["rotate"][0].get<float>(), object["rotate"][0].get<float>()));
        trans = glm::translate(trans, convvec3_blender(getvec3_json(object["translate"])));

        //if(ob->dynamic)
        //    ob->transform = trans;
        
        for (auto& hull_ : object["hulls"])
            ob->hulls.push_back(new hull(hull_["export_name"].get<std::string>(), getmat4_json(hull_["transform"])));
        liveObjects.insert({ object["name"].get<std::string>(),ob });
        
        objects_.push_back(new Object(object["name"].get<std::string>(), object["export_name"].get<std::string>(), modelManager, getmat4_json(object["transform"])));
    }
    for (auto& light : sceneData["lights"])
    {
        lightList.push_back(PointLight(light["name"].get<std::string>(), convvec3_blender(getvec3_json(light["location"])),
            light["power"].get<float>(), light["range"].get<float>(), getvec3_json(light["color"]), light["use_shadow"].get<bool>()));
    }
    numLights = lightList.size();
    std::cout << glm::to_string(lightList[0].position);
}

void Scene::dontCull()
{
    visibleObjects.clear();
    for (auto obj : liveObjects)
        visibleObjects.push_back(obj.second);
}
void Scene::artificialCull()
{
    visibleObjects.clear();
    for (auto obj : liveObjects)
        if (obj.second->name[0]!='P')
            visibleObjects.push_back(obj.second);
}
void Scene::fillDrawList()
{
    visibleObjects.clear();
    for (auto obj : liveObjects)
        if (obj.second->aabb.isOnFrustum(obj.second->getTransform())) 
            visibleObjects.push_back(obj.second);
}
void Scene::setInstanceCount()
{
    for (auto uniqueModel : modelManager->Models)
    {
        uniqueModel.second->instanceCount = 0;
    }
    for (auto obj : visibleObjects)
    {
        obj->model->instanceCount++;
    }
}
void Scene::setInstanceOffsets()
{
    modelManager->Models.begin()->second->instanceOffset = 0;
    auto obj_ = modelManager->Models.begin(); obj_++;
    for (auto obj = obj_; obj != modelManager->Models.end(); ++obj)
    {
        obj->second->instanceOffset = std::prev(obj)->second->instanceOffset + std::prev(obj)->second->instanceCount;
    }
}
void Scene::fillInstanceBuffer()
{
    delete instancedTransforms;
    instancedTransforms = new glm::mat4[visibleObjects.size()];
    for (auto uniqueModel : modelManager->Models)
    {
        uniqueModel.second->instanceCurr = -1;
    }
    for (auto obj : visibleObjects)
    {
       instancedTransforms[obj->model->instanceOffset + ++(obj->model->instanceCurr)] = obj->transform;
    }
    //for (int i = 0; i < objects.size(); i++)
    //    std::cout << "\n " << glm::to_string(instancedTransforms[i]);
}
void Scene::loadObjects()
{
    for (auto obj : liveObjects)
    {
        obj.second->load();
        obj.second->loadHulls();
    }

    //set dynamic of object when doing this outside of scene
    //physics->setObject(objects[i]);
}
void Scene::drawObjects(Shader ourShader)
{
    for (auto obj : liveObjects)
    {
        obj.second->draw(ourShader);
    }
}
void Scene::drawObjectsInstanced(Shader ourShader)
{
    for (auto obj : modelManager->Models)
    {
        if (obj.second->instanceCount != 0)
        {
            ourShader.use();
            ourShader.setInt("instanceOffset", obj.second->instanceOffset);
            obj.second->DrawInstanced(ourShader);
        }
            
    }
   /* ourShader.use();
    for (auto uniqueModels : modelManager->Models)
    {
        ourShader.setInt("instanceOffset", uniqueModels.second->instanceOffset);
        uniqueModels.second->DrawInstanced(ourShader);
    }*/
}
void Scene::drawShadowObjectsInstanced(Shader ourShader)
{
    for (auto obj : modelManager->Models)
    {
        if (obj.second->instanceCount != 0)
        {
            ourShader.use();
            ourShader.setInt("instanceOffset", obj.second->instanceOffset);
            obj.second->DrawInstanced(ourShader);
        }

    }
    /*ourShader.use();
    for (auto uniqueModels : modelManager->Models)
    {
        ourShader.setInt("instanceOffset", uniqueModels.second->instanceOffset);
        uniqueModels.second->DrawInstanced(ourShader);
    }*/
}
void Scene::addObject(std::string objectName,std::string path)
{
    liveObjects.insert({ objectName ,new Object(objectName,path,modelManager) });
    liveObjects[objectName]->load();
}
void Scene::drawHulls(Shader ourShader)
{
    for (auto obj : liveObjects)
    {
        if(obj.second->dynamic)
            obj.second->drawHulls(ourShader);
    }
    /*
    for (int i = 0; i < objects.size(); i++)
    {
        objects[i]->drawHulls(ourShader);
    }
    */
}
void Scene::setPhysics()
{

    for (auto obj : liveObjects)
    {
        physics->setObject(obj.second);
    }
    /*for (int i = 0; i < objects.size(); i++)
    {
        physics->setObject(objects[i]);
    }*/
}
void Scene::updatePhysics()
{
    for (auto obj : liveObjects)
    {
        if(obj.second->dynamic)
            physics->setTransforms(obj.second);
    }/*
    for (int i = 0; i < objects.size(); i++)
    {
        physics->setTransforms(objects[i]);
    }*/
}

void Scene::printdetail()
{
    std::cout << std::endl << "Scene read: " << sceneName << "\n   Number of Object: " << liveObjects.size();
    for (auto obj : liveObjects)
    {
      (obj.second)->printobj();
    }
    /*for (int i = 0; i < objects.size(); i++)
    {
        objects[i]->printobj();
    }*/

}

//std::vector<Object*>::iterator Scene::find_it(std::string t)
//{
//    for (auto obj :     )
//    {
//        if (obj->name == t)
//            return obj;
//    }
//}
//int Scene::find(std::string t)
//{
//    for (int i = 0; i < objects.size(); i++)
//    {
//        if (t == objects[i]->name)
//            return i;
//    }
//    std::cout << "ERROR:: HULL COULD'NT FIND GRAPHIC MODEL";
//}
//
//void Scene::setScale(float scale)
//{
//    for (auto obj : liveObjects)
//    {
//        (obj.second)->transform = glm::scale((obj.second)->transform, glm::vec3(scale));;
//    }
//   /* for (int i = 0; i < objects.size(); i++)
//    {
//        objects[i]->transform = glm::scale(objects[i]->transform, glm::vec3(scale));
//    }*/
//}
void Scene::setUpEvents(EventHandler* eventHandler)
{
    //eventHandler->registerCallback("SpawnObject", &Scene::loadObject,this);
}
void Scene::removeObject(std::string objectName)
{
    liveObjects["helmet.002"]->model->instanceCount--;
    liveObjects.erase("helmet.002");
}
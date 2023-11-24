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
#include <nlohmann/json.hpp>
#include <set>
using Json = nlohmann::json;
bool compareDistance(glm::vec3 a, glm::vec3 b, glm::vec3 c);
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
Scene::Scene(std::string sceneName,EventHandler* eventHandler,ModelManager* modelManager)
{
    this->sceneName = sceneName;
    this->eventHandler = eventHandler;
    this->modelManager = modelManager;
    this->physics = new Physics();
    this->instancedTransforms = NULL;
    parseScene(getStringFromDisk("\\Scenes\\" + sceneName + ".scene"));
}
//Scene::Scene(char* path, Physics* physics, EventHandler* eventHandler, ModelManager* modelManager)
//{
//    this->sceneName = sceneName;
//    this->physics = physics;
//    this->eventHandler = eventHandler;
//    this->modelManager = modelManager;
//    parseScene(getStringFromDisk_direct(path));
//}
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
    scenelight = new SceneLight();
    updateLightBuffer();
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
void Scene::fillDrawList(glm::vec3 cameraLoc)
{
    visibleObjects.clear();
    transparentObjects.clear();
    auto cmp = [cameraLoc](Object* a, Object* b) { return glm::distance(a->aabb->center, cameraLoc) > glm::distance(b->aabb->center, cameraLoc); };
    std::set<Object*, decltype(cmp)>transparentObjects (cmp);

    for (auto obj : liveObjects)
    {
        //bool isInFrustum = obj.second->aabb->isOnFrustum(obj.second->getTransform());
        bool isInFrustum = true;
        bool isOpaque = !obj.second->model->hasTransparentMesh;
        if (isInFrustum && isOpaque)
        {
            visibleObjects.push_back(obj.second);
        }
        else if(isInFrustum && !isOpaque)
        {
            transparentObjects.insert(obj.second);
        }
    }
    for (auto obj : transparentObjects)
    {
        this->transparentObjects.push_back(obj);
    }
};
bool compareDistance(glm::vec3 a, glm::vec3 b,glm::vec3 c)
{
    glm::vec3 ac = a - c;
    glm::vec3 bc = b - c;
    return ((ac.x * ac.x )+ (ac.y * ac.y )+ (ac.z * ac.z )> (bc.x * bc.x )+ (bc.y * bc.y) + (bc.z * bc.z));
}
void Scene::fillDrawList()
{
    visibleObjects.clear();
    for (auto obj : liveObjects)
    {
        bool isInFrustum = obj.second->aabb->isOnFrustum(obj.second->getTransform());
        if (isInFrustum)
        {
            visibleObjects.push_back(obj.second);
        }
    }
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
    if(instancedTransforms != NULL)
        delete[] instancedTransforms;
    instancedTransforms = new glm::mat4[visibleObjects.size()];
    for (auto uniqueModel : modelManager->Models)
    {
        uniqueModel.second->instanceCurr = -1;
    }
    for (auto obj : visibleObjects)
    {
       instancedTransforms[obj->model->instanceOffset + ++(obj->model->instanceCurr)] = obj->transform;
    }
    //for (int i = 0; i < visibleObjects.size(); i++)
     //   std::cout << "\n " << glm::to_string(instancedTransforms[i]);
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
void Scene::drawTransparentObjects(Shader ourShader)
{
    for (auto obj : transparentObjects)
    {
        obj->draw(ourShader);
        //std::cout << obj->name;
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
    /*for (auto obj : visibleObjects)
    {
        ourShader.use();
        ourShader.setInt("instanceOffset",obj->model->instanceOffset);
        obj->model->DrawInstanced(ourShader);
    }*/
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
void Scene::drawAabb(Shader ourShader)
{
    for (auto obj : liveObjects)
    {
        obj.second->drawAabb(ourShader);
    }
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
void Scene::setUpEvents(EventHandler* eventHandler)
{
    //eventHandler->registerCallback("SpawnObject", &Scene::loadObject,this);
}
void Scene::removeObject(std::string objectName)
{
    liveObjects["helmet.002"]->model->instanceCount--;
    liveObjects.erase("helmet.002");
}
void Scene::updateLightBuffer()
{
    scenelight->updateBuffer(this->lightList);
}
Scene::~Scene()
{
    delete modelManager;
    delete instancedTransforms;
    delete physics;
}
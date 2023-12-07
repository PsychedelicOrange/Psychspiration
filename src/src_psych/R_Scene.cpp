#include <R_Scene.h>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtx/string_cast.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <json.hpp>
#include <set>
#include <RModelManager.h>
#include <RTextureManager.h>
using Json = nlohmann::json;
glm::mat4 getmat4_json(Json temp)
{
    float temp_arr[16];
    for (int i = 0; i < 4; i++)
    {
        for (int j = 0; j < 4; j++)
        {
            temp_arr[4 * i + j] = temp[i][j].get<float>();
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
RScene::RScene(std::string sceneName) {
    parseScene(getStringFromDisk("/Scenes/" + sceneName + ".scene"));
}
RObject* RScene::getObject(string name)
{
    for (auto object : Objects)
        if (object->name == name)
            return object;
    std::cout << "Object not found !";
    return nullptr;
}
void RScene::parseScene(std::string data)
{
    Json sceneData = Json::parse(data);
    // yet to implement hull loading yet
    for (auto& object : sceneData["objects"])
    {
        RObject* ob= new RObject();
        ob->name = object["name"].get<std::string>();
        ob->path = object["export_name"].get<std::string>();
        ob->transform = getmat4_json(object["transform"]);
        ob->localScale = getvec3_json(object["scale"]);
        ob->translate = convvec3_blender(getvec3_json(object["translate"]));
        auto rotate= convvec3_blender(getvec3_json(object["rotate"]));
        //ob->angle = rotate[0];
        ob->rotateAxis = rotate;//{ rotate[1],rotate[2],rotate[3] };
        ob->dynamic = false;
        try {
        ob->dynamic = object["dynamic"].get<bool>();
        }
        catch(...){
            std::cout << "Dynamic property not specified";
        }
        Objects.push_back(ob);
    }
    for (auto& light : sceneData["lights"])
    {
        RPointLight* p = new RPointLight();
        p->position = convvec3_blender(getvec3_json(light["location"]));
        p->color = getvec3_json(light["color"]);
        p->powerWatts = light["power"].get<float>();
        p->clipRadius = light["range"].get<float>();
        pLights.push_back(p);
    }
}
void RScene::LoadObjects()
{
    RModelManager* modelMan = RModelManager::getInstance();
    for(auto object : Objects){
        std::cout << "Now Loading : " << object->path << std::endl;
        object->model = modelMan->getModel(object->path);
        string baked_path = pathResource+"/Textures/"+object->path+"_baked.hdr"; 
        if(checkFileExists(baked_path)){
            std::cout << "Baked lightmaps found, now loading ..." << std::endl;
            RTextureManager* textureManager = RTextureManager::getInstance();
            object->lightmapID = (textureManager->getTexture("Textures/"+ object->path+"_baked.hdr"))->ID;
        }else{
            std::cout << "Baked lightmaps not found." << std::endl;
        }
    }
}
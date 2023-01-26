#pragma once
#include <EventHandler.h>
#include <ModelManager.h>
#include <Model.h>
#include <Object.h>
#include<Shader.h>
#include<glm/glm.hpp>
#include<vector>
#include<string>
#include <Physics.h>
#include <Light.h>
#include <unordered_map>
#include <SceneLight.h>
class Scene
{
//importer
public:
    //engine
    EventHandler* eventHandler;
    ModelManager* modelManager;
    glm::mat4* instancedTransforms; // huge ass buffer

    std::string sceneName; // name of folder containing scene
    std::vector<Object*> objects_; // vector of object pointers in the scene
    std::unordered_map<std::string, Object*> liveObjects; // live objects in vicinity of player
    std::list <Object*> visibleObjects; // objects visible to the player / in the camera frustum
    std::vector<Model*> uniqueModels; // vector of unique model
    std::vector<Object*> transparentObjects;
    // lights
    std::vector<std::string> lightvec;
    std::vector<PointLight> lightList; // vector of lights 
    unsigned int numLights;
    SceneLight* scenelight;
    // physics 
    Physics* physics;
    void setScale(float scale);
    Scene(std::string sceneName,EventHandler* eventHandler, ModelManager* modelManager);
    ~Scene();
    //void draw(Shader ourShader);
    void setUpEvents(EventHandler* eventHandler);
    void parseScene(std::string data);

    void dontCull();
    void fillDrawList();
    void artificialCull();

    void fillDrawList(glm::vec3 cameraLoc);

    void binaryInsert(Object* obj, glm::vec3 cameraLoc);

    void setInstanceCount();
    void setInstanceOffsets();
    void fillInstanceBuffer();

    void makeHAB();
    void loadObjects();
    void drawObjects(Shader ourShader);
    void drawTransparentObjects(Shader ourShader);
    void drawObjectsInstanced(Shader ourShader);
    void drawShadowObjectsInstanced(Shader ourShader);
    void addObject(std::string objectName, std::string path);
    void removeObject(std::string objectName);
    void populateHulls();
    int find(std::string t);
    std::vector<Object*>::iterator find_it(std::string t);
    void drawHulls(Shader ourShader);
    void drawAabb(Shader ourShader);
    void setPhysics();
    void updatePhysics();
    void printdetail();
    void updateLightBuffer();
    glm::mat4 getmat4_csv(int i);
};
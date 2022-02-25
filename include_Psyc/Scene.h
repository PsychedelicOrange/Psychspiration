#pragma once
#include <Model.h>
#include <Object.h>
#include<Shader.h>
#include<glm/glm.hpp>
#include<vector>
#include<string>
#include <Physics.h>
struct PointLight {
    std::string name;
    glm::vec3 position;
    glm::vec3 color;
    float power;
    float constant;
    float linear;
    float quadratic;
    float size;
    float use_shadows;
};
struct GPULight {
    glm::vec4 position;
    glm::vec4 color;
    unsigned int enabled;
    float intensity;
    float range;
    float padding;
};

class Scene
{
public:
    std::string sceneName; // name of folder containing scene
    std::vector<Object*> objects; // vector of object pointers assigned in scene constructor 
    //models 
    std::vector<Model*> models;
    //model transforms and info 
    std::vector<std::string> propvec;
    std::vector<std::string> name;
    std::vector<glm::vec3> location;
    std::vector<glm::vec3> scale;
    std::vector<glm::vec4> rotation;
    //lights
    std::vector<std::string> lightvec;
    std::vector<PointLight> lightList;
    unsigned int numLights;
    // physics 
    Physics physics;
    /*
    //collision shapes
    btAlignedObjectArray<btCollisionShape*> collisionShapes;
    ///collision configuration contains default setup for memory, collision setup. Advanced users can create their own configuration.
    btDefaultCollisionConfiguration* collisionConfiguration = new btDefaultCollisionConfiguration();

    ///use the default collision dispatcher. For parallel processing you can use a diffent dispatcher (see Extras/BulletMultiThreaded)
    btCollisionDispatcher* dispatcher = new btCollisionDispatcher(collisionConfiguration);

    ///btDbvtBroadphase is a good general purpose broadphase. You can also try out btAxis3Sweep.
    btBroadphaseInterface* overlappingPairCache = new btDbvtBroadphase();

    ///the default constraint solver. For parallel processing you can use a different solver (see Extras/BulletMultiThreaded)
    btSequentialImpulseConstraintSolver* solver = new btSequentialImpulseConstraintSolver;

    btDiscreteDynamicsWorld* dynamicsWorld = new btDiscreteDynamicsWorld(dispatcher, overlappingPairCache, solver, collisionConfiguration);
    */
    Scene(std::string sceneName);
    void draw(Shader ourShader);
    void drawobj(Shader ourShader);
    void cleanupModels();
    Model* loadModels();
    void loadPhysics();
    void updatePhysics();
};
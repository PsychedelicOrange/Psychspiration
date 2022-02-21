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
void Scene::setStaticRigidBody()
{
    btCollisionShape* groundShape = new btBoxShape(btVector3(btScalar(50.), btScalar(50.), btScalar(50.)));
    this->collisionShapes.push_back(groundShape);

    btTransform groundTransform;
    groundTransform.setIdentity();
    groundTransform.setOrigin(btVector3(0, -56, 0));
    btScalar mass(0.);
    bool isDynamic = mass != 0.f;
    btVector3 localInertia(0, 0, 0);
    if (isDynamic)
        groundShape->calculateLocalInertia(mass, localInertia);
    
    //motion state
    btDefaultMotionState* myMotionState = new btDefaultMotionState(groundTransform);
    btRigidBody::btRigidBodyConstructionInfo rbInfo(mass, myMotionState, groundShape, localInertia);
    btRigidBody* body = new btRigidBody(rbInfo);

    //add the body to the dynamics world
    this->dynamicsWorld->addRigidBody(body);
}
void Scene::setDynamicRigidBody()
{
    btCollisionShape* colShape = new btBoxShape(btVector3(btScalar(5.), btScalar(5.), btScalar(5.)));
    this->collisionShapes.push_back(colShape);
    btTransform startTransform;
    startTransform.setIdentity();
    btScalar mass(1.f);
    startTransform.setOrigin(btVector3(2, 10, 0));
    btVector3 localInertia(0, 0, 0);
    bool isDynamic = mass != 0.f;
    if (isDynamic)
        colShape->calculateLocalInertia(mass, localInertia);
    btDefaultMotionState* myMotionState = new btDefaultMotionState(startTransform);
    btRigidBody::btRigidBodyConstructionInfo rbInfo(mass, myMotionState, colShape, localInertia);
    btRigidBody* body = new btRigidBody(rbInfo);
    dynamicsWorld->addRigidBody(body);
}
void Scene::setGravity()
{
    dynamicsWorld->setGravity(btVector3(0, -10, 0));
}
void Scene::doSim()
{
    for (int i = 0; i < 150; i++)
    {
        for (int j = this->dynamicsWorld->getNumCollisionObjects() - 1; j >= 0; j--)
        {
            btCollisionObject* obj = dynamicsWorld->getCollisionObjectArray()[j];
            btRigidBody* body = btRigidBody::upcast(obj);
            btTransform trans;
            if (body && body->getMotionState())
            {
                body->getMotionState()->getWorldTransform(trans);
            }
            else
            {
                trans = obj->getWorldTransform();
            }
            trans.getOpenGLMatrix();
        }
    }
}
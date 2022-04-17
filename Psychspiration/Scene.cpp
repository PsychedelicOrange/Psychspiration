#include<Scene.h>
#include<FileIO.h>
#include <glm/gtc/matrix_transform.hpp>
#include <fstream>
#include <sstream>
#include <iostream>
#include <thread>
Scene::Scene(std::string sceneName)
{
    this->sceneName = sceneName;
    propvec = parse("resource\\" + sceneName + "\\scene_prop.csv");
    for (int i = 0; i < (propvec.size() - 16); i = i + 17)
    {
        std::cout << propvec[i]<<"\n";
    }

    for (int i = 0; i < (propvec.size() - 16); i = i + 17)
    {
        name.push_back(propvec[i]);
        transforms.push_back(getmat4_csv(i));
        //objects.push_back(new Object(propvec[i]));
        //objects.push_back(new Object(propvec[i],new Model("resource\\" + sceneName + "\\" + propvec[i] + ".glb"), getmat4_csv(i)));
    }
    
    lightvec = parse("resource\\" + sceneName+"\\scene_lights.csv");
    std::cout << "Number of lights: " << lightvec.size() / 9<<std::endl;
    /*for (int i = 0; i < (lightvec.size() - 9); i = i + 10)
    {
        std::cout << lightvec[i];
    }*/
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
/*
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
        models[i]->Draw(ourShader);
    }
}
*/
void Scene::drawobj(Shader ourShader)
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

void Scene::loadModels()
{
    //Model* models{ new Model[scene.name.size()] };   
    for (int i = 0; i < name.size(); i++)
    {   
        if (name[i][0] != '_')

            objects.push_back(new Object(name[i], new Model("resource\\" + sceneName + "\\" + name[i] + ".gltf"), transforms[i]));
        //models.push_back(new Model("resource\\" + sceneName + "\\" + name[i] + ".glb"));
    }
}
void Scene::loadObject()
{
    glm::mat4 helmetTrans{ 1.0f };
    Object* helmet = new Object((std::string)("table"), new Model("resource\\newDDSexporter\\node_damagedHelmet_-6514.gltf"),helmetTrans);
    objects.push_back(helmet);
}
void Scene::loadHulls()
{
    for (int i = 0; i < name.size(); i++)
    {
        if (name[i][0] == '_')
        {
            std::vector<std::string> temp = split(name[i], '_');
            int k = find(temp[1]);
            objects[k]->dynamic = true;
            objects[k]->hulls.push_back(hull(new Model("resource\\" + sceneName + "\\" + name[i] + ".gltf"),transforms[i]));
        }
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
void Scene::loadPhysics()
{
    loadHulls();
    for (int i = 0; i < objects.size(); i++)
    {
        if (!objects[i]->dynamic)
            physics.setStaticRigidBody(objects[i]);
        else
            physics.setDynamicRigidBody(objects[i]);
    }
}
void Scene::updatePhysics()
{
    for (int i = 0; i < objects.size(); i++)
    {
        physics.setTransforms(objects[i]);
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
int Scene::find(std::string t)
{
    for (int i = 0; i < objects.size(); i++)
    {
        if (t == objects[i]->name)
            return i;
    }
    std::cout << "ERROR:: HULL COULD'NT FIND GRAPHIC MODEL";
}
/*
 
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
        }
    }
}
*/
void Scene::setScale(float scale)
{
    for (int i = 0; i < objects.size(); i++)
    {
        objects[i]->transform = glm::scale(objects[i]->transform, glm::vec3(scale));
    }
}

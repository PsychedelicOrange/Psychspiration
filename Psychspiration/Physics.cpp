#include <Physics.h>
#include <Mesh.h> // for vertex struct 
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtx/string_cast.hpp>
#include <iostream>
inline btVector3 glm2bt(const glm::vec3& vec)
{
    return { vec.x, vec.y, vec.z };
}

inline glm::vec3 bt2glm(const btVector3& vec)
{
    return { vec.x(), vec.y(), vec.z() };
}

Physics::Physics() {

    collisionConfiguration = new btDefaultCollisionConfiguration();
    
    dispatcher = new btCollisionDispatcher(collisionConfiguration);
    
    overlappingPairCache = new btDbvtBroadphase();
    
    solver = new btSequentialImpulseConstraintSolver;
    
    dynamicsWorld= new btDiscreteDynamicsWorld(dispatcher, overlappingPairCache, solver, collisionConfiguration);
}
void Physics::setObject(Object* obj)
{
    if (!obj->dynamic)
        setStaticRigidBody(obj);
    else
    {
        obj->loadHulls();
        setDynamicRigidBody(obj);
    }
}

void Physics::setStaticRigidBody(Object* obj)
{
    btCompoundShape* compoundShape = new btCompoundShape();

    btTransform groundTransform;
    groundTransform.setIdentity();
    groundTransform.setFromOpenGLMatrix(&(obj->transform[0][0]));
    //= obj->model->compShape 
    Mesh* t_mesh;
    for (int i = 0; i < obj->model->meshes.size(); i++)
    {
        t_mesh = &obj->model->meshes[i];
        btCollisionShape* colShape = obj->model->meshes[i].colShape;

        auto mesh = new btTriangleIndexVertexArray((int)t_mesh->indices.size() / 3, (int*)&(t_mesh->indices[0]), 3 * sizeof(unsigned int),t_mesh->vertices.size(),(btScalar* )&(t_mesh->vertices[0]), sizeof(Vertex));
  
        colShape = new btBvhTriangleMeshShape(mesh, false);

        collisionShapes.push_back(colShape);

        compoundShape->addChildShape(groundTransform,colShape);
 
    }
    btScalar mass =  0 ;
    btVector3 localInertia(0, 0, 0);
    compoundShape->setLocalScaling(btVector3(obj->transform[0][0], obj->transform[1][1], obj->transform[2][2]));
    //motion state
    btDefaultMotionState* myMotionState = new btDefaultMotionState(groundTransform);
    btRigidBody::btRigidBodyConstructionInfo rbInfo(mass, myMotionState, compoundShape, localInertia);
    btRigidBody* body = new btRigidBody(rbInfo);

    //add the body to the dynamics world and to Model object
    dynamicsWorld->addRigidBody(body);
    obj->model->rigidBody = body;
}
void Physics::setDynamicRigidBody(Object* obj)
{
    //temp compound shape
    btCompoundShape* compoundShape = new btCompoundShape();
    std::cout << "\n For " << obj->name << ": opengl matrix : " << glm::to_string(obj->transform);
    btTransform startTransform;
    startTransform.setFromOpenGLMatrix(&(obj->transform[0][0]));
    btTransform* hullTransform;

    //make and add all collision shapes to compound shape
    btConvexHullShape* colShape;
    for (int i = 0; i < obj->hulls.size(); i++)
    {
        colShape = new btConvexHullShape((btScalar*)&(obj->hulls[i]->model->meshes[0].vertices[0]),obj->hulls[i]->model->meshes[0].vertices.size(), sizeof(Vertex));
        //colShape->setLocalScaling(btVector3(obj->hulls[i].transform[0][0], obj->hulls[i].transform[1][1], obj->hulls[i].transform[2][2]));
        collisionShapes.push_back(colShape);
        hullTransform = new btTransform();
        hullTransform->setFromOpenGLMatrix(&(obj->hulls[i]->transform[0][0]));
        compoundShape->addChildShape(*hullTransform, colShape);
        
    }
    //btScalar* mass = new btScalar[compoundShape->getNumChildShapes()]{ 1 };
    btScalar mass[100];
    for (int i = 0; i < 100; i++)
    {
        mass[i] = 1;
    }
    btTransform principal;
    btVector3 inertia;
    compoundShape->calculatePrincipalAxisTransform(mass, principal, inertia);
    //compoundShape->setLocalScaling(btVector3(obj->transform[0][0], obj->transform[1][1], obj->transform[2][2]));
    btCompoundShape* compound2 = new btCompoundShape();
    collisionShapes.push_back(compound2);
    
    // recompute the shift to make sure the compound shape is re-aligned
    for (int i = 0; i < compoundShape->getNumChildShapes(); i++)
        compound2->addChildShape(compoundShape->getChildTransform(i) * principal.inverse(),
            compoundShape->getChildShape(i));
    
    delete compoundShape;
    //compound2->setLocalScaling(btVector3(obj->transform[0][0], obj->transform[1][1], obj->transform[2][2]));

    btDefaultMotionState* myMotionState = new btDefaultMotionState(startTransform);
    btRigidBody::btRigidBodyConstructionInfo rbInfo(*mass, myMotionState, compound2, inertia);
    btRigidBody* body = new btRigidBody(rbInfo);

    dynamicsWorld->addRigidBody(body);
    obj->model->rigidBody = body;
}
void Physics::setGravity()
{
    dynamicsWorld->setGravity(btVector3(0, -10, 0));
}
void Physics::stepSim()
{
    dynamicsWorld->stepSimulation(1.f / 60.f, 10);
}

void Physics::setTransforms(Object* obj)
{
    btRigidBody* rbody = obj->model->rigidBody;
    btCollisionObject* pobj = rbody;
    btTransform trans;
    if (rbody && rbody->getMotionState())
    {
        rbody->getMotionState()->getWorldTransform(trans);
    }
    else
    {
        trans = pobj->getWorldTransform();
    }
    //std::cout<<obj->name<<" : \t"<< float(trans.getOrigin().getX())<<" , "<< float(trans.getOrigin().getY()) << " , " << float(trans.getOrigin().getZ())<<"\n";
    // Convert the btTransform into the GLM matrix using 'glm::value_ptr'
    trans.getOpenGLMatrix(&obj->transform[0][0]);
   
    //if (obj->dynamic)
    //{
    //    /*obj->transform[0][0] *= pobj->getCollisionShape()->getLocalScaling().x();
    //    obj->transform[0][0] *= pobj->getCollisionShape()->getLocalScaling().y();
    //    obj->transform[0][0] *= pobj->getCollisionShape()->getLocalScaling().z();*/
    //    //obj->transform = glm::scale(obj->transform,bt2glm(pobj->getCollisionShape()->getLocalScaling()));
    //}
    //if (!obj->debug)
    //{
    //    std::cout << "\n For " << obj->name << ": after setTransform : " << glm::to_string(obj->transform);
    //    obj->debug = true;
    //}
    
}

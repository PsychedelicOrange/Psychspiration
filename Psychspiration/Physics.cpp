#include <Physics.h>
#include <glm/gtc/type_ptr.hpp>
#include <iostream>
Physics::Physics() {

    collisionConfiguration = new btDefaultCollisionConfiguration();
    
    dispatcher = new btCollisionDispatcher(collisionConfiguration);
    
    overlappingPairCache = new btDbvtBroadphase();
    
    solver = new btSequentialImpulseConstraintSolver;
    
    dynamicsWorld= new btDiscreteDynamicsWorld(dispatcher, overlappingPairCache, solver, collisionConfiguration);
}


void Physics::setStaticRigidBody(Object* obj)
{
    btCompoundShape* compoundShape = new btCompoundShape();

    btTransform groundTransform;
    groundTransform.setIdentity();
   // groundTransform.setFromOpenGLMatrix(mat42bt(obj->transform));
    //= obj->model->compShape 
    btScalar halfX[3];// setting all meshes as static body
    for (int i = 0; i < obj->model->meshes.size(); i++)
    {
        btCollisionShape* colShape = obj->model->meshes[i].colShape;

        for (int j = 0; j < 3; j++)
        {
            //shape using AABB of meshes 
            halfX[j] = (obj->model->meshes[i].AABB[1][j] - obj->model->meshes[i].AABB[0][j]) / 2;
        }
        colShape = new btBoxShape(btVector3(halfX[0], halfX[1], halfX[2]));
        collisionShapes.push_back(colShape);
       

        compoundShape->addChildShape(groundTransform,colShape);
 
    }
    btScalar mass =  0 ;
    btTransform principal;
    btVector3 inertia;
    btVector3 localInertia(0, 0, 0);
    compoundShape->calculateLocalInertia(mass, localInertia);

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

    btTransform startTransform;
    startTransform.setIdentity();
    //startTransform.setFromOpenGLMatrix(mat42bt(obj->transform));
    btScalar halfX[3];

    //make and add all collision shapes to compound shape
    for (int i = 0; i < obj->model->meshes.size(); i++)
    {
        for (int j = 0; j < 3; j++)
        {
            //shape using AABB of meshes 
            halfX[j] = (obj->model->meshes[i].AABB[1][j] - obj->model->meshes[i].AABB[0][j]) / 2;
        }
        btCollisionShape* colShape = new btBoxShape(btVector3(halfX[0], halfX[1], halfX[2]));
        collisionShapes.push_back(colShape);

        compoundShape->addChildShape(startTransform, colShape);
        
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

    btCompoundShape* compound2 = new btCompoundShape();
    collisionShapes.push_back(compound2);

    // recompute the shift to make sure the compound shape is re-aligned
    for (int i = 0; i < compoundShape->getNumChildShapes(); i++)
        compound2->addChildShape(compoundShape->getChildTransform(i) * principal.inverse(),
            compoundShape->getChildShape(i));
    btVector3 localInertia(0, 0, 0);

    delete compoundShape;


    btDefaultMotionState* myMotionState = new btDefaultMotionState(startTransform);
    btRigidBody::btRigidBodyConstructionInfo rbInfo(*mass, myMotionState, compound2, localInertia);
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
    std::cout<<obj->name<<" : \t"<< float(trans.getOrigin().getX())<<" , "<< float(trans.getOrigin().getY()) << " , " << float(trans.getOrigin().getZ())<<"\n";
    // Convert the btTransform into the GLM matrix using 'glm::value_ptr'
   // trans.getOpenGLMatrix(glm::value_ptr(obj->transform));
}
btScalar* Physics::mat42bt(glm::mat4 mat)
{
    //  A pointer to a 16 element array (12 rotation(row major padded on the right by 1), and 3 translation 
    btScalar* x= new btScalar[16];
    for (size_t i = 0; i < 4; i++)
    {
        x[0 + 4 * i] = mat[i][0];
        x[1 + 4 * i] = mat[i][1];
        x[2 + 4 * i] = mat[i][2];
        x[3 + 4 * i] = mat[i][3];
    }
    return x;
}
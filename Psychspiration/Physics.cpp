#include <Physics.h>
Physics::Physics() {

    collisionConfiguration = new btDefaultCollisionConfiguration();
    
    dispatcher = new btCollisionDispatcher(collisionConfiguration);
    
    overlappingPairCache = new btDbvtBroadphase();
    
    solver = new btSequentialImpulseConstraintSolver;
    
    dynamicsWorld= new btDiscreteDynamicsWorld(dispatcher, overlappingPairCache, solver, collisionConfiguration);
}


void Physics::setStaticRigidBody(Object* obj)
{
    btScalar halfX[3];// setting all meshes as static body
    for (int i = 0; i < obj->model->meshes.size(); i++)
    {
        for (int j = 0; j < 3; j++)
        {
            //shape using AABB of meshes 
            halfX[j] = (obj->model->meshes[i].AABB[1][j] - obj->model->meshes[i].AABB[0][j]) / 2;
        }
        btCollisionShape* groundShape = new btBoxShape(btVector3(halfX[0], halfX[1], halfX[2]));

        collisionShapes.push_back(groundShape);
        
        btTransform groundTransform;
        groundTransform.setIdentity();
        
        groundTransform.setFromOpenGLMatrix(mat42bt(obj->transform));
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
        dynamicsWorld->addRigidBody(body);
    }
}
void Physics::setDynamicRigidBody(Object* obj)
{

    btScalar halfX[3];// setting all meshes as static body
    for (int i = 0; i < obj->model->meshes.size(); i++)
    {
        for (int j = 0; j < 3; j++)
        {
            //shape using AABB of meshes 
            halfX[j] = (obj->model->meshes[i].AABB[1][j] - obj->model->meshes[i].AABB[0][j]) / 2;
        }
        btCollisionShape* colShape = new btBoxShape(btVector3(halfX[0], halfX[1], halfX[2]));
        collisionShapes.push_back(colShape);
        btTransform startTransform;
        startTransform.setIdentity();
        btScalar mass(1.f);
        startTransform.setFromOpenGLMatrix(mat42bt(obj->transform));
        btVector3 localInertia(0, 0, 0);
        bool isDynamic = mass != 0.f;
        if (isDynamic)
            colShape->calculateLocalInertia(mass, localInertia);
        btDefaultMotionState* myMotionState = new btDefaultMotionState(startTransform);
        btRigidBody::btRigidBodyConstructionInfo rbInfo(mass, myMotionState, colShape, localInertia);
        btRigidBody* body = new btRigidBody(rbInfo);
        dynamicsWorld->addRigidBody(body);
    }
}
void Physics::setGravity()
{
    dynamicsWorld->setGravity(btVector3(0, -10, 0));
}
void Physics::doSim()
{
    for (int i = 0; i < 150; i++)
    {
        for (int j = dynamicsWorld->getNumCollisionObjects() - 1; j >= 0; j--)
        {
            btCollisionObject* pobj = dynamicsWorld->getCollisionObjectArray()[j];
            btRigidBody* body = btRigidBody::upcast(pobj);
            btTransform trans;
            if (body && body->getMotionState())
            {
                body->getMotionState()->getWorldTransform(trans);
            }
            else
            {
                trans = pobj->getWorldTransform();
            }
        }
    }
}
btScalar* mat42bt(glm::mat4 mat)
{
    //  A pointer to a 16 element array (12 rotation(row major padded on the right by 1), and 3 translation 
    btScalar* x;
    for (size_t i = 0; i < 4; i++)
    {
        x[0 + 4 * i] = mat[i][0];
        x[1 + 4 * i] = mat[i][1];
        x[2 + 4 * i] = mat[i][2];
        x[3 + 4 * i] = mat[i][3];
    }

}
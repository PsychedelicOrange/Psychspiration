#include <Physics.h>
#include <bullet/BulletCollision/CollisionDispatch/btGhostObject.h>
#include <RMesh.h> // for vertex struct 
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtx/string_cast.hpp>
#include <iostream>
#include <HullBuilder.h>
#include <VHACD.h>

inline btVector3 glm2bt(const glm::vec3& vec)
{
    return { vec.x, vec.y, vec.z };
}

inline glm::vec3 bt2glm(const btVector3& vec)
{
    return { vec.x(), vec.y(), vec.z() };
}

Physics::Physics()
{

    collisionConfiguration = new btDefaultCollisionConfiguration();
    
    dispatcher = new btCollisionDispatcher(collisionConfiguration);
    
    overlappingPairCache = new btDbvtBroadphase();
    
    solver = new btSequentialImpulseConstraintSolver;
    
    dynamicsWorld= new btDiscreteDynamicsWorld(dispatcher, overlappingPairCache, solver, collisionConfiguration);
    dynamicsWorld->getPairCache()->setInternalGhostPairCallback(new btGhostPairCallback());
    debugdrawphysics = new DebugDrawPhysics();
    dynamicsWorld->setDebugDrawer(debugdrawphysics);
    dynamicsWorld->getDebugDrawer()->setDebugMode(1);
    setGravity();
}
Physics::~Physics()
{
    //remove the rigidbodies from the dynamics world and delete them
    for (int i = dynamicsWorld->getNumCollisionObjects() - 1; i >= 0; i--)
    {
        btCollisionObject* obj = dynamicsWorld->getCollisionObjectArray()[i];
        btRigidBody* body = btRigidBody::upcast(obj);
        if (body && body->getMotionState())
        {
            delete body->getMotionState();
        }
        dynamicsWorld->removeCollisionObject(obj);
        delete obj;
    }
    //delete collision shapes
    for (int j = 0; j < collisionShapes.size(); j++)
    {
        btCollisionShape* shape = collisionShapes[j];
        collisionShapes[j] = 0;
        delete shape;
    }

    delete dynamicsWorld;
    delete solver;
    delete overlappingPairCache;
    delete dispatcher;
    delete collisionConfiguration;
}
void Physics::setObject(RObject* obj)
{
    if (!obj->dynamic)
        setStaticRigidBody(obj);
    else
    {
        //obj->loadHulls();
        setDynamicRigidBody(obj);
    }
}
void Physics::setObjects(vector<RObject*> Objects)
{
    for (auto obj : Objects)
    {
        setObject(obj);
    }
}

void Physics::setStaticRigidBody(RObject* obj)
{
    btCompoundShape* compoundShape = new btCompoundShape();

    btTransform groundTransform;
    groundTransform.setIdentity();
    groundTransform.setFromOpenGLMatrix(&(obj->transform[0][0]));
    //= obj->model->compShape 
    RMesh* t_mesh;
    auto scale = btVector3(obj->localScale.x, obj->localScale.y, obj->localScale.z);
    for (int i = 0; i < obj->model->meshes.size(); i++)
    {
        t_mesh = obj->model->meshes[i];
        btCollisionShape* colShape;

        auto mesh = new btTriangleIndexVertexArray((int)t_mesh->indices.size() / 3, (int*)&(t_mesh->indices[0]), 3 * sizeof(unsigned int),t_mesh->vertices.size(),(btScalar* )&(t_mesh->vertices[0]), sizeof(Vertex));
        //std::cout <<"Scale"<< obj->localScale.x << "," << obj->localScale.y << "," << obj->localScale.z;
        colShape = new btScaledBvhTriangleMeshShape(new btBvhTriangleMeshShape(mesh, false),scale);

        collisionShapes.push_back(colShape);
        btTransform hullTransform;
        glm::mat4 t(1.0);
        hullTransform.setFromOpenGLMatrix(&(t[0][0]));
        compoundShape->addChildShape(hullTransform,colShape);
 
    }
    btScalar mass =  0 ;
    btVector3 localInertia(0, 0, 0);
    //compoundShape->setLocalScaling(scale);
    //motion state
    btDefaultMotionState* myMotionState = new btDefaultMotionState(groundTransform);
    btRigidBody::btRigidBodyConstructionInfo rbInfo(mass, myMotionState, compoundShape, localInertia);
    btRigidBody* body = new btRigidBody(rbInfo);
    //add the body to the dynamics world and to Model object
    dynamicsWorld->addRigidBody(body);
    obj->rigidBody = body;
}
void Physics::setDynamicRigidBody(RObject* obj)
{
    //Logging logging;
    //temp compound shape
    btCompoundShape* compoundShape = new btCompoundShape();
    //std::cout << "\n For " << obj->name << ": opengl matrix : " << glm::to_string(obj->transform);
    btTransform startTransform;
    startTransform.setFromOpenGLMatrix(&(obj->transform[0][0]));
    btConvexHullShape* colShape;
    HullBuilder hullBuilder;
    auto scale = btVector3(obj->localScale.x,obj->localScale.y,obj->localScale.z);
    hullBuilder.builder(obj->path,obj->hulls);
    for (auto hull : obj->hulls)
    {
        colShape = new btConvexHullShape((btScalar*)&(hull->mesh->vertices[0]), hull->mesh->vertices.size(), sizeof(Vertex));
        colShape->setLocalScaling(scale);
        
        colShape->setSafeMargin(00.04);
        collisionShapes.push_back(colShape);

        glm::mat4 t = glm::mat4(1.0f);
        btTransform hullTransform;
        hullTransform.setFromOpenGLMatrix(&(t[0][0]));
        compoundShape->addChildShape(hullTransform, colShape);
    }

    int nchilds = compoundShape->getNumChildShapes();
    btScalar* mass = new btScalar[nchilds];
    btScalar total_mass = 0.175; //total_mass /= nchilds;
    for (int i = 0; i < nchilds; i++)
        mass[i] = total_mass;
    btTransform principal;
    btVector3 inertia;
    compoundShape->calculatePrincipalAxisTransform(mass, principal, inertia);
    //compoundShape->setLocalScaling(scale);
    compoundShape->setMargin(0.04);
    btCompoundShape* compound2 = new btCompoundShape();
    collisionShapes.push_back(compound2);
    // recompute the shift to make sure the compound shape is re-aligned
    for (int i = 0; i < compoundShape->getNumChildShapes(); i++)
        compound2->addChildShape(compoundShape->getChildTransform(i) * principal.inverse(),
            compoundShape->getChildShape(i));

    delete compoundShape;
    //compound2->setLocalScaling(scale);

    btDefaultMotionState* myMotionState = new btDefaultMotionState(startTransform);
    btRigidBody::btRigidBodyConstructionInfo rbInfo(*mass, myMotionState, compound2, inertia);
    btRigidBody* body = new btRigidBody(rbInfo);
    //body->setCollisionFlags(body->getCollisionFlags() | btCollisionObject::CF_KINEMATIC_OBJECT);
    //body->setActivationState(DISABLE_DEACTIVATION);
    dynamicsWorld->addRigidBody(body);
    obj->rigidBody = body;
}
void Physics::setGravity()
{
    dynamicsWorld->setGravity(btVector3(0, -10, 0));
}
void Physics::stepSim(float deltaTime)
{
    /*
        https://web.archive.org/web/20091118162634/http://bulletphysics.org/mediawiki-1.5.8/index.php/Stepping_The_World
    */
    dynamicsWorld->stepSimulation(deltaTime,4,(float)1/60);
}

void Physics::updateTransform(RObject* obj)
{
    btRigidBody* rbody = obj->rigidBody;
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

    glm::mat4 transform,scaled_transform;
    trans.getOpenGLMatrix(&transform[0][0]);
    scaled_transform = glm::scale(glm::mat4(1.0f), obj->localScale);
    obj->transform = transform * scaled_transform;
}
void Physics::updateTransforms(vector<RObject* >objects)
{
    for (auto obj : objects)
        updateTransform(obj);
}

void Physics::drawDebug(mat4 view,mat4 projection)
{
    dynamicsWorld->debugDrawWorld();
    debugdrawphysics->setMVP(view, projection);
    debugdrawphysics->flushLines();
}

#include <Physics.h>
#define ENABLE_VHACD_IMPLEMENTATION 1
#include <VHACD.h>
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

Physics::Physics() 
{

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
        //obj->loadHulls();
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
        btCollisionShape* colShape;

        auto mesh = new btTriangleIndexVertexArray((int)t_mesh->indices.size() / 3, (int*)&(t_mesh->indices[0]), 3 * sizeof(unsigned int),t_mesh->vertices.size(),(btScalar* )&(t_mesh->vertices[0]), sizeof(Vertex));
        std::cout <<"Scale"<< obj->transform[0][0] << "," << obj->transform[1][1] << "," << obj->transform[2][2];
        auto scale = btVector3(obj->transform[0][0], obj->transform[1][1], obj->transform[2][2]);
        //colShape = new btScaledBvhTriangleMeshShape(new btBvhTriangleMeshShape(mesh, false),scale);
        colShape = new btBvhTriangleMeshShape(mesh, false);
        obj->model->meshes[i].colShape = colShape ;
        
        collisionShapes.push_back(colShape);

        compoundShape->addChildShape(groundTransform,colShape);
 
    }
    btScalar mass =  0 ;
    btVector3 localInertia(0, 0, 0);
    //compoundShape->setLocalScaling(btVector3(obj->transform[0][0], obj->transform[1][1], obj->transform[2][2]));
    //motion state
    btDefaultMotionState* myMotionState = new btDefaultMotionState(groundTransform);
    btRigidBody::btRigidBodyConstructionInfo rbInfo(mass, myMotionState, compoundShape, localInertia);
    btRigidBody* body = new btRigidBody(rbInfo);
    //add the body to the dynamics world and to Model object
    dynamicsWorld->addRigidBody(body);
    obj->rigidBody = body;
}
void Physics::setDynamicRigidBody(Object* obj)
{
    //temp compound shape
    btCompoundShape* compoundShape = new btCompoundShape();
    //std::cout << "\n For " << obj->name << ": opengl matrix : " << glm::to_string(obj->transform);
    btTransform startTransform;
    startTransform.setFromOpenGLMatrix(&(obj->transform[0][0]));
    btTransform* hullTransform;

    //make and add all collision shapes to compound shape
    btConvexHullShape* colShape;
    
    for (int i = 0; i < obj->hulls.size(); i++)
    {

        colShape = new btConvexHullShape((btScalar*)&(obj->hulls[i]->model->meshes[0].vertices[0]),obj->hulls[i]->model->meshes[0].vertices.size(), sizeof(Vertex));
        colShape->setSafeMargin(00.04);
        collisionShapes.push_back(colShape);
        hullTransform = new btTransform();
        glm::mat4 t = glm::mat4(1.0f);
        hullTransform->setFromOpenGLMatrix(&(t[0][0]));
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
    compoundShape->setLocalScaling(btVector3(obj->localScale.x, obj->localScale.y, obj->localScale.z));
    compoundShape->setMargin(0.04);
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
    obj->rigidBody = body;
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
//VHACD::IVHACD* iface = VHACD::CreateVHACD_ASYNC();
 //VHACD::IVHACD::Parameters params;
 //for (auto mesh : obj->model->meshes)
 //{
 //    /*for (int i = 0; i < mesh.vertices.size() ; i++)
 //    {
 //        std::cout << "\n " << mesh.vertices_flat[i]<<"," <<mesh.vertices_flat[i+1]<<","<<mesh.vertices_flat[i+2];
 //    }
 //    for (int i = 0; i < mesh.indices.size(); i++)
 //    {
 //        std::cout << "\n " << mesh.indices_flat[i] << "," << mesh.indices_flat[i + 1] << "," << mesh.indices_flat[i + 2];
 //    }*/
 //    std::cout << "iface->Compute(mesh.vertices_flat," << mesh.vertices.size() << ", mesh.indices_flat," << mesh.indices.size() << ", params); ";
 //    iface->Compute(mesh.vertices_flat,mesh.vertices.size(), mesh.indices_flat, mesh.indices.size()/3, params);
 //    while (!iface->IsReady())
 //    {
 //        std::this_thread::sleep_for(std::chrono::nanoseconds(10000)); // s
 //    }
 //    if (!iface->GetNConvexHulls())
 //        std::cout << "failed";
 //    else
 //    {
 //        std::cout << obj->name << "'s mesh computed !\n";
 //        for (int i = 0; i < iface->GetNConvexHulls(); i++)
 //        {
 //            VHACD::IVHACD::ConvexHull hull;
 //            iface->GetConvexHull(i,hull);
 //            colShape = new btConvexHullShape((btScalar*)hull.m_points, hull.m_nPoints);
 //            collisionShapes.push_back(colShape);
 //            hullTransform = new btTransform();
 //            //hullTransform->setFromOpenGLMatrix(&(obj->hulls[i]->transform[0][0]));
 //            compoundShape->addChildShape(*hullTransform, colShape);

 //        }
 //    }
 //}
 //
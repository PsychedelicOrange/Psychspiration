#pragma once
#include <RObject.h>
#include <bullet/btBulletDynamicsCommon.h>
#include <DebugDrawPhysics.h>
class Physics {
	public:
	//collision shapes
	btAlignedObjectArray<btCollisionShape*> collisionShapes;

	///collision configuration contains default setup for memory, collision setup. Advanced users can create their own configuration.
	btDefaultCollisionConfiguration* collisionConfiguration;

	///use the default collision dispatcher. For parallel processing you can use a diffent dispatcher (see Extras/BulletMultiThreaded)
	btCollisionDispatcher* dispatcher;

	///btDbvtBroadphase is a good general purpose broadphase. You can also try out btAxis3Sweep.
	btBroadphaseInterface* overlappingPairCache;

	///the default constraint solver. For parallel processing you can use a different solver (see Extras/BulletMultiThreaded)
	btSequentialImpulseConstraintSolver* solver;

	btDiscreteDynamicsWorld* dynamicsWorld;

	//debug draw
	DebugDrawPhysics* debugdrawphysics;

	//constraint with all axis locked 
	Physics();
	~Physics();
	void cleanupPhysics();
	void cleanupModels();
	void setObject(RObject* obj);
	void setObjects(vector<RObject*> obj);
	void setDynamicRigidBody(RObject* obj);
	void setStaticRigidBody(RObject* obj);
	void setGravity();
	void assignBB();
	void setTransform(RObject* obj);
	void setTransforms(vector < RObject*> objects);
	void drawDebug(mat4 view, mat4 projection);
	void stepSim();
	
};







#pragma once
#include <Object.h>
#include <bullet/btBulletDynamicsCommon.h>
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

	Physics();
	void cleanupPhysics();
	void cleanupModels();
	void setDynamicRigidBody(Object* obj);
	void setStaticRigidBody(Object* obj);
	void setGravity();
	void assignBB();
	void doSim();
	btScalar* mat42bt(glm::mat4 mat);
};







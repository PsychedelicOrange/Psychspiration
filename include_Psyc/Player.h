#pragma once
#include <iostream>
#include <Object.h>
#include <Physics.h>
#include <BulletDynamics/Character/btKinematicCharacterController.h>
#include <BulletCollision/CollisionDispatch/btGhostObject.h>
#include <Camera.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <functional>

class Player
{
public:
	Object* obj; //models and physics
	Physics* physics;
	btBoxShape* playerShape;
	btPairCachingGhostObject* ghostObject;
	btKinematicCharacterController* controller;
	Camera* camera;

	EventHandler* eventHandler; // controls
	Player(Object* obj,Camera* camera,EventHandler* eventHandler,Physics* physics)
	{
		this->camera = camera;
		this->obj = obj;
		this->physics = physics;
		this->eventHandler = eventHandler;
		playerShape = new btBoxShape(btVector3(obj->aabb.extents.x, obj->aabb.extents.y, obj->aabb.extents.z));
		ghostObject = new btPairCachingGhostObject();
		btTransform t; t.setFromOpenGLMatrix(&obj->transform[0][0]);
		ghostObject->setWorldTransform(t);
		physics->dynamicsWorld->getPairCache()->setInternalGhostPairCallback(new btGhostPairCallback());
		ghostObject->setCollisionShape(playerShape);
		ghostObject->setCollisionFlags(btCollisionObject::CF_CHARACTER_OBJECT);
		controller = new btKinematicCharacterController(ghostObject, playerShape, 0.5);
		physics->dynamicsWorld->addCollisionObject(ghostObject, btBroadphaseProxy::CharacterFilter, btBroadphaseProxy::StaticFilter | btBroadphaseProxy::DefaultFilter);
		physics->dynamicsWorld->addAction(controller);
	}
	void updateTrans()
	{
		controller->getGhostObject()->getWorldTransform().getOpenGLMatrix(&obj->transform[0][0]);
	}
	void ProcessKeyboard(GLFWwindow* window)
	{
		btVector3 t; t.setValue(camera->Front.x, camera->Front.y, camera->Front.z); t *= 0.1;
		controller->setVelocityForTimeInterval(t,eventHandler->deltaTime);

		//glm::vec3 translate{0.f}; // do the inheritance wala thing we dont want GLFWwindow here we want it in eventHandler	
		//float velocity = 1.0f * eventHandler->deltaTime;
		//if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
		//	translate.x +=  velocity;
		//if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
		//	translate.x -= velocity;
		//if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
		//	translate.y -=  velocity;
		//if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
		//	translate.y += velocity;
		//obj->transform = glm::translate(obj->getTransform(), translate);
	}
	void setUpEvents()
	{
		/*eventHandler->registerCallback("Player_Move_Forward", [=]() {
			std::cout << "\n\nplayer moved\n\n";
			
			obj->transform = glm::translate(obj->getTransform(), glm::vec3(1, 0, 0));
			});*/
	}
	
};

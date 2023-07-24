#pragma once
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <Globals.h>
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <R_Scene.h>
#include <SettingsManager.h>
#include <RInput.h>
#include <RWindow.h>
#include <RModelManager.h>
#include <Renderer.h>
#include <ConsoleDebug.h>
#include <Physics.h>
#include <RawInput.h>
#include <string>
int main()
{

	//engine innit
	ConsoleDebug console;
	Settings settings = SettingsManager::getSettings();
	EventHandler eventHandler;
	Input input(settings);
	
	Window Windows(&input);
	//RawInput rawInput(&Windows,settings.ControlToGLFWKeys);


	RModelManager* modelMan = RModelManager::getInstance();
	//load models
	RScene scene("testt");
	scene.LoadObjects();

	for (auto objects : scene.Objects)
	{
		console.log(objects->model);
	}
	//load lights
	RPointLight light;
	
	//load camera
	RCameraObject* camera = new RCameraObject(new RCamera(),input,glm::vec3(10,10,10), glm::vec3(0, 1, 0), glm::vec3(-10, -10, -10));
	
	//load physics
	Physics physics;
	physics.setObjects(scene.Objects);

	// and lights camera action !
	//console.log()
	Renderer renderer;
	renderer.insertObjects(scene.Objects);
	
	renderer.pLightsLive.insert(renderer.pLightsLive.end(), scene.pLights.begin(), scene.pLights.end());
	renderer.pLightsLive.push_back(&light);
	renderer.camerasLive.push_back(camera);
	//renderer.staticObjects.push_back(ball);

	renderer.innit();

	//rawInput.camera = renderer.camerasLive[0];

	//controls for physics
	bool* play = new bool;
	*play = false;
	input.registerKeyCallback("PLAY_KEY",3 ,[=]() {*play = true;});

	while (!glfwWindowShouldClose(Windows.window))
	{
		input.updateDeltaTimePoll();
		if (*play)
		{
			for (auto obj : renderer.dynamicObjects)
			{
				auto body = obj->rigidBody;
				body->activate(true);
				body->applyCentralForce({ 0,0,-100 });
			}
			*play = false;
		}
		physics.stepSim();
		physics.updateTransforms(renderer.dynamicObjects);
		
		
		glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		renderer.drawStatic();
		renderer.draw(renderer.dynamicObjects);

		//physics.drawDebug(camera->getView(), camera->projection);
		
		//rawInput.MoveCameraRawKeys();
		glfwSwapBuffers(Windows.window);
		glfwPollEvents();
	}
	// Save settings to json before exit
	SettingsManager::dumpJson(settings);
}

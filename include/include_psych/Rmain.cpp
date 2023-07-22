#pragma once
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <Globals.h>
#include <glad/glad.h>
#include <R_Scene.h>
#include <RInput.h>
#include <RWindow.h>
#include <RModelManager.h>
#include <Renderer.h>
#include <ConsoleDebug.h>
#include <SettingsManager.h>
#include <Physics.h>
#include <RawInput.h>
#include <string>
int main()
{

	//engine innit
	ConsoleDebug console;
	Settings settings = SettingsManager::getSettings();
	Input input(settings);
	
	Window Windows(settings,&input);
	RawInput rawInput(&Windows);


	RModelManager* modelMan = RModelManager::getInstance();
	//load models
	RScene scene("testt");
	scene.LoadObjects();

	//RObject* ball = new RObject();
	//ball->dynamic = 1;
	//ball->name = "spider";
	//ball->path = "Spider_Eyes_Spider";
	//ball->model = modelMan->getModel("Spider_Eyes_Spider");
	//ball->localScale = glm::vec3(0.1, 0.1, 0.1);
	//ball->transform = glm::mat4(1.0f);
	//ball->transform = glm::scale(ball->transform, glm::vec3(0.1, 0.1, 0.1));
	//ball->transform = glm::translate(ball->transform, glm::vec3(0,10, 0));
	for (auto objects : scene.Objects)
	{
		console.log(objects->model);
	}
	/*console.log(ball->model);*/
	//load lights
	RPointLight light;

	//load camera
	RCameraObject* camera = new RCameraObject(new RCamera(),glm::vec3(10,10,10), glm::vec3(0, 1, 0), glm::vec3(-10, -10, -10));

	//load physics
	Physics physics;
	physics.setObjects(scene.Objects);
	//physics.setObject(ball);
	// and lights camera action !
	//console.log()
	Renderer renderer;
	renderer.insertObjects(scene.Objects);
	
	renderer.pLightsLive.insert(renderer.pLightsLive.end(), scene.pLights.begin(), scene.pLights.end());
	renderer.pLightsLive.push_back(&light);
	renderer.camerasLive.push_back(camera);
	//renderer.staticObjects.push_back(ball);

	renderer.innit();

	rawInput.camera = renderer.camerasLive[0];

	//controls 
	bool* play = new bool;
	*play = false;
	input.eventHandler.registerCallback("PLAY_KEY", [=]() {*play = true;});

	while (!glfwWindowShouldClose(Windows.window))
	{
		if (*play)
		{
			physics.stepSim();
		}
		physics.setTransforms(renderer.dynamicObjects);
		rawInput.updateDeltaTime();
		
		glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		renderer.drawStatic();
		renderer.draw(renderer.dynamicObjects);

		//physics.drawDebug(camera->getView(), camera->projection);
		
		rawInput.MoveCameraRawKeys();
		glfwSwapBuffers(Windows.window);
		glfwPollEvents();
	}
	// Save settings to json before exit
	SettingsManager::dumpJson(settings);
}

#pragma once
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <gui.h>
#include <SettingsManager.h>
#include <State.h>
#include <RInput.h>
#include <RWindow.h>
#include <RModelManager.h>
#include <ConsoleDebug.h>
#include <Physics.h>
#include <RawInput.h>
#include <string>
#include <Player.h>
#include <RCameraObject.h>
#include <Renderer.h>
#include <R_Scene.h>
int main()
{
	//engine innit
	ConsoleDebug console;
	SettingsManager setMan;
	Settings settings = setMan.getSettings();
	Input input(settings);
	
	Window window(&input);
	Gui gui(window);
	Renderer renderer(input);
	//RawInput rawInput(&window,settings.ControlToGLFWKeys);
	RModelManager* modelMan = RModelManager::getInstance();
	//load models
	RScene scene("level0");
	scene.LoadObjects();
	//load lights
	RPointLight light;
	
	//load camera
	RCameraObject* camera = new RCameraObject(new RCamera(),input,glm::vec3(10,10,10), glm::vec3(0, 1, 0), glm::vec3(-10, -10, -10));

	//load physics
	Physics physics;
	physics.setObjects(scene.Objects);

	for (auto const objects : scene.Objects)
	{
		console.log(objects);
	}
	// player !!
	
	Player player(scene.getObject("player"),input,camera);
	
	// and lights camera action !
	//console.log()
	renderer.insertObjects(scene.Objects);
	
	renderer.pLightsLive.insert(renderer.pLightsLive.end(), scene.pLights.begin(), scene.pLights.end());
	renderer.pLightsLive.push_back(&light);
	renderer.camerasLive.push_back(camera);
	//renderer.staticObjects.push_back(ball);
	renderer.innit();

	//rawInput.camera = renderer.camerasLive[0];

	//general state
	State state(input);
	//state.play = true;
	player.body->activate(true);
	//player.body->applyTorqueImpulse(btVector3(0,5, 0));
	player.body->applyCentralImpulse(btVector3(0, 0, -10));
	
	while (!glfwWindowShouldClose(window.window))
	{
		input.updatedeltaTime();
		if (state.play)
		{
			physics.stepSim(input.deltaTime);
			physics.updateTransforms(renderer.dynamicObjects);
			input.tick();
		}
		if (state.show_menu)
		{
			window.disableInfiniteMouse();
		}
		else
			window.enableInfiniteMouse();

		
		gui.startFrame();
		gui.doOverlay();
		//gui.plotA(glm::degrees(player.angle_of_attack), 360);
		//gui.addDebugVariable(player.angle_of_attack,"Angle of Attack");
		gui.doDebugVariables();
		glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		//renderer.drawStatic();
		renderer.draw(renderer.dynamicObjects);
		renderer.draw(renderer.staticObjects);
		//glClear(GL_COLOR_BUFFER_BIT);
		gui.render();
		//physics.drawDebug(camera->getView(), camera->projection);
		
		//rawInput.MoveCameraRawKeys();
		glfwSwapBuffers(window.window);
		glfwPollEvents();
	}
	// Save settings to json before exit
	setMan.dumpJson(settings);
}

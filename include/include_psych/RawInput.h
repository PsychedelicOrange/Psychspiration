#pragma once
#include <RWindow.h>
#include <RCameraObject.h>
//#include <Controls.h>
class RawInput {
public:
    RCameraObject* camera;
    Window* window;
    unordered_map<string, int> controlToGLFWkeys;
    
    float lastX;
    float lastY;
    bool firstMouse = true;
	float deltaTime = 0.0f;
	float lastFrame = 0.0f;

    RawInput(Window* window, unordered_map<string, int>& controlToGLFWkeys);
    void updateCamera(GLFWwindow* w, double xpos, double ypos);
    void MoveCameraRawKeys();
    void updateDeltaTime();
    

};
#pragma once
#include <GLFW/glfw3.h>
#include <RCameraObject.h>
#include <RInput.h>
//#include <Controls.h>
class RawInput {
public:
    RCameraObject* camera;
    Window* window;
    unordered_map<string, int> controlToGLFWkeys;
    RawInput(Window* window,unordered_map<string,int>& controlToGLFWkeys): window(window),controlToGLFWkeys(controlToGLFWkeys) {
        lastX = window->width / 2.0f;
        lastY = window->height / 2.0f;
        static_cast<void**>(glfwGetWindowUserPointer(window->window))[2] = this;
        auto cursor_pos_callback = [](GLFWwindow* w, double xpos, double ypos)
        {
            static_cast<RawInput*>(static_cast<void**>(glfwGetWindowUserPointer(w))[2])->updateCamera(w, xpos, ypos);
        };
        glfwSetCursorPosCallback(window->window, cursor_pos_callback);
    }
    
    float lastX;
    float lastY;
    bool firstMouse = true;
	float deltaTime = 0.0f;
	float lastFrame = 0.0f;
    void updateCamera(GLFWwindow* w, double xpos,double ypos)
    {
        if (firstMouse)
        {
            lastX = xpos;
            lastY = ypos;
            firstMouse = false;
        }

        float xoffset = xpos - lastX;
        float yoffset = lastY - ypos; // reversed since y-coordinates go from bottom to top

        lastX = xpos;
        lastY = ypos;
        camera->processMouseInput(xoffset, yoffset);

    }
    void MoveCameraRawKeys()
    {
        if (glfwGetKey(window->window, controlToGLFWkeys["FORWARD_KEY"]) == GLFW_PRESS)
            camera->ProcessKeyboard(0, deltaTime);
        if (glfwGetKey(window->window, controlToGLFWkeys["BACKWARD_KEY"]) == GLFW_PRESS)
            camera->ProcessKeyboard(1, deltaTime);
        if (glfwGetKey(window->window, controlToGLFWkeys["LEFT_KEY"]) == GLFW_PRESS)
            camera->ProcessKeyboard(2, deltaTime);
        if (glfwGetKey(window->window, controlToGLFWkeys["RIGHT_KEY"]) == GLFW_PRESS)
            camera->ProcessKeyboard(3, deltaTime);
    }
    void updateDeltaTime()
    {
        float currentFrame = static_cast<float>(glfwGetTime());
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;
    }
    

};
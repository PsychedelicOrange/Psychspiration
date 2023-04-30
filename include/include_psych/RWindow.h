#pragma once
#include <RSettings.h>
#include <iostream>
#include <functional>

void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
    // make sure the viewport matches the new window dimensions; note that width and 
    // height will be significantly larger than specified on retina displays.
    glViewport(0, 0, width, height);
}
void oneTimeGladLoader() {
    // glad: load all OpenGL function pointers
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        std::cout << "Failed to initialize GLAD" << std::endl;
    }
}

class Window {
public:
    int height, width;
    string display_mode;
    GLFWwindow* window;
	Window(Settings settings): height(settings.resolution[1]),width(settings.resolution[0]),display_mode(settings.display_mode) {
        // glfw: initialize and configure
        glfwInit();
        glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
        glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
        glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
        glfwWindowHint(GLFW_SAMPLES, 4);
        glfwWindowHint(GLFW_OPENGL_DEBUG_CONTEXT, true);
        //glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_COMPAT_PROFILE);

#ifdef __APPLE__
        glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif
        // glfw window creation
        int* count = new int;
        if (display_mode == "fullscreen")
            window = glfwCreateWindow(width, height, "Psychspiration", (glfwGetMonitors(count))[0], NULL);
        else if (display_mode == "windowed")
            window = glfwCreateWindow(width, height, "Psychspiration", 0, NULL);
        else
            std::cout << "Window has incorrect display_mode { " << display_mode << "}";
        if (window == NULL)
        {
            std::cout << "Failed to create GLFW window" << std::endl;
            glfwTerminate();
        }
        glfwMakeContextCurrent(window);
        // tell GLFW to capture our mouse
        glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
        glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
        oneTimeGladLoader();
	};
    /*glfwSetCursorPosCallback(window, mouse_callback);
    glfwSetScrollCallback(window, scroll_callback);
    glfwSetKeyCallback(window, key_callback);*/
};

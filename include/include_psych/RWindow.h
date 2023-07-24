#pragma once
#include <RSettings.h>
#include <iostream>
#include <functional>
#include <RInput.h>

class Window {
public:
    int height, width;
    DisplayType display_mode;
    GLFWwindow* window;
	Window(Input* input): height(input->settings.resolution[1]),width(input->settings.resolution[0]),display_mode(input->settings.display_mode) {
        // glfw: initialize and configure
        glfwInit();
        glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
        glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
        glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
        //glfwWindowHint(GLFW_SAMPLES, 4);
        glfwWindowHint(GLFW_OPENGL_DEBUG_CONTEXT, true);
        //glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_COMPAT_PROFILE);

#ifdef __APPLE__
        glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif
        // glfw window creation
        int* count = new int;
        if (display_mode == DisplayType::FULLSCREEN)
            window = glfwCreateWindow(width, height, "Psychspiration", (glfwGetMonitors(count))[0], NULL);
        else if (display_mode == DisplayType::WINDOWED)
            window = glfwCreateWindow(width, height, "Psychspiration", 0, NULL);
        else
            std::cout << "Window has incorrect display_mode (FULLSCREEN, WINDOWED, WINDOWED_FULLSCREEN) : " << (int)display_mode;
        if (window == NULL)
        {
            std::cout << "Failed to create GLFW window" << std::endl;
            glfwTerminate();
        }
        glfwMakeContextCurrent(window);
        // enable infinte mouse
        glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
        // raw mouse input if supported
        if (glfwRawMouseMotionSupported())
            glfwSetInputMode(window, GLFW_RAW_MOUSE_MOTION, GLFW_TRUE);

        // Set callbacks
        void** userPointers = new void* [3];
        userPointers[0] = input;
        userPointers[1] = window;

        glfwSetWindowUserPointer(window, userPointers);
        auto key_callback = [](GLFWwindow* w, int key, int scancode, int action, int mods)
        {
            static_cast<Input*>(static_cast<void**>(glfwGetWindowUserPointer(w))[0])->key_callback( key, scancode, action, mods);
        };
        glfwSetKeyCallback(window, key_callback);
        
        auto cursor_pos_callback = [](GLFWwindow* w, double xpos, double ypos)
        {
            static_cast<Input*>(static_cast<void**>(glfwGetWindowUserPointer(w))[0])->mouse_movement_callback( xpos, ypos);
        };
        glfwSetCursorPosCallback(window, cursor_pos_callback);

        auto mouse_button_callback = [](GLFWwindow* w, int button, int action, int mods)
        {
            static_cast<Input*>(static_cast<void**>(glfwGetWindowUserPointer(w))[0])->mouse_button_callback(button, action, mods);
        };
        glfwSetMouseButtonCallback(window, mouse_button_callback);

        auto scroll_callback = [](GLFWwindow* w, double xoffset, double yoffset)
        {
            static_cast<Input*>(static_cast<void**>(glfwGetWindowUserPointer(w))[0])->scroll_callback(xoffset,yoffset);
        };
        glfwSetScrollCallback(window, scroll_callback);

        auto framebuffer_callback = [](GLFWwindow* w, int width, int height)
        {
            static_cast<Window*>(static_cast<void**>(glfwGetWindowUserPointer(w))[1])->framebuffer_size_callback(w, width, height);
        };
        glfwSetFramebufferSizeCallback(window, framebuffer_callback);
        
        oneTimeGladLoader();
        input->registerKeyCallback("EXIT_KEY",1, [=]() {this->close(); });
	};
    void close()
    {
        glfwSetWindowShouldClose(window, true);
    }
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
};

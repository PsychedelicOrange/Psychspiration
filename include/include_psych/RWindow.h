#pragma once
#include <RSettings.h>
#include <iostream>
#include <functional>
#include <RInput.h>
#include <glad/glad.h>
#include <GLFW/glfw3.h>
class Window {
public:
    int height, width;
    DisplayType display_mode;
    GLFWwindow* window;
    Window(Input* input);
    void enableInfiniteMouse();
    void disableInfiniteMouse();
    void close();
    void framebuffer_size_callback(GLFWwindow* window, int width, int height);
    void oneTimeGladLoader();
};

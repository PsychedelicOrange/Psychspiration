#pragma once
#include <RWindow.h>
#include <imgui.h>
#include <imgui/backends/imgui_impl_glfw.h>
#include <imgui/backends/imgui_impl_opengl3.h>
#include <glm/glm.hpp>
#include <implot.h>
#include <implot_internal.h>
#include<State.h>
struct RollingBuffer {
    float Span;
    ImVector<ImVec2> Data;
    RollingBuffer() {
        Span = 10.0f;
        Data.reserve(2000);
    }
    void AddPoint(float x, float y) {
        float xmod = fmodf(x, Span);
        if (!Data.empty() && xmod < Data.back().x)
            Data.shrink(0);
        Data.push_back(ImVec2(xmod, y));
    }
};
struct ScrollingBuffer {
    int MaxSize;
    int Offset;
    ImVector<ImVec2> Data;
    ScrollingBuffer(int max_size = 2000) {
        MaxSize = max_size;
        Offset = 0;
        Data.reserve(MaxSize);
    }
    void AddPoint(float x, float y) {
        if (Data.size() < MaxSize)
            Data.push_back(ImVec2(x, y));
        else {
            Data[Offset] = ImVec2(x, y);
            Offset = (Offset + 1) % MaxSize;
        }
    }
    void Erase() {
        if (Data.size() > 0) {
            Data.shrink(0);
            Offset = 0;
        }
    }
};
class Gui {
	bool show_demo_window = true;
	bool show_another_window = false;
	ImGuiIO io;
    RollingBuffer fpsValuesPlot;
    RollingBuffer aPlot;
    vector<string> debugVariables;
    float t = 0;
public:
	Gui(Window& w);
	~Gui();
	void startFrame();
    void plotA(float value, int maxAxis);
	void doOverlay(bool* p_open = NULL);
    void stateWindow(State& state);
    void doDebugVariables();
    void addDebugVariable(float value,string name);
    void EndFrame();
	void render();
};
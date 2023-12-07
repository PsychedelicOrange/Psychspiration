#include <gui.h>
Gui::Gui(Window& window)
{
	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImPlot::CreateContext();
	io = ImGui::GetIO(); (void)io;
	io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;     // Enable Keyboard Controls
	//io.ConfigFlags |= ImguiConfig
	//io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;      // Enable Gamepad Controls
	
	ImGui::StyleColorsDark();

	ImGui_ImplGlfw_InitForOpenGL(window.window, true);
	ImGui_ImplOpenGL3_Init("#version 460");
}
void Gui::startFrame() {
	ImGui_ImplOpenGL3_NewFrame();
	ImGui_ImplGlfw_NewFrame();
	ImGui::NewFrame();
}
void Gui::doOverlay(bool* p_open)
{
	ImGui::ShowDemoWindow(p_open);
	p_open = new bool(true);
	static int location = 0;
	io = ImGui::GetIO();
	ImGuiWindowFlags window_flags = ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_NoSavedSettings | ImGuiWindowFlags_NoFocusOnAppearing | ImGuiWindowFlags_NoNav;
	if (location >= 0)
	{
		const float PAD = 10.0f;
		const ImGuiViewport* viewport = ImGui::GetMainViewport();
		ImVec2 work_pos = viewport->WorkPos; // Use work area to avoid menu-bar/task-bar, if any!
		ImVec2 work_size = viewport->WorkSize;
		ImVec2 window_pos, window_pos_pivot;
		window_pos.x = (location & 1) ? (work_pos.x + work_size.x - PAD) : (work_pos.x + PAD);
		window_pos.y = (location & 2) ? (work_pos.y + work_size.y - PAD) : (work_pos.y + PAD);
		window_pos_pivot.x = (location & 1) ? 1.0f : 0.0f;
		window_pos_pivot.y = (location & 2) ? 1.0f : 0.0f;
		ImGui::SetNextWindowPos(window_pos, ImGuiCond_Always, window_pos_pivot);
		window_flags |= ImGuiWindowFlags_NoMove;
	}
	else if (location == -2)
	{
		// Center window
		ImGui::SetNextWindowPos(ImGui::GetMainViewport()->GetCenter(), ImGuiCond_Always, ImVec2(0.5f, 0.5f));
		window_flags |= ImGuiWindowFlags_NoMove;
	}
	ImGui::SetNextWindowBgAlpha(0.35f); // Transparent background
	if (ImGui::Begin("Example: Simple overlay",NULL, window_flags))
	{
		//IMGUI_DEMO_MARKER("Examples/Simple Overlay");
		ImGui::Text("Psycshpiration\n" "(Middle Mouse Click to toggle overlay)");
		ImGui::Separator();
		ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / io.Framerate, io.Framerate);
		if (ImGui::BeginPopupContextWindow())
		{
			if (ImGui::MenuItem("Custom", NULL, location == -1)) location = -1;
			if (ImGui::MenuItem("Center", NULL, location == -2)) location = -2;
			if (ImGui::MenuItem("Top-left", NULL, location == 0)) location = 0;
			if (ImGui::MenuItem("Top-right", NULL, location == 1)) location = 1;
			if (ImGui::MenuItem("Bottom-left", NULL, location == 2)) location = 2;
			if (ImGui::MenuItem("Bottom-right", NULL, location == 3)) location = 3;
			if (p_open && ImGui::MenuItem("Close")) *p_open = false;
			ImGui::EndPopup();
		}
		float history = 10.0f;		
		t += ImGui::GetIO().DeltaTime;
		fpsValuesPlot.AddPoint(t, io.Framerate);
		ImPlotAxisFlags Axisflags = ImPlotAxisFlags_NoLabel | ImPlotAxisFlags_AutoFit;// ImPlotAxisFlags_NoTickLabels;
		ImPlotFlags plotFlags = ImPlotFlags_CanvasOnly;
		//ImPlotCol col = 
		if (ImPlot::BeginPlot("##Rolling", ImVec2(-1, 100), plotFlags)) {
			ImPlot::SetupAxes(nullptr, nullptr, Axisflags | ImPlotAxisFlags_NoTickLabels, Axisflags);
			ImPlot::SetupAxisLimits(ImAxis_X1, 0, history, ImGuiCond_Always);
			ImPlot::SetupAxisLimits(ImAxis_Y1, 0, 400);
			ImPlot::PlotLine("FPS", &fpsValuesPlot.Data[0].x, &fpsValuesPlot.Data[0].y, fpsValuesPlot.Data.size(), 0, 0, 2 * sizeof(float));
			ImPlot::EndPlot();
		}

	}
	ImGui::End();
}
void Gui::stateWindow(State& state){
	if (ImGui::Begin("Example: Simple overlay"))
	{
		ImGui::Text("Change state: ");
		ImGui::SliderFloat("exposure", &state.exposure, 0.0f, 1.0f);
		if (ImGui::Button("Hdr toggle")){
			state.hdr = !(bool)state.hdr;
		}
	}ImGui::End();
}
void Gui::plotA(float value,int maxAxis) {
	float history = 10.0f;
	//t += ImGui::GetIO().DeltaTime; Called already in FPS overlay
	aPlot.AddPoint(t, value);
	ImPlotAxisFlags Axisflags = ImPlotAxisFlags_NoLabel | ImPlotAxisFlags_AutoFit; //ImPlotAxisFlags_NoTickLabels;
	ImPlotFlags plotFlags = ImPlotFlags_CanvasOnly;
	ImGui::Begin("Debug Plot A");
	ImGui::Text("A: %.3f", value);
	if (ImPlot::BeginPlot("##Rolling", ImVec2(-1, 100), plotFlags)) {
		ImPlot::SetupAxes(nullptr, nullptr, Axisflags, Axisflags);
		ImPlot::SetupAxisLimits(ImAxis_X1, 0, history, ImGuiCond_Always);
		ImPlot::SetupAxisLimits(ImAxis_Y1, 0, maxAxis);
		ImPlot::PlotLine("plotA", &aPlot.Data[0].x, &aPlot.Data[0].y, aPlot.Data.size(), 0, 0, 2 * sizeof(float));
		ImPlot::EndPlot();
	}
	ImGui::End();
}
void Gui::addDebugVariable(float value, string name)
{
	debugVariables.push_back(name + ": " + std::to_string(value));
}
void Gui::EndFrame()
{
	render();
	debugVariables.clear();
}
void Gui::render()
{
	ImGui::Render();
	ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
}
void Gui::doDebugVariables()
{
	ImGui::Begin("Debug Variables");
	for(auto var : debugVariables)
	{
		ImGui::Text(var.c_str());
	}
	ImGui::End();
}
Gui::~Gui()
{
	ImGui_ImplOpenGL3_Shutdown();
	ImGui_ImplGlfw_Shutdown();
	ImPlot::DestroyContext();
	ImGui::DestroyContext();

}
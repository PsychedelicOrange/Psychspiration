#include <Globals.h>
#include <glad/glad.h>
#include <RSettings.h>
#include <RWindow.h>
#include <RModelManager.h>
#include <ConsoleDebug.h>
int main()
{
	Settings settings;
	Window Windows(settings);
	RModelManager* modelMan = RModelManager::getInstance();
	ConsoleDebug console;
	RModel* model = modelMan->getModel("SM_Deccer_Cubes");
	console.log(model); 
	
}

#include <Settings.h>
#include <FileIO.h>
std::vector<std::string> split(const std::string& s, char delim);
void Settings::update()
{
	std::vector<std::string> settings = parse("Settings.csv");
    SCR_WIDTH = stoi(settings[0]);
    SCR_HEIGHT = stoi(settings[1]);
    resourcePath = settings[2];
    //shaderPath = settings[3];
    hdr = stoi(settings[3]);
    exposure = stof(settings[4]);
    MSAA = stoi(settings[5]);
    AF = stoi(settings[6]);
}



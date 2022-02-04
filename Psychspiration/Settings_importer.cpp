#include "Psychspiration/Settings_importer.h"
std::vector<std::string> split(const std::string& s, char delim);
void Settings::update()
{
	std::fstream file{"C:\\Users\\parth\\source\\repos\\Psychspiration\\Settings.csv"};
    std::string prop;
    if (!file)
    {   // Print an error and exit
        std::cerr << "Uh oh, Sample.dat could not be opened for reading!" << std::endl;
    }
    while (file)
    {
        std::getline(file, prop);
    }
    std::cout << prop;

    std::vector<std::string> settings;
    settings = split(prop, ',');
    SCR_WIDTH = stoi(settings[0]);
    SCR_HEIGHT = stoi(settings[1]);
    resourcePath = settings[2];
    shaderPath = settings[3];
}
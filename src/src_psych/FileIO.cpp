#include <FileIO.h>
#ifdef _WIN32
#include <Windows.h>
#endif
#ifdef __linux__
#include <unistd.h>
#endif
#include <filesystem>
#include <sstream>
#include <fstream>
#include <iostream>
// for string delimiter
std::string pathResource = getdefpathResource();
std::vector<std::string> split(const std::string& s, char delim) {
    std::vector<std::string> result;
    std::stringstream ss(s);
    std::string item;
    while (getline(ss, item, delim)) {
        result.push_back(item);
    }
    return result;
}
bool checkFileExists(std::string path) {
	std::ifstream file;
	file.open(path);
	if (file)
	{
		return true;
	}
	else
	{
		return false;
	}
}
std::string getStringFromDisk_direct(std::string path)
{
	std::fstream file{  path };
	std::string prop;
	if (!file)
	{   // Print an error and exit
		std::cerr << "Uh oh, \" " << path << "\" could not be opened for reading!" << std::endl;
		return "";
	}
	while (file)
	{
		std::getline(file, prop);
	}
	file.close();
	return prop;
}

std::string getStringFromDisk(std::string path)
{
	std::cout << "Resource path: " << pathResource << std::endl;
	std::fstream file{pathResource+path};
	std::string prop;
	if (!file)
	{
		std::cerr << "Uh oh, \" " << path << "\" could not be opened for reading!" << std::endl;
	}
	while (file)
	{
		std::getline(file, prop);
	}
	file.close();
	return prop;
}
std::vector<std::string> parse(std::string path)
{
    return split(getStringFromDisk(path), ',');
}
std::string getRelativePath()
{
	std::string tempString;
	

#ifdef _WIN32
	char pBuf[MAX_PATH];
	size_t len = sizeof(pBuf);
	GetModuleFileNameA(NULL, pBuf, MAX_PATH);
#endif 
#ifdef __linux__
	char pBuf[100];
	size_t len = sizeof(pBuf);
	readlink("/proc/self/exe", pBuf, len);
#endif
	std::filesystem::path relPath(pBuf);
	relPath.remove_filename();
	return relPath.string();
}


std::string getdefpathResource()
{
	std::filesystem::path debug = getRelativePath();
	for (int i = 0; i < 3; i++)
		debug = debug.parent_path();
	debug /= std::filesystem::path("Resources");
	return debug.string();
}



std::vector<std::string> getSceneList()
{
	std::filesystem::path sceneFolder = pathResource;
	sceneFolder /= std::filesystem::path("Scenes");
	int sceneCount = static_cast<int>(std::distance(std::filesystem::directory_iterator(sceneFolder), std::filesystem::directory_iterator()));
	std::vector<std::string> sceneList;
	for (const auto& entry : std::filesystem::directory_iterator(sceneFolder))
		sceneList.push_back(entry.path().stem().string());

	return sceneList;
}

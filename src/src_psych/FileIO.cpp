#include<FileIO.h>
#include <Windows.h>
#include <filesystem>
#include <sstream>
#include <fstream>
#include <iostream>
// for string delimiter
std::vector<std::string> split(const std::string& s, char delim) {
    std::vector<std::string> result;
    std::stringstream ss(s);
    std::string item;
    while (getline(ss, item, delim)) {
        result.push_back(item);
    }
    return result;
}

std::string getStringFromDisk_direct(std::string path)
{
	std::fstream file{  path };
	std::string prop;
	if (!file)
	{   // Print an error and exit
		std::cerr << "Uh oh, \" " << path << "\" could not be opened for reading!" << std::endl;
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
	std::fstream file{ pathResource + path };
	std::string prop;
	if (!file)
	{   // Print an error and exit
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
	
	char pBuf[MAX_PATH];
	size_t len = sizeof(pBuf);
	int bytes = GetModuleFileNameA(NULL, pBuf, MAX_PATH);
	bytes ? bytes : -1;
	std::filesystem::path relPath(pBuf);
	relPath.remove_filename();
	return relPath.string();
	/*std::wstring wstr = pBuf;
	if (!wstr.empty())
	{
		int sizeRequired = WideCharToMultiByte(CP_UTF8, 0, wstr.c_str(), -1, NULL, 0, NULL, NULL);

		if (sizeRequired > 0)
		{
			std::vector<char> utf8String(sizeRequired);
			int bytesConverted = WideCharToMultiByte(CP_UTF8, 0, wstr.c_str(),
				-1, &utf8String[0], utf8String.size(), NULL,
				NULL);
			if (bytesConverted != 0)
			{
				tempString = &utf8String[0];
			}
			else
			{
				std::stringstream err;
				err << __FUNCTION__
					<< " std::string WstrToUtf8Str failed to convert wstring '"
					<< wstr.c_str() << L"'";
				throw std::runtime_error(err.str());
			}
		}
	}
	tempString.erase(tempString.size() - 18, tempString.size() - 1);
	return tempString;*/
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

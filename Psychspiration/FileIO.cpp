#include<FileIO.h>
#include <Windows.h>
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
std::string getStringFromDisk(std::string path)
{
	std::fstream file{ getRelativePath() + path };
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
	WCHAR pBuf[1024];
	size_t len = sizeof(pBuf);
	int bytes = GetModuleFileName(NULL, pBuf, len);
	bytes ? bytes : -1;
	std::wstring wstr = pBuf;
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
	return tempString;
}
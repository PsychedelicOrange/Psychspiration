#pragma once
#include <string>
#include <vector>
extern std::string pathResource;
std::string getRelativePath();
const std::string relativePath= getRelativePath();
std::string getdefpathResource();

std::vector<std::string> split(const std::string& s, char delim);
std::string getStringFromDisk_direct(std::string path);
std::string getStringFromDisk(std::string path);
std::vector<std::string> parse(std::string path);
std::vector<std::string> getSceneList();
bool checkFileExists(std::string path);
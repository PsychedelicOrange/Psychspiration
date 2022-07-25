#pragma once
#include <string>
#include <vector>
std::string getRelativePath();
const std::string relativePath= getRelativePath();
std::vector<std::string> split(const std::string& s, char delim);
std::string getStringFromDisk_direct(std::string path);
std::string getStringFromDisk(std::string path);
std::vector<std::string> parse(std::string path);

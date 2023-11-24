#pragma once
#include <Hull.h>
#include <string>
using std::string;
class HullBuilder {
public:
	void builder(string path, std::vector<Hull*>& hulls);
};
#pragma once
#include <RObject.h>
class ConsoleDebug {
public :
	ConsoleDebug() = default;
	void log(RObject* object);
	void log(RModel* model);
	void log(Material material);
};
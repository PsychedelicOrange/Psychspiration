 #pragma once
#include <RLightTypes.h>
#include <RLightObjects.h>
#include <vector>

class pLightBuffer
{
public:
	unsigned int lightUBO;
    pLightBuffer():lightUBO(-1){
	};
    void innit();
	void updateBuffer(std::vector<RPointLight*> lightList);
};
#pragma once
#include <Shader.h>
#include <RCameraObject.h>
#include <RLights.h>
class RenderState
{
public:
	Shader* shader;
	RCamera* camera;
	RLights* lights;
};
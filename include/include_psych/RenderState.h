#pragma once
#include <Shader.h>
#include <RCameraObject.h>
#include <RLights.h>
class RenderState
{
	Shader* shader;
	RCamera* camera;
	RLights* lights;
};
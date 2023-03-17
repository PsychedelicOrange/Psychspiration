#pragma once
#include <enums.h>
class RCamera
{
public:
	//common
	float zfar, znear;

	CameraType cameraType;

	// orthographic parameters
	float xmag, ymag; // xmag = hieght/2 ..etc.

	//perspective parameters
	float aspectRatio, yfov;
};
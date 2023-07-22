#pragma once
#include <enums.h>
class RCamera
{
public:
	//common
	float zfar = 100.0, znear = 0.1;

	CameraType cameraType = CameraType::PERSPECTIVE;

	// orthographic parameters
	float xmag, ymag; // xmag = hieght/2 ..etc.

	//perspective parameters
	float aspectRatio=(float)16.0/9.0, yfov=45;
};
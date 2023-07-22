#pragma once
// Albedo,Normal,MetallicRoughness,Emmisive,Other
enum class TextureType
{
	Albedo,Normal,MetallicRoughness,Emmisive,Other
};
// OPAQUE,BLEND,MASK
enum class AlphaType
{
	OPAQUE,BLEND,MASK
};
// PERSPECTIVE, ORTHOGRAPHIC
enum class CameraType
{
	PERSPECTIVE,ORTHOGRAPHIC
};
//POINT, DIRECTIONAL,SPOT , AREA
enum class LightType
{
	POINT,DIRECTIONAL,SPOT,AREA
};
// FULLSCREEN, WINDOWED, WINDOWED_FULLSCREEN
enum class DisplayType
{
	FULLSCREEN, WINDOWED, WINDOWED_FULLSCREEN
};
// FORWARD_KEY, BACKWARD_KEY, RIGHT_KEY, LEFT_KEY
enum class Controls
{
	FORWARD_KEY, BACKWARD_KEY, RIGHT_KEY, LEFT_KEY,PLAY_KEY
};

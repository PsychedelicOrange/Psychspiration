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
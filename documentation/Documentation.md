# Psychspiration Documentation
## Index
	1.Build Instructions										
	2.How to export scenes from Blender
# 1. Build instructions 
### Clone Repository
```
git clone https://github.com/PsychedelicOrange/Psychspiration
cd Psychspiration
```
### Get Vcpkg
```
git clone https://github.com/Microsoft/vcpkg.git
.\vcpkg\bootstrap-vcpkg.bat
```
### Install libraries and build 
```
cmake -B build -S . -DCMAKE_TOOLCHAIN_FILE=.\vcpkg\scripts\buildsystems\vcpkg.cmake -DVCPKG_TARGET_TRIPLET=x64-windows-static
```
# 2. How to export scenes from Blender
## Resource Directory Structure
```
Resources
├───Models
├───Hulls
├───Scenes
├───Textures
├───Shaders
```
## Prerequisites
* Blender 2.8
* [V-HACD Addon for Blender](https://github.com/andyp123/blender_vhacd) (optional for Physics)
* [compressonator](https://github.com/GPUOpen-Tools/compressonator) (optional for Fast Loading)
## How it works
* Inbuilt GLTF exporter in blender is used to export models to /Models.
* compressonator is used to compress texture to .dds to /Textures
* Scene description is written as a json to .scene file in /Scenes.
	*  _Scenes/sceneName.scene_
	```
	{
		'objects':[ 
				{name,export_name,scale,translate,rotate,transform,
				hulls:[
					{name,export_name,transform}
				]}
			],
		'lights':[
			{name,location,color,power,range,use_shadow}
			]
		
		}
	```
## Before running Blender_scripts/Scene_exporter.py
* Naming Conventions
    * Object names should'nt contain '\_hulls\_' 
	* Texture file names should'nt contain 'png' or 'jpg' in the name (excluding the extension obviously).
* Material Conventions
	* Follow [GLTF conventions](https://docs.blender.org/manual/en/2.80/addons/io_scene_gltf2.html) 
	* Set Export path in blender to Root/Resources
	* Need atleast one point light in scene (To fix)
* Generate Physics Hulls ( Optional : Only if physics are required )
	1. Convert all objects to meshes, apply modifiers, etc.
	2. V-HACD all desired moving physics objects 
	3. Parent the generated hulls to the Main(graphical) object

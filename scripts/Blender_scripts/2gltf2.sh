#!/bin/bash
if [ "$#" = 1 ]
then
	/home/parth/code/Psychspiration/bin/TestVHACD $1
	/home/parth/Downloads/blender-3.6.5-linux-x64/blender -b -P 2gltf2.py -- "$1"
else
	echo To glTF 2.0 converter.
	echo Supported file formats: .abc .blend .dae .fbx. .obj .ply .stl .usd .wrl .x3d
	echo 
	echo 2gltf2.sh [filename]
fi

# Psychspiration Documentation
## Index										
	1.Directory Structure
	2.Anatomy of a Scene
	3.How to export scenes from Blender
	4.How to render scenes in Psychspiration
	5.How to render individual objects in Psych
	6.How to construct objects from models not exported from blender
## Directory Structure
```
Root
├───Resources
│       ├───Models
│       ├───Scenes
│       └───Textures
├───shaders
```
## Anatomy of a Scene
```
Scenes
├───sceneName.scene
```
### sceneName.scene
```
{
    'objects':[ 
    		{name,export_name,transform}
        ],
    'lights':[
		{name,location,color,power,range,use_shadow}
        ]
    
    }
```

## How to export scenes from Blender
 
### Before running Blender_scripts/Scene_exporter.py
* Naming Conventions
    * Object names should'nt contain '_' at the start ( because '_tree' is used as the hull of 'tree')
	* Texture file names should'nt contain 'png' or 'jpg' in the name (excluding the extension obviously).
* Material Conventions
	* Follow [GLTF conventions](https://docs.blender.org/manual/en/2.80/addons/io_scene_gltf2.html) 
	* Set Export path in blender to Root/Resources
	* Need atleast one point light in scene (To fix)
	* Generate Physics Hulls ( Optional : Only if physics are required )
		1. Convert all objects to meshes, apply modifiers, etc.
		2. V-HACD all desired moving physics objects 
		3. Parent the generated hulls to the Main(graphical) object

## How to render scenes in Psychspiration

```
//in main()

	// Create a Scene Object 
		Scene myscene(sceneName); 
	// Load all models and textures of all objects in the scene
		myscene.loadObjects();

// in render loop
	// updates transforms of all Objects in the scene
		myscene.updatePhysics();
	// finally draw all objects
		myscene.drawObjects();
```
## How to render individual objects in Psychspiration

1. Use the constructors to make a New Object
2. Please refer to the implementation of the Scene class to see what steps need to done and where

## How to construct objects from models not exported from blender

```
Object* object = new Object("myobject",new Model(model_path_relative_to_root));
```

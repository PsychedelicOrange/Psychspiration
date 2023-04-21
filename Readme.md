# Psychspiration

Hobby Game Engine 
![Engine Screenshot](https://live.staticflickr.com/65535/52650553805_552e7114ce_b.jpg)
[More Screenshots](https://www.flickr.com/photos/197526599@N02/albums/72177720307335647)
[Demo Download](https://drive.google.com/file/d/1xFDgGSghTHYpK5WjfNraR_NswliVC89u/view?usp=sharing)
[Build Instructions](./documentation/Documentation.md)
# Features

* PBR shading based on learnopengl.com
* Supports dynamic point,directional lights with shadows (PCF)
* Import scenes from blender (Custom .py exporter)
* MSAA, Anti-Aliasing, HDR, Transparency,Instanced rendering 
* Integrated Bullet Physics (*broken in current version*)

# Goals
## Graphics Feature Addition
* [x] Better GLTF Support
* [x] Transparency
* [x] IBL
* [ ] GI
* [ ] SSR
## Graphics Optimization
* [x] Instanced rendering
* [x] View Frustrum Culling
## Asset pipeline / setting up tool chain btw blender / engine
* [ ] Make a gui for a resource Manager for deleting/viewing/creating Scenes
* [ ] Compress textures ? (.dds 10 mipmaps take up soo much extra space)
## Building / distributing
* [x] Easy to setup dev env
* [ ] Achieve distributable builds
* [ ] Set up one click build button



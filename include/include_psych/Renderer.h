#pragma once
#include <RLightTypes.h>
#include <RObject.h>
#include <RCameraObject.h>
#include <Shader.h>
#include <SceneLight.h>
#include <utility>
#include <Line.h>
#include <glad/glad.h>
#include <RInput.h>
using std::pair;
class Renderer
{
public:
	
	// Shaders
	vector<Shader*> shaders;
	// live lists
	vector<RObject*> staticObjects; // static environment objects 
	vector<RMesh*> staticMeshes; // derived from staticObjects
	vector<RObject*> dynamicObjects; // dynamic objects
	vector<RPointLight*> pLightsLive; // all point lights loaded
	vector<RDirectionalLight*> dLightsLive; // all directional lights loaded
	vector<RSpotLight*> sLightsLive; // all spot lights loaded
	vector<RAreaLight*> aLightsLive; // all area lights loaded

	vector<RCameraObject*> camerasLive; // all cameras being used

	// Buffers
	vector<GLuint> vaos = vector<GLuint>(5, 0);
	vector<GLuint> vbos = vector<GLuint>(5, 0);
	pLightBuffer plightBuffer;
	
	// Miscelleanous
	Line line;
	Input& input;
	Renderer(Input& input) : input(input) {};
	void setUpEvents();
	void insertObjects(vector<RObject*>& objects);
	void updateShaders();
	void innit();
	void prepareMeshes();
	void initVABOs();
	void drawStatic();
	void draw(RObject* obj);
	void draw(vector<RObject*> objects);
	void drawInstanced();
	void setGlobalUniforms(Shader* shader);
	void setMeshUniforms(RMesh* mesh, Shader* shader);
};
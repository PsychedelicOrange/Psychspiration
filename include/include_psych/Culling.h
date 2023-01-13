#pragma once
#include <glad/glad.h>
#include <glm/gtc/type_ptr.hpp>
#include <vector>
#include <Shader.h>
struct VolumeTileAABB {
	glm::vec4 minPoint;
	glm::vec4 maxPoint;
};
struct screenToView {
	glm::mat4 inverseProjection;
	glm::uvec4 tileSizes; // for padding i assume
	glm::uvec2 screenDimensions;
};
class Culling {
public:
	unsigned int clusterAABB;// the ssbo
	screenToView settings;
	std::vector <VolumeTileAABB> clusterAABBs = std::vector <VolumeTileAABB>(3456); // cpu side read back for debug
	Shader constructClustersShader = Shader("clusterShader.comp");
	Culling(glm::mat4 inverseProjection,
		glm::uvec4 tileSizes = glm::uvec4(120),
		glm::uvec2 screenDimensions = { 1920,1080 })
	{
		glGenBuffers(1, &clusterAABB);
		glBindBuffer(GL_SHADER_STORAGE_BUFFER, clusterAABB);
		glBufferData(GL_SHADER_STORAGE_BUFFER, 3456*sizeof(VolumeTileAABB), NULL, GL_STATIC_READ);
		glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 1, clusterAABB);
		glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);

		settings.tileSizes = tileSizes;
		settings.screenDimensions = screenDimensions;
		settings.inverseProjection = inverseProjection;
	};
	void dispatch()
	{
		constructClustersShader.use();
		glDispatchCompute(16, 9, 24);
	}
};
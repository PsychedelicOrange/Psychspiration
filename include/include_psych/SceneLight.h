 #pragma once
#include <Light.h>
#include <vector>
class SceneLight
{
public:
	unsigned int lightUBO;
	SceneLight(){

		glGenBuffers(1, &lightUBO);
		glBindBuffer(GL_UNIFORM_BUFFER, lightUBO);
        glBindBufferBase(GL_UNIFORM_BUFFER, 3, lightUBO);
        glBindBuffer(GL_UNIFORM_BUFFER, 0);
	};
	void updateBuffer(std::vector<PointLight>& lightList)
	{
        int numLights = lightList.size();
        glBindBuffer(GL_UNIFORM_BUFFER, lightUBO);
        glBufferData(GL_UNIFORM_BUFFER, numLights * sizeof(GPULight), NULL, GL_STREAM_DRAW);

        GLint bufMask = GL_READ_WRITE;
        GPULight* lights_buffer;
        lights_buffer = (struct GPULight*)glMapBuffer(GL_UNIFORM_BUFFER, bufMask);
        for (unsigned int i = 0; i < numLights; i++) {
            //Fetching the light from the current scene
            lights_buffer[i].position = glm::vec4(lightList[i].position, 1.0f);
            lights_buffer[i].color = glm::vec4(lightList[i].color, 1.0f);
            lights_buffer[i].enabled = 1;
            lights_buffer[i].intensity = lightList[i].power;
            lights_buffer[i].range = lightList[i].size;
        }
        glUnmapBuffer(GL_UNIFORM_BUFFER);
	}
};
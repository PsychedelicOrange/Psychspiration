 #pragma once
#include <RLightTypes.h>
#include <RLightObjects.h>
#include <vector>
class pLightBuffer
{
public:
	unsigned int lightUBO;
    pLightBuffer():lightUBO(-1){
	};
    void innit()
    {
        glGenBuffers(1, &lightUBO);
		glBindBuffer(GL_UNIFORM_BUFFER, lightUBO);
        glBindBufferBase(GL_UNIFORM_BUFFER, 3, lightUBO);
        glBindBuffer(GL_UNIFORM_BUFFER, 0);
    }
	void updateBuffer(std::vector<RPointLight*> lightList)
	{
        int numLights = lightList.size();
        glBindBuffer(GL_UNIFORM_BUFFER, lightUBO);
        glBufferData(GL_UNIFORM_BUFFER, numLights * sizeof(GPUpLight), NULL, GL_STREAM_DRAW);

        GLint bufMask = GL_READ_WRITE;
        GPUpLight* lights_buffer;
        lights_buffer = (struct GPUpLight*)glMapBuffer(GL_UNIFORM_BUFFER, bufMask);
        for (unsigned int i = 0; i < numLights; i++) {
            //Fetching the light from the current scene
            lights_buffer[i].position = glm::vec4(lightList[i]->position, 1.0f);
            lights_buffer[i].color = glm::vec4(lightList[i]->color, 1.0f);
            lights_buffer[i].enabled = 1;
            lights_buffer[i].intensity = lightList[i]->powerWatts;
            lights_buffer[i].range = lightList[i]->clipRadius;
        }
        glUnmapBuffer(GL_UNIFORM_BUFFER);
	}
};
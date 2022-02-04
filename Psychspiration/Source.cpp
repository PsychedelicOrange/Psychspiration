#include<glad/glad.h>
#include<GLFW/glfw3.h>
#include<iostream>
#include<func.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
const unsigned int WIDTH = 800;
const unsigned int HEIGHT = 600;
glm::vec3 cameraPos = glm::vec3(0.0f, 0.0f, 3.0f);
glm::vec3 cameraFront = glm::vec3(0.0f, 0.0f, -1.0f);
glm::vec3 cameraUp = glm::vec3(0.0f, 1.0f, 0.0f);

float deltatime = 0.0f;
float lastFrame = 0.0f;
float currentFrame = 0;
int main()
{
	// basic innit
	glfwwindowinnit();
	GLFWwindow* window = glfwCreateWindow(WIDTH, HEIGHT, "LearnOpenGL", NULL, NULL);
	errorhandle(window);
	glfwMakeContextCurrent(window);
	glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
	
	gladinnit();
	// opengl functions used from here
	glEnable(GL_DEPTH_TEST);

	Shader myshader("C:\\Users\\parth\\source\\repos\\Psychspiration\\resources\\shaders\\vertex_for_model.vs", "C:\\Users\\parth\\source\\repos\\Psychspiration\\resources\\shaders\\fragment.fs");
	

	unsigned int vbo, vao;
	glGenVertexArrays(1, &vao);
	glGenBuffers(1, &vbo);
	glBindVertexArray(vao);
	glBindBuffer(GL_ARRAY_BUFFER, vbo);

	float vertices[] = {
		-0.5f, -0.5f, 0.0f,
		 0.5f, -0.5f, 0.0f,
		 0.0f,  0.5f, 0.0f
	};
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
	//unbinding
	glEnableVertexAttribArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	float* input;
	float i[4]{ 0 };
	glm::mat4 finaltrans = glm::mat4(1.0f);
	
	//render loop
	while (!glfwWindowShouldClose(window))
	{
		
		input = processInput(window,i);

		glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT);

		myshader.use();

		glm::mat4 model = glm::mat4(1.0f);
		model = glm::rotate(model, glm::radians(-55.0f), glm::vec3(1.0f, 0.0f, 0.0f));
		glm::mat4 view = glm::mat4(1.0f);
		view = glm::translate(view, glm::vec3(0.0f, 0.0f, -3.0f));
		glm::mat4 projection;
		projection = glm::perspective(glm::radians(45.0f), 800.0f / 600.0f, 0.1f, 100.0f);
		glm::mat4 mvp = projection * view * model;
		for (int i = 0; i < 4; i++)
		{
			for (int j = 0; j < 4; j++)
			{
				std::cout << mvp[i][j] << " ";
			}

			// Newline for new row
			std::cout << std::endl;
		}
		//mover(input,0.001, &finaltrans);
		
		myshader.setmat4("mvp", mvp);
		glBindVertexArray(vao);
		glDrawArrays(GL_TRIANGLES, 0, 3);

		glfwSwapBuffers(window);
		glfwPollEvents();
	}
	glDeleteVertexArrays(1, &vao);
	glDeleteBuffers(1, &vbo);
	myshader.delete_();
	glfwTerminate();
	return 0;

}

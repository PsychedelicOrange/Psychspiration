#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <func.h>



void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void mouse_callback(GLFWwindow* window, double xpos, double ypos);
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);
void processInput(GLFWwindow* window);

// settings
const unsigned int SCR_WIDTH = 1920;
const unsigned int SCR_HEIGHT = 1080;

// camera
Camera camera(glm::vec3(0.0f, 0.0f, 3.0f));
float lastX = SCR_WIDTH / 2.0f;
float lastY = SCR_HEIGHT / 2.0f;
bool firstMouse = true;

// timing
float deltaTime = 0.0f;
float lastFrame = 0.0f;
void updatelightloc(std::string x, float deltaTime);

// lighting
glm::vec3 pointLightPositions[] = {
        glm::vec3(0.0f,  -0.5f,  0.5f),
       glm::vec3(0.0f,  0.0f,  0.5f),
        glm::vec3(0.0f,  0.0f,  0.5f),
        glm::vec3(0.0f,  0.0f,  0.5f)
};

int main()
{
    // glfw: initialize and configure
    // ------------------------------
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

#ifdef __APPLE__
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif

    // glfw window creation
    // --------------------
    GLFWwindow* window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "LearnOpenGL", NULL, NULL);
    if (window == NULL)
    {
        std::cout << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
    glfwSetCursorPosCallback(window, mouse_callback);
    glfwSetScrollCallback(window, scroll_callback);
    //glfwWindowHint(GLFW_SAMPLES, 8);

    // tell GLFW to capture our mouse
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

    // glad: load all OpenGL function pointers
    // ---------------------------------------
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        std::cout << "Failed to initialize GLAD" << std::endl;
        return -1;
    }

    // configure global opengl state
    // -----------------------------
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_CULL_FACE);
    //glEnable(GL_MULTISAMPLE);

    //stbi_set_flip_vertically_on_load(true);

   
    // build and compile our shader zprogram
    // ------------------------------------
    //Shader ourShader("C:\\Users\\parth\\source\\repos\\Psychspiration\\resources\\shaders\\model_simple.vs", "C:\\Users\\parth\\source\\repos\\Psychspiration\\resources\\shaders\\model_simple.fs");
    Shader ourShader("C:\\Users\\parth\\source\\repos\\Psychspiration\\resources\\shaders\\vertex.vs", "C:\\Users\\parth\\source\\repos\\Psychspiration\\resources\\shaders\\fragment.fs", nullptr);
    
    Shader lightCubeShader("C:\\Users\\parth\\source\\repos\\Psychspiration\\resources\\shaders\\vertex_lightcube.vs", "C:\\Users\\parth\\source\\repos\\Psychspiration\\resources\\shaders\\fragment_lightcube.fs", nullptr);
    //Model backpack("C:\\Users\\parth\\Desktop\\simple grass block\\grass_obj.obj");
    //Model backpack("C:\\Users\\parth\\source\\repos\\Psychspiration\\resources\\models\\remington_rolling_block_rifle\\scene.gltf");
    float scale = 0.01;
   // Model bulb("C:\\Users\\parth\\source\\repos\\Psychspiration\\resources\\models\\light bulb\\bulb.obj");
    Model axes("C:\\Users\\parth\\source\\repos\\Psychspiration\\resources\\models\\axes.glb");
    // 
     // be sure to activate shader when setting uniforms/drawing objects
    ourShader.use();

    // point light 1
    ourShader.setVec3("pointLights[0].position", pointLightPositions[0]);
    ourShader.setVec3("pointLights[0].color", 1,1,1);
    ourShader.setFloat("pointLights[0].constant", 1.0f);
    ourShader.setFloat("pointLights[0].linear", 0.09);
    ourShader.setFloat("pointLights[0].quadratic", 0.032);
    //spotlight
    ourShader.setVec3("spotLight.color", 1, 1, 1);
    ourShader.setFloat("spotLight.constant", 1.0f);
    ourShader.setFloat("spotLight.linear", 0.09);
    ourShader.setFloat("spotLight.quadratic", 0.032);
    ourShader.setFloat("spotLight.cutOff", glm::cos(glm::radians(12.5f)));
    ourShader.setFloat("spotLight.outerCutOff", glm::cos(glm::radians(15.0f)));
    //materials
    ourShader.setFloat("material.shininess", 256.0f);
    
    /*
    // point light 2
    ourShader.setVec3("pointLights[1].position", pointLightPositions[1]);
    ourShader.setVec3("pointLights[1].ambient", 0.05f, 0.05f, 0.05f);
    ourShader.setVec3("pointLights[1].diffuse", 0.8f, 0.8f, 0.8f);
    ourShader.setVec3("pointLights[1].specular", 1.0f, 1.0f, 1.0f);
    ourShader.setFloat("pointLights[1].constant", 1.0f);
    ourShader.setFloat("pointLights[1].linear", 0.09);
    ourShader.setFloat("pointLights[1].quadratic", 0.032);
    // point light 3
    ourShader.setVec3("pointLights[2].position", pointLightPositions[2]);
    ourShader.setVec3("pointLights[2].ambient", 0.05f, 0.05f, 0.05f);
    ourShader.setVec3("pointLights[2].diffuse", 0.8f, 0.8f, 0.8f);
    ourShader.setVec3("pointLights[2].specular", 1.0f, 1.0f, 1.0f);
    ourShader.setFloat("pointLights[2].constant", 1.0f);
    ourShader.setFloat("pointLights[2].linear", 0.09);
    ourShader.setFloat("pointLights[2].quadratic", 0.032);
    // point light 4
    ourShader.setVec3("pointLights[3].position", pointLightPositions[3]);
    ourShader.setVec3("pointLights[3].ambient", 0.05f, 0.05f, 0.05f);
    ourShader.setVec3("pointLights[3].diffuse", 0.8f, 0.8f, 0.8f);
    ourShader.setVec3("pointLights[3].specular", 1.0f, 1.0f, 1.0f);
    ourShader.setFloat("pointLights[3].constant", 1.0f);
    ourShader.setFloat("pointLights[3].linear", 0.09);
    ourShader.setFloat("pointLights[3].quadratic", 0.032);
    
    */
    Scene scene("C:\\Users\\parth\\source\\repos\\Psychspiration\\resources\\alleyway", ourShader);
    //Model* models = loadScene(scene);
    //std::vector<Model> modelz;
    
    Model* models{ new Model[scene.name.size()] };
    for (int i = 0; i < scene.name.size(); i++)
    {
        //load model
        //Model x(scene.scenePath + scene.name[i] + ".glb");
        //modelz.push_back(x);
        models[i].getpath(scene.scenePath + scene.name[i] + ".glb");
    }
    

    // render loop
    // -----------
    while (!glfwWindowShouldClose(window))
    {
        // per-frame time logic
        // --------------------
        float currentFrame = glfwGetTime();
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;

        // input
        // -----
        processInput(window);
        


        // render
        // ------
        glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        ourShader.use();
        // spotLight
        ourShader.setVec3("spotLight.position", camera.Position);
        ourShader.setVec3("spotLight.direction", camera.Front);
        ourShader.setVec3("pointLights[0].position", pointLightPositions[0]);
        // view/projection transformations
        glm::mat4 projection = glm::perspective(glm::radians(camera.Zoom), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 100.0f);
        glm::mat4 view = camera.GetViewMatrix();
        ourShader.setMat4("projection", projection);
        ourShader.setMat4("view", view);
        ourShader.setVec3("viewPos", camera.Position);
        
        // render the loaded model
        
        glm::mat4 model = glm::mat4(1.0f);
        model = glm::translate(model, glm::vec3(0.0f, 0.0f, 0.0f)); // translate it down so it's at the center of the scene
        model = glm::scale(model, scale*glm::vec3(1.0f, 1.0f, 1.0f));	// it's a bit too big for our scene, so scale it down
        ourShader.setMat4("model", model);
        axes.Draw(ourShader);
        
        drawScene(scene,ourShader,models);
        //std::cout << "0" << std::endl;
        /*
        //render the bulbs
        glm::mat4 model1 = glm::mat4(1.0f);
        lightCubeShader.use();
        lightCubeShader.setMat4("projection", projection);
        lightCubeShader.setMat4("view", view);
        for (unsigned int i = 0; i < 1; i++)
        {
            model1 = glm::mat4(1.0f);
            model1 = glm::translate(model1, pointLightPositions[i]);
            model1 = glm::scale(model1, glm::vec3(0.2f)); // Make it a smaller cube
            // model1 = model1 * world_trans_intitial
            lightCubeShader.setMat4("model", model1);
            bulb.Draw(lightCubeShader);
        }
        
        */

        // glfw: swap buffers and poll IO events (keys pressed/released, mouse moved etc.)
        // -------------------------------------------------------------------------------
        glfwSwapBuffers(window);
        glfwPollEvents();
    }



    // glfw: terminate, clearing all previously allocated GLFW resources.
    // ------------------------------------------------------------------
    glfwTerminate();
    return 0;
}

// process all input: query GLFW whether relevant keys are pressed/released this frame and react accordingly
// ---------------------------------------------------------------------------------------------------------
void processInput(GLFWwindow* window)
{
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);
    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
        camera.ProcessKeyboard(FORWARD, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
        camera.ProcessKeyboard(BACKWARD, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
        camera.ProcessKeyboard(LEFT, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
        camera.ProcessKeyboard(RIGHT, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_UP) == GLFW_PRESS)
        updatelightloc("forward", deltaTime);
    if (glfwGetKey(window, GLFW_KEY_DOWN) == GLFW_PRESS)
        updatelightloc("backward", deltaTime);
    if (glfwGetKey(window, GLFW_KEY_LEFT) == GLFW_PRESS)
        updatelightloc("left", deltaTime);
    if (glfwGetKey(window, GLFW_KEY_RIGHT) == GLFW_PRESS)
        updatelightloc("right", deltaTime);
    if (glfwGetKey(window, GLFW_KEY_Q) == GLFW_PRESS)
        updatelightloc("up", deltaTime);
    if (glfwGetKey(window, GLFW_KEY_E) == GLFW_PRESS)
        updatelightloc("down", deltaTime);

}

// glfw: whenever the window size changed (by OS or user resize) this callback function executes
// ---------------------------------------------------------------------------------------------
void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
    // make sure the viewport matches the new window dimensions; note that width and 
    // height will be significantly larger than specified on retina displays.
    glViewport(0, 0, width, height);
}


// glfw: whenever the mouse moves, this callback is called
// -------------------------------------------------------
void mouse_callback(GLFWwindow* window, double xpos, double ypos)
{
    if (firstMouse)
    {
        lastX = xpos;
        lastY = ypos;
        firstMouse = false;
    }

    float xoffset = xpos - lastX;
    float yoffset = lastY - ypos; // reversed since y-coordinates go from bottom to top

    lastX = xpos;
    lastY = ypos;

    camera.ProcessMouseMovement(xoffset, yoffset);
}

// glfw: whenever the mouse scroll wheel scrolls, this callback is called
// ----------------------------------------------------------------------
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
{
    camera.ProcessMouseScroll(yoffset);
}

void updatelightloc(std::string x, float deltaTime)
{
    float speed = 1;
    speed *= deltaTime;
    if (x == "forward")
    {
        pointLightPositions[0].z += speed;
    }
    if (x == "backward")
    {
        pointLightPositions[0].z -= speed;
    }
    if (x =="left")
    {
        pointLightPositions[0].x += speed;
    }
    if (x =="right")
    {
        pointLightPositions[0].x -= speed;
    }
    if (x == "up")
    {
        pointLightPositions[0].y += speed;
    }
    if (x == "down")
    {
        pointLightPositions[0].y -= speed;
    }

}
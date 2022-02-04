#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <func.h>



void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void mouse_callback(GLFWwindow* window, double xpos, double ypos);
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);
void processInput(GLFWwindow* window);
void setLights(Shader ourShader);
void renderSphere();
// settings
const unsigned int SCR_WIDTH = 1920;
const unsigned int SCR_HEIGHT = 1080;
bool shadows = true;
bool normals = true;
bool shadowsKeyPressed = false;
bool normalsKeyPressed = false;

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
        glm::vec3(-2.0f, 4.0f, -1.0f),
       glm::vec3(0.0f,  0.0f,  0.5f),
        glm::vec3(0.0f,  0.0f,  0.5f),
        glm::vec3(0.0f,  0.0f,  0.5f)
};

struct PointLight {
    glm::vec3 position;
    glm::vec3 color;
    float constant;
    float linear;
    float quadratic;
    glm::vec2 size;
};
struct DirectionalLight {
    glm::vec3 direction;
    glm::vec3 color;
};
struct SpotLight {
    glm::vec3 position;
    glm::vec3 direction;
    float angleInnerCone;
    float angleOuterCone;
    float constant;
    float linear;
    float quadratic;
    glm::vec3 color;
};


int main()
{
    std::cout << " using version " << aiGetVersionMajor << "." << aiGetVersionMinor << " Assimp. " << std::endl;
    // glfw: initialize and configure
    // ------------------------------
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    //glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_COMPAT_PROFILE);

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
    //glEnable(GL_CULL_FACE);
    glEnable(GL_DEPTH_CLAMP);
    glEnable(GL_MULTISAMPLE);
    glEnable(GL_FRAMEBUFFER_SRGB);
    //stbi_set_flip_vertically_on_load(true);
    // build and compile our shader zprogram
    // ------------------------------------
    Shader ourShader("C:\\Users\\parth\\source\\repos\\Psychspiration\\resources\\shaders\\vertex_invertex_.vs", "C:\\Users\\parth\\source\\repos\\Psychspiration\\resources\\shaders\\shadowbckup.fs",nullptr);
    Shader simpleDepthShader("C:\\Users\\parth\\source\\repos\\Psychspiration\\resources\\shaders\\pointshadow.vs","C:\\Users\\parth\\source\\repos\\Psychspiration\\resources\\shaders\\pointshadow.fs","C:\\Users\\parth\\source\\repos\\Psychspiration\\resources\\shaders\\pointshadow.gs");
    Shader lightCubeShader("C:\\Users\\parth\\source\\repos\\Psychspiration\\resources\\shaders\\vertex_lightcube.vs", "C:\\Users\\parth\\source\\repos\\Psychspiration\\resources\\shaders\\fragment_lightcube.fs", nullptr);
    Shader debugDepthQuad("C:\\Users\\parth\\source\\repos\\Psychspiration\\resources\\shaders\\debug.vs", "C:\\Users\\parth\\source\\repos\\Psychspiration\\resources\\shaders\\debug.fs", nullptr);
    Shader pbrShader("C:\\Users\\parth\\source\\repos\\Psychspiration\\resources\\shaders\\vertex_invertex_.vs", "C:\\Users\\parth\\source\\repos\\Psychspiration\\resources\\shaders\\pbr.fs", nullptr);
    float scale = 0.1;
    //Model bulb("C:\\Users\\parth\\source\\repos\\Psychspiration\\resources\\models\\light bulb\\bulb.obj");
    Model bulb("C:\\Users\\parth\\source\\repos\\Psychspiration\\resources\\bulb\\bulb1.glb");
    //Model axes("C:\\Users\\parth\\source\\repos\\Psychspiration\\resources\\axes\\axes.obj");
    setLights(pbrShader);
    const unsigned int SHADOW_WIDTH = 1920, SHADOW_HEIGHT = 1920;
    unsigned int depthMapFBO;
    glGenFramebuffers(1, &depthMapFBO);
    // create depth cubemap texture
    unsigned int depthCubemap;
    glGenTextures(1, &depthCubemap);
    glBindTexture(GL_TEXTURE_CUBE_MAP, depthCubemap);
    for (unsigned int i = 0; i < 6; i++)
        glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_DEPTH_COMPONENT, SHADOW_WIDTH, SHADOW_HEIGHT, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
    // attach depth texture as FBO's depth buffer
    glBindFramebuffer(GL_FRAMEBUFFER, depthMapFBO);
    glFramebufferTexture(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, depthCubemap, 0);
    glDrawBuffer(GL_NONE);
    glReadBuffer(GL_NONE);
    glBindFramebuffer(GL_FRAMEBUFFER, 0);

    void setLights(Shader pbrShader);
     
    Scene scene("C:\\Users\\parth\\source\\repos\\Psychspiration\\resources\\sponza", ourShader);
    int nrLights;
    Model* models{ new Model[scene.name.size()] };
    bool gotLights = 0;
    for (int i = 0; i < scene.name.size(); i++)
    {
        std::cout << "Object in scene: " << scene.name[i]<< std::endl;
        const aiScene* assimpScene = models[i].getpath(scene.scenePath + scene.name[i] + ".glb");
        
        if (!gotLights)
        {
            std::cout << "Scene has lights : " << assimpScene->HasLights();
            if (assimpScene->HasLights())
            {
               
                std::vector <PointLight> pointLights;
                std::vector <DirectionalLight> directionalLights;
                std::vector <SpotLight> spotLights;
                nrLights = assimpScene->mNumLights;
                std::cout <<"number of lights" << nrLights;
                int nrPointLights=0;
                int nrDirectionalLights=0;
                int nrSpotLights=0;
                aiLight** light = assimpScene->mLights;
                for (int i = 0; i < nrLights; i++)
                {
                    if(light[i]->mType== aiLightSource_POINT)
                    {
                        PointLight pointLight;
                        pointLight.position.x = light[i]->mPosition.x;
                        pointLight.position.y = light[i]->mPosition.y;
                        pointLight.position.z = light[i]->mPosition.z;
                        pointLight.color.x = light[i]->mColorDiffuse.r;
                        pointLight.color.y = light[i]->mColorDiffuse.g;
                        pointLight.color.z = light[i]->mColorDiffuse.b;
                        pointLight.constant = light[i]->mAttenuationConstant;
                        pointLight.linear = light[i]->mAttenuationLinear;
                        pointLight.quadratic = light[i]->mAttenuationQuadratic;
                        pointLight.size.x = light[i]->mSize.x;
                        pointLight.size.y = light[i]->mSize.y;
                        pointLights.push_back(pointLight);
                        nrPointLights++;
                    }
                    if (light[i]->mType == aiLightSource_DIRECTIONAL)
                    {   
                        DirectionalLight directionalLight;
                        directionalLight.color.x = light[i]->mColorDiffuse.r;
                        directionalLight.color.y = light[i]->mColorDiffuse.g;
                        directionalLight.color.z = light[i]->mColorDiffuse.b;
                        directionalLight.direction.x = light[i]->mDirection.x;
                        directionalLight.direction.y = light[i]->mDirection.y;
                        directionalLight.direction.z = light[i]->mDirection.z;
                        directionalLights.push_back(directionalLight);
                        nrDirectionalLights++;
                    }
                    if (light[i]->mType == aiLightSource_SPOT)
                    {
                        SpotLight spotLight;
                        spotLight.color.x = light[i]->mColorDiffuse.r;
                        spotLight.color.y = light[i]->mColorDiffuse.g;
                        spotLight.color.z = light[i]->mColorDiffuse.b;
                        spotLight.position.x = light[i]->mPosition.x;
                        spotLight.position.y = light[i]->mPosition.y;
                        spotLight.position.z = light[i]->mPosition.z;
                        spotLight.constant = light[i]->mAttenuationConstant;
                        spotLight.linear = light[i]->mAttenuationLinear;
                        spotLight.quadratic = light[i]->mAttenuationQuadratic;
                        spotLight.angleInnerCone = light[i]->mAngleInnerCone;
                        spotLight.angleOuterCone = light[i]->mAngleOuterCone;
                        spotLights.push_back(spotLight);
                        nrSpotLights++;
                    }
                }

                gotLights = 1;
            }
            
        }
            
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
        glm::vec3 lightPos = pointLightPositions[0];
        // render
        // ------
        glClearColor(0,0,0, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        
        // -----------------------------------------------
        float near_plane = 1.0f;
        float far_plane = 25.0f;
        glm::mat4 shadowProj = glm::perspective(glm::radians(90.0f), (float)SHADOW_WIDTH / (float)SHADOW_HEIGHT, near_plane, far_plane);
        std::vector<glm::mat4> shadowTransforms;
        shadowTransforms.push_back(shadowProj * glm::lookAt(lightPos, lightPos + glm::vec3(1.0f, 0.0f, 0.0f), glm::vec3(0.0f, -1.0f, 0.0f)));
        shadowTransforms.push_back(shadowProj * glm::lookAt(lightPos, lightPos + glm::vec3(-1.0f, 0.0f, 0.0f), glm::vec3(0.0f, -1.0f, 0.0f)));
        shadowTransforms.push_back(shadowProj * glm::lookAt(lightPos, lightPos + glm::vec3(0.0f, 1.0f, 0.0f), glm::vec3(0.0f, 0.0f, 1.0f)));
        shadowTransforms.push_back(shadowProj * glm::lookAt(lightPos, lightPos + glm::vec3(0.0f, -1.0f, 0.0f), glm::vec3(0.0f, 0.0f, -1.0f)));
        shadowTransforms.push_back(shadowProj * glm::lookAt(lightPos, lightPos + glm::vec3(0.0f, 0.0f, 1.0f), glm::vec3(0.0f, -1.0f, 0.0f)));
        shadowTransforms.push_back(shadowProj * glm::lookAt(lightPos, lightPos + glm::vec3(0.0f, 0.0f, -1.0f), glm::vec3(0.0f, -1.0f, 0.0f)));

        // 1. render scene to depth cubemap
       // --------------------------------
        glViewport(0, 0, SHADOW_WIDTH, SHADOW_HEIGHT);
        glBindFramebuffer(GL_FRAMEBUFFER, depthMapFBO);
        glClear(GL_DEPTH_BUFFER_BIT);
        simpleDepthShader.use();
        for (unsigned int i = 0; i < 6; ++i)
            simpleDepthShader.setMat4("shadowMatrices[" + std::to_string(i) + "]", shadowTransforms[i]);
        simpleDepthShader.setFloat("far_plane", far_plane);
        simpleDepthShader.setVec3("lightPos", lightPos);

        glm::mat4 model = glm::mat4(1.0f);
        model = glm::translate(model, glm::vec3(0.0f, 0.0f, 0.0f));
        model = glm::scale(model, scale*glm::vec3(1.0f, 1.0f, 1.0f));
        simpleDepthShader.setMat4("model", model); 
        //axes.Draw(simpleDepthShader);
        drawScene(scene,simpleDepthShader,models);
        // render rows*column number of spheres with varying metallic/roughness values scaled by rows and columns respectively
        model = glm::mat4(1.0f);
        int nrRows = 5;
        int nrColumns = 5;
        float spacing = 1.0f;
        glBindFramebuffer(GL_FRAMEBUFFER, 0);

        // 2. render scene as normal 
        // -------------------------
        glViewport(0, 0, SCR_WIDTH, SCR_HEIGHT);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        pbrShader.use();
        glm::mat4 projection = glm::perspective(glm::radians(camera.Zoom), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 100.0f);
        glm::mat4 view = camera.GetViewMatrix();
        pbrShader.setMat4("projection", projection);
        pbrShader.setMat4("view", view);
        pbrShader.setVec3("viewPos", camera.Position);
        // set lighting uniforms
        
        pbrShader.setVec3("spotLight.position", camera.Position);
        pbrShader.setVec3("spotLight.direction", camera.Front);
        pbrShader.setVec3("pointLights[0].position", pointLightPositions[0]);
        pbrShader.setInt("doshadows", shadows); // enable/disable shadows by pressing 'SPACE'
        pbrShader.setInt("donormals", normals);
        pbrShader.setFloat("far_plane", far_plane);
        // spotLight
        pbrShader.setVec3("spotLight.position", camera.Position);
    /*  glActiveTexture(GL_TEXTURE1);
        glBindTexture(GL_TEXTURE_CUBE_MAP, depthCubemap);*/
        const GLenum skyboxTexture = GL_TEXTURE11;
        glActiveTexture(skyboxTexture);
        glBindTexture(GL_TEXTURE_CUBE_MAP, depthCubemap);    
        pbrShader.setMat4("model", model);
        //axes.Draw(ourShader);

        drawScene(scene, pbrShader, models);

        //render the bulbs

        glm::mat4 model1 = glm::mat4(1.0f);
        lightCubeShader.use();
        lightCubeShader.setMat4("projection", projection);
        lightCubeShader.setMat4("view", view);
        for (unsigned int i = 0; i < 1; i++)
        {
            model1 = glm::mat4(1.0f);
            model1 = glm::translate(model1, pointLightPositions[i]);
            model1 = glm::scale(model1, glm::vec3(0.08f)); // Make it a smaller cube
            // model1 = model1 * world_trans_intitial
            lightCubeShader.setMat4("model", model1);
            bulb.Draw(lightCubeShader);
        }

       
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
    if (glfwGetKey(window, GLFW_KEY_1) == GLFW_PRESS && !shadowsKeyPressed)
    {
        shadows = !shadows;
        shadowsKeyPressed = true;
    }
    if (glfwGetKey(window, GLFW_KEY_1) == GLFW_RELEASE)
    {
        shadowsKeyPressed = false;
    }
    if (glfwGetKey(window, GLFW_KEY_2) == GLFW_PRESS && !normalsKeyPressed)
    {
        normals = !normals;
        normalsKeyPressed = true;
    }
    if (glfwGetKey(window, GLFW_KEY_2) == GLFW_RELEASE)
    {
        normalsKeyPressed = false;
    }
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
// renders (and builds at first invocation) a sphere
// -------------------------------------------------
unsigned int sphereVAO = 0;
unsigned int indexCount;
void renderSphere()
{
    if (sphereVAO == 0)
    {
        glGenVertexArrays(1, &sphereVAO);

        unsigned int vbo, ebo;
        glGenBuffers(1, &vbo);
        glGenBuffers(1, &ebo);

        std::vector<glm::vec3> positions;
        std::vector<glm::vec2> uv;
        std::vector<glm::vec3> normals;
        std::vector<unsigned int> indices;

        const unsigned int X_SEGMENTS = 64;
        const unsigned int Y_SEGMENTS = 64;
        const float PI = 3.14159265359;
        for (unsigned int x = 0; x <= X_SEGMENTS; ++x)
        {
            for (unsigned int y = 0; y <= Y_SEGMENTS; ++y)
            {
                float xSegment = (float)x / (float)X_SEGMENTS;
                float ySegment = (float)y / (float)Y_SEGMENTS;
                float xPos = std::cos(xSegment * 2.0f * PI) * std::sin(ySegment * PI);
                float yPos = std::cos(ySegment * PI);
                float zPos = std::sin(xSegment * 2.0f * PI) * std::sin(ySegment * PI);

                positions.push_back(glm::vec3(xPos, yPos, zPos));
                uv.push_back(glm::vec2(xSegment, ySegment));
                normals.push_back(glm::vec3(xPos, yPos, zPos));
            }
        }

        bool oddRow = false;
        for (unsigned int y = 0; y < Y_SEGMENTS; ++y)
        {
            if (!oddRow) // even rows: y == 0, y == 2; and so on
            {
                for (unsigned int x = 0; x <= X_SEGMENTS; ++x)
                {
                    indices.push_back(y * (X_SEGMENTS + 1) + x);
                    indices.push_back((y + 1) * (X_SEGMENTS + 1) + x);
                }
            }
            else
            {
                for (int x = X_SEGMENTS; x >= 0; --x)
                {
                    indices.push_back((y + 1) * (X_SEGMENTS + 1) + x);
                    indices.push_back(y * (X_SEGMENTS + 1) + x);
                }
            }
            oddRow = !oddRow;
        }
        indexCount = indices.size();

        std::vector<float> data;
        for (unsigned int i = 0; i < positions.size(); ++i)
        {
            data.push_back(positions[i].x);
            data.push_back(positions[i].y);
            data.push_back(positions[i].z);
            if (normals.size() > 0)
            {
                data.push_back(normals[i].x);
                data.push_back(normals[i].y);
                data.push_back(normals[i].z);
            }
            if (uv.size() > 0)
            {
                data.push_back(uv[i].x);
                data.push_back(uv[i].y);
            }
        }
        glBindVertexArray(sphereVAO);
        glBindBuffer(GL_ARRAY_BUFFER, vbo);
        glBufferData(GL_ARRAY_BUFFER, data.size() * sizeof(float), &data[0], GL_STATIC_DRAW);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int), &indices[0], GL_STATIC_DRAW);
        unsigned int stride = (3 + 2 + 3) * sizeof(float);
        glEnableVertexAttribArray(0);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, stride, (void*)0);
        glEnableVertexAttribArray(1);
        glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, stride, (void*)(3 * sizeof(float)));
        glEnableVertexAttribArray(2);
        glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, stride, (void*)(6 * sizeof(float)));
    }

    glBindVertexArray(sphereVAO);
    glDrawElements(GL_TRIANGLE_STRIP, indexCount, GL_UNSIGNED_INT, 0);
}
void setLights(Shader ourShader)
{
    // be sure to activate shader when setting uniforms/drawing objects
    ourShader.use();
    ourShader.setInt("depthMap", 11);
    // point light 1
    ourShader.setVec3("pointLights[0].position", pointLightPositions[0]);

    ourShader.setVec3("pointLights[0].color", 23.47, 21.31, 20.79);
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

}
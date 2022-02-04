#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <func.h>
#include<stb_image.h>
#include <stb_image_write.h>
#include <Settings_importer.h>

void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void mouse_callback(GLFWwindow* window, double xpos, double ypos);
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);
void processInput(GLFWwindow* window);
void setLights(Shader ourShader);
void renderSphere();
void renderQuad();

// settings
//const unsigned int SCR_WIDTH = 1920;
//const unsigned int SCR_HEIGHT = 1080;
Settings* User1 = new Settings();
bool shadows = true;
bool normals = true;
bool forward = true;
bool shadowsKeyPressed = false;
bool normalsKeyPressed = false;
bool forwardKeyPressed = false;

// camera
Camera camera(glm::vec3(0.0f, 0.0f, 3.0f));
float lastX = User1->SCR_WIDTH / 2.0f;
float lastY = User1->SCR_HEIGHT / 2.0f;
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
    //GLFWwindow* window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "LearnOpenGL", glfwGetPrimaryMonitor(), NULL);
    GLFWwindow* window = glfwCreateWindow(User1->SCR_WIDTH, User1->SCR_HEIGHT, "LearnOpenGL", 0, NULL);
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
    //glEnable(GL_DEPTH_CLAMP);
    //glEnable(GL_MULTISAMPLE);
    //glEnable(GL_FRAMEBUFFER_SRGB);
    //stbi_set_flip_vertically_on_load(true);
    // build and compile our shader zprogram
    // ------------------------------------
    Shader::shaderPath = User1->shaderPath;
    
    Shader ourShader("vertex_invertex_.vs","shadowbckup.fs","0");
    Shader simpleDepthShader("pointshadow.vs","pointshadow.fs","pointshadow.gs");
    Shader lightCubeShader("vertex_lightcube.vs","fragment_lightcube.fs", "0");
    Shader debugDepthQuad("debug.vs", "debug.fs", "0");
    Shader pbrShader("vertex_invertex_.vs", "pbr.fs", "0");
    Shader defGeometryPass("Gpass.vs", "Gpass.fs","0");
    Shader defLightingPass("Lpass_pbr.vs", "Lpass_pbr.fs", "0");
    float scale = 0.1;
    //Model bulb("C:\\Users\\parth\\source\\repos\\Psychspiration\\resources\\models\\light bulb\\bulb.obj");
    Model bulb("C:\\Users\\parth\\source\\repos\\Psychspiration\\resources\\bulb\\bulb1.glb");
    Model axes("C:\\Users\\parth\\source\\repos\\Psychspiration\\resources\\models\\helmet_with_lights.glb");

    setLights(defLightingPass);
    setLights(pbrShader);


    unsigned int gBuffer;
    glGenFramebuffers(1, &gBuffer);
    glBindFramebuffer(GL_FRAMEBUFFER, gBuffer);
    unsigned int gPosition, gNormal, gAlbedo, gRoughMetal;

    // - position color buffer
    glGenTextures(1, &gPosition);
    glBindTexture(GL_TEXTURE_2D, gPosition);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, User1->SCR_WIDTH, User1->SCR_HEIGHT, 0, GL_RGBA, GL_FLOAT, NULL);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, gPosition, 0);

    // - normal color buffer
    glGenTextures(1, &gNormal);
    glBindTexture(GL_TEXTURE_2D, gNormal);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, User1->SCR_WIDTH, User1->SCR_HEIGHT, 0, GL_RGBA, GL_FLOAT, NULL);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT1, GL_TEXTURE_2D, gNormal, 0);

    // - albedo color buffer
    glGenTextures(1, &gAlbedo);
    glBindTexture(GL_TEXTURE_2D, gAlbedo);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, User1->SCR_WIDTH, User1->SCR_HEIGHT, 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT2, GL_TEXTURE_2D, gAlbedo, 0);

    // - roughmetal color buffer
    glGenTextures(1, &gRoughMetal);
    glBindTexture(GL_TEXTURE_2D, gRoughMetal);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, User1->SCR_WIDTH, User1->SCR_HEIGHT, 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT3, GL_TEXTURE_2D, gRoughMetal, 0);

    // - tell OpenGL which color attachments we'll use (of this framebuffer) for rendering 
    unsigned int attachments[4] = { GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1, GL_COLOR_ATTACHMENT2,GL_COLOR_ATTACHMENT3 };
    glDrawBuffers(4, attachments);



    // create and attach depth buffer (renderbuffer)
    unsigned int rboDepth;
    glGenRenderbuffers(1, &rboDepth);
    glBindRenderbuffer(GL_RENDERBUFFER, rboDepth);
    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT, User1->SCR_WIDTH, User1->SCR_HEIGHT);
    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, rboDepth);
    // finally check if framebuffer is complete
    if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
        std::cout << "Framebuffer not complete!" << std::endl;
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
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
    defLightingPass.use();
    defLightingPass.setInt("gPosition", 0);
    defLightingPass.setInt("gNormal", 1);
    defLightingPass.setInt("gAlbedo", 2);
    defLightingPass.setInt("gRoughMetal", 3);
    
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


        
        // 2. Geometry Pass
        // -------------------------

        glBindFramebuffer(GL_FRAMEBUFFER, gBuffer);
            glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
            defGeometryPass.use();
            glm::mat4 model = glm::mat4(1.0f);
            glm::mat4 projection = glm::perspective(glm::radians(camera.Zoom), (float)User1->SCR_WIDTH / (float)User1->SCR_HEIGHT, 0.1f, 100.0f);
            glm::mat4 view = camera.GetViewMatrix();
            defGeometryPass.setMat4("projection", projection);
            defGeometryPass.setMat4("view", view);
            defGeometryPass.setInt("donormals", normals);
            drawScene(scene, defGeometryPass, models);
            defGeometryPass.setMat4("model", model);
            defGeometryPass.setBool("existnormals", 1);
            //axes.Draw(defGeometryPass);
        glBindFramebuffer(GL_FRAMEBUFFER, 0);

        // 2. Lighting pass: 
        // ------------------------
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        defLightingPass.use();
        defLightingPass.setVec3("viewPos", camera.Position);
        defLightingPass.setVec3("spotLight.position", camera.Position);
        defLightingPass.setVec3("spotLight.direction", camera.Front);
        defLightingPass.setVec3("pointLights[0].position", pointLightPositions[0]);
        defLightingPass.setInt("doshadows", shadows); // enable/disable shadows by pressing '1'
        defLightingPass.setInt("donormals", normals); // enable/disable normals by pressing '2'

        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, gPosition);
        glActiveTexture(GL_TEXTURE0+1);
        glBindTexture(GL_TEXTURE_2D, gNormal);
        glActiveTexture(GL_TEXTURE0+2);
        glBindTexture(GL_TEXTURE_2D, gAlbedo);
        glActiveTexture(GL_TEXTURE0+3);
        glBindTexture(GL_TEXTURE_2D, gRoughMetal);
        //drawScene(scene, defLightingPass, models);
        renderQuad();
   
        // end of deffered shading
        glBindFramebuffer(GL_READ_FRAMEBUFFER, gBuffer);
        glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0); // write to default framebuffer
        glBlitFramebuffer(
            0, 0, User1->SCR_WIDTH, User1->SCR_HEIGHT, 0, 0, User1->SCR_WIDTH, User1->SCR_HEIGHT, GL_DEPTH_BUFFER_BIT, GL_NEAREST
        );
        glBindFramebuffer(GL_FRAMEBUFFER, 0);
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
    if (glfwGetKey(window, GLFW_KEY_0) == GLFW_PRESS && !forwardKeyPressed)
    {
        forward = !forward;
        forwardKeyPressed = true;
    }
    if (glfwGetKey(window, GLFW_KEY_0) == GLFW_RELEASE)
    {
        forwardKeyPressed = false;
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

// renderQuad() renders a 1x1 XY quad in NDC
// -----------------------------------------
unsigned int quadVAO = 0;
unsigned int quadVBO;
void renderQuad()
{
    if (quadVAO == 0)
    {
        float quadVertices[] = {
            // positions        // texture Coords
            -1.0f,  1.0f, 0.0f, 0.0f, 1.0f,
            -1.0f, -1.0f, 0.0f, 0.0f, 0.0f,
             1.0f,  1.0f, 0.0f, 1.0f, 1.0f,
             1.0f, -1.0f, 0.0f, 1.0f, 0.0f,
        };
        // setup plane VAO
        glGenVertexArrays(1, &quadVAO);
        glGenBuffers(1, &quadVBO);
        glBindVertexArray(quadVAO);
        glBindBuffer(GL_ARRAY_BUFFER, quadVBO);
        glBufferData(GL_ARRAY_BUFFER, sizeof(quadVertices), &quadVertices, GL_STATIC_DRAW);
        glEnableVertexAttribArray(0);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
        glEnableVertexAttribArray(1);
        glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));
    }
    glBindVertexArray(quadVAO);
    glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
    glBindVertexArray(0);
}
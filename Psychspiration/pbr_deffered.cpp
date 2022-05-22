#pragma once
#include <glad/glad.h>
#include <Window.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <EventHandler.h>
#include <FileIO.h>
#include <Scene.h>
#include <Settings.h>
#include <Shader.h>
#include <Camera.h>
#include <FileIO.h>
#include <Mesh.h>
#include <Model.h>
#include <ModelManager.h>
#include <Player.h>
#include <func.h>
#include <TimerQueryAsync.h>
#include <stb_image.h>
#include <stb_image_write.h>
#include <functional>
#include <iostream>
#include <glm/gtx/string_cast.hpp>
#include <chrono>
#include <thread>
void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void mouse_callback(GLFWwindow* window, double xpos, double ypos);
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);
void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods);
void processInput(GLFWwindow* window);
void processHoldKeys(GLFWwindow* window);
//void setLights(Shader ourShader);
//void setLights(Scene scene);
//void asyncLoad(Scene* scene);
void renderSphere();
void renderQuad();

//Engine classes
EventHandler* eventHandler = new EventHandler();

// settings
Settings User1(eventHandler);
bool shadows = true; //toggle
bool normals = true; //toggle
bool forward = true;
bool shadowsKeyPressed = false;
bool normalsKeyPressed = false;
bool forwardKeyPressed = false;
bool play = false;
// camera
Camera camera(eventHandler,glm::vec3(0.0f, 0.0f, 3.0f));
float lastX = User1.SCR_WIDTH / 2.0f;
float lastY = User1.SCR_HEIGHT / 2.0f;
bool firstMouse = true;
//Physics

// timing
//float deltaTime = 0.0f;
float lastFrame = 0.0f;
void updatelightloc(std::string x, float deltaTime);

// lighting
//std::vector<PointLight> light;
//unsigned int numLights;
unsigned int maxLights{ 100 };
// maxLights = max(scene[i].numLights);
//Scene* activeScene;
//struct GPULight* lights;

//Player* player = new Player(eventHandler);
int main()
{
    // glfw: initialize and configure
    // ------------------------------
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_SAMPLES, 4);
    //glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_COMPAT_PROFILE);

#ifdef __APPLE__
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif

    // glfw window creation
    // --------------------
    int* count= new int;
    //GLFWwindow* window = glfwCreateWindow(User1.SCR_WIDTH, User1.SCR_HEIGHT, "Psychspiration", (glfwGetMonitors(count))[0], NULL);
    GLFWwindow* window = glfwCreateWindow(User1.SCR_WIDTH, User1.SCR_HEIGHT, "Psychspiration", 0, NULL);
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
    glfwSetKeyCallback(window,key_callback);
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
    glEnable(GL_MULTISAMPLE);
    glEnable(GL_FRAMEBUFFER_SRGB);
    //stbi_set_flip_vertically_on_load(true);
    
    ModelManager* modelManager;
    Physics* physics;
    
    modelManager = new ModelManager();
    physics = new Physics();

    // build and compile our shader zprogram
    // ------------------------------------
    //Shader matcapShader("matcap.vs", "matcap.fs", "0");
    Shader wireShader("model_simple.vs", "model_simple.fs", "0");
    Shader lightCubeShader("vertex_lightcube.vs", "fragment_lightcube.fs", "0");
    Shader pbrShader("pbr.vs", "pbr.fs", "0");
    Shader pbrShader_instanced("pbr_instanced.vs", "pbr_instanced.fs", "0");
    Shader hdrShader("quad.vs", "hdr.fs","0");
    Shader simpleDepthShader("pointshadow.vs", "pointshadow - Copy.fs", "pointshadow - Copy.gs");
    Shader simpleDepthShader_instanced("pointshadow_instanced.vs", "pointshadow - Copy.fs", "pointshadow - Copy.gs");
    Shader dirShadow_instanced("simpleDepthShader_instanced.vs", "emptyfrag.fs", "0");
    //setLights(pbrShader);
    Model bulb("resource\\bulb\\bulb2.glb");

    TimerQueryAsync timer(5);
    
    Scene scene(User1.resourcePath,physics,eventHandler,modelManager);

    scene.loadObjects();
    scene.makeHAB();

    //Player* player = new Player(new Object((std::string)("table"), new Model("resource\\newDDSexporter\\node_damagedHelmet_-6514.gltf"), glm::mat4(1.0f)), eventHandler);
    //scene.objects.push_back(player->obj);

    GLuint uboModelMatrices;
    glGenBuffers(1, &uboModelMatrices);
    glBindBuffer(GL_SHADER_STORAGE_BUFFER, uboModelMatrices);
    glBufferData(GL_SHADER_STORAGE_BUFFER, sizeof(glm::mat4) * scene.objects.size(), scene.instancedTransforms, GL_STATIC_DRAW);
    glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 1, uboModelMatrices);
    glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);

    //lights are stored in ubo // might increase performance compared to ssbo, also no need to change light attributes in shader
    
    unsigned int lightUBO;
    glGenBuffers(1, &lightUBO);
    glBindBuffer(GL_UNIFORM_BUFFER, lightUBO);
    glBufferData(GL_UNIFORM_BUFFER, maxLights * sizeof(GPULight), NULL, GL_DYNAMIC_DRAW);
    GLint bufMask = GL_READ_WRITE;
    GPULight* lights_buffer;
    lights_buffer = (struct GPULight*)glMapBuffer(GL_UNIFORM_BUFFER, bufMask);

    for (unsigned int i = 0; i < scene.numLights; ++i) {
        //Fetching the light from the current scene
        lights_buffer[i].position = glm::vec4(scene.lightList[i].position, 1.0f);
        lights_buffer[i].color = glm::vec4(scene.lightList[i].color, 1.0f);
        lights_buffer[i].enabled = 1;
        lights_buffer[i].intensity = scene.lightList[i].power;
        lights_buffer[i].range = scene.lightList[i].size;
        //std::cout << i << "th light ::" << std::endl <<"\tposition: " << lights[i].position.x << " " << lights[i].position.y << " " << lights[i].position.z<<std::endl;
    }
    glUnmapBuffer(GL_UNIFORM_BUFFER);
    glBindBufferBase(GL_UNIFORM_BUFFER, 3, lightUBO);
    glBindBuffer(GL_UNIFORM_BUFFER, 0);

    // point light shadow
    // create depth cubemap texture 
    //const unsigned int SHADOW_WIDTH = 1024, SHADOW_HEIGHT = 1024;
    const unsigned int SHADOW_MAP_MAX_SIZE = 1024;
    unsigned int depthCubemap;
    glGenTextures(1, &depthCubemap);
    glBindTexture(GL_TEXTURE_CUBE_MAP_ARRAY, depthCubemap);
    glTexImage3D(GL_TEXTURE_CUBE_MAP_ARRAY, 0, GL_DEPTH_COMPONENT, SHADOW_MAP_MAX_SIZE, SHADOW_MAP_MAX_SIZE, 6 * scene.numLights, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
    
    //glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_DEPTH_COMPONENT, SHADOW_WIDTH, SHADOW_HEIGHT, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);

    glTexParameterf(GL_TEXTURE_CUBE_MAP_ARRAY, GL_TEXTURE_MIN_FILTER, GL_NEAREST); //linear
    glTexParameterf(GL_TEXTURE_CUBE_MAP_ARRAY, GL_TEXTURE_MAG_FILTER, GL_NEAREST); //linear

    glTexParameteri(GL_TEXTURE_CUBE_MAP_ARRAY, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP_ARRAY, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP_ARRAY, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
    glTexParameterf(GL_TEXTURE_CUBE_MAP_ARRAY, GL_TEXTURE_BASE_LEVEL, 0);
    glTexParameterf(GL_TEXTURE_CUBE_MAP_ARRAY, GL_TEXTURE_MAX_LEVEL, 0);
    pbrShader.use();
    pbrShader.setInt("depthMap", 11);
    pbrShader_instanced.use();
    pbrShader_instanced.setInt("depthMap", 11);
    // attach depth texture as FBO's depth buffer
    unsigned int depthMapFBO;
    glGenFramebuffers(1, &depthMapFBO);
    glBindFramebuffer(GL_FRAMEBUFFER, depthMapFBO);
    glFramebufferTexture(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, depthCubemap, 0);
    /* for (int layer = 0; layer < numLights; layer++)
     {
         glFramebufferTextureLayer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, depthCubemap, 0, layer);
     }*/
     //glFramebufferTextureLayer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, depthCubemap, 0, 6*numLights);
    glDrawBuffer(GL_NONE);
    glReadBuffer(GL_NONE);
    glBindFramebuffer(GL_FRAMEBUFFER, 0);

    // directional light shadow
    const unsigned int SHADOW_MAP_MAX_SIZE_DIR = 4096;
    unsigned int depthMap;
    glGenTextures(1,&depthMap);
    glBindTexture(GL_TEXTURE_2D, depthMap);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, SHADOW_MAP_MAX_SIZE_DIR, SHADOW_MAP_MAX_SIZE_DIR, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
    float borderColor[] = { 1.0, 1.0, 1.0, 1.0 };
    glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, borderColor);
    pbrShader.use();
    pbrShader.setInt("depthMap", 11);
    pbrShader_instanced.use();
    pbrShader_instanced.setInt("depthMap_dir",12);

    // attach depth texture as FBO's depth buffer
    unsigned int depthMapFBO_dir;
    glGenFramebuffers(1, &depthMapFBO_dir);
    glBindFramebuffer(GL_FRAMEBUFFER, depthMapFBO_dir);
    glFramebufferTexture(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, depthMap, 0);
    glDrawBuffer(GL_NONE);
    glReadBuffer(GL_NONE);
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    
    // configure floating point framebuffer
    // ------------------------------------
    unsigned int postFBO;
    glGenFramebuffers(1, &postFBO);
    glBindFramebuffer(GL_FRAMEBUFFER, postFBO);
    // create floating point color buffer with hdr 16 bit
    unsigned int textureColorBufferMultiSampled;
    glGenTextures(1, &textureColorBufferMultiSampled);
    glBindTexture(GL_TEXTURE_2D_MULTISAMPLE, textureColorBufferMultiSampled);
    glTexImage2DMultisample(GL_TEXTURE_2D_MULTISAMPLE, 4, GL_RGBA16F, User1.SCR_WIDTH, User1.SCR_HEIGHT, GL_TRUE);
    glBindTexture(GL_TEXTURE_2D_MULTISAMPLE, 0);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D_MULTISAMPLE, textureColorBufferMultiSampled, 0);
    // create a (also multisampled) renderbuffer object for depth and stencil attachments
    unsigned int rbo;
    glGenRenderbuffers(1, &rbo);
    glBindRenderbuffer(GL_RENDERBUFFER, rbo);
    glRenderbufferStorageMultisample(GL_RENDERBUFFER, 4, GL_DEPTH24_STENCIL8, User1.SCR_WIDTH, User1.SCR_HEIGHT);
    glBindRenderbuffer(GL_RENDERBUFFER, 0);
    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, rbo);

    if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
        std::cout << "ERROR::FRAMEBUFFER:: Framebuffer is not complete!" << std::endl;
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    // configure second post-processing framebuffer
    unsigned int intermediateFBO;
    glGenFramebuffers(1, &intermediateFBO);
    glBindFramebuffer(GL_FRAMEBUFFER, intermediateFBO);
    // create a color attachment texture
    unsigned int screenTexture;
    glGenTextures(1, &screenTexture);
    glBindTexture(GL_TEXTURE_2D, screenTexture);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, User1.SCR_WIDTH, User1.SCR_HEIGHT, 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, screenTexture, 0);	// we only need a color buffer

    if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
        std::cout << "ERROR::FRAMEBUFFER:: Intermediate framebuffer is not complete!" << std::endl;
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    //setup point light shadow shader
    float near_plane = 0.0f;
    float far_plane = 100.0f;
    glm::mat4 shadowProj = glm::perspective(glm::radians(90.0f), 1.f/*aspect ratio*/, near_plane, far_plane);
    std::vector<glm::mat4> shadowTransforms;
    for (unsigned int i = 0; i < scene.numLights; i++)
    {
        shadowTransforms.push_back(shadowProj * glm::lookAt(scene.lightList[i].position, scene.lightList[i].position + glm::vec3(1.0f, 0.0f, 0.0f), glm::vec3(0.0f, -1.0f, 0.0f)));
        shadowTransforms.push_back(shadowProj * glm::lookAt(scene.lightList[i].position, scene.lightList[i].position + glm::vec3(-1.0f, 0.0f, 0.0f), glm::vec3(0.0f, -1.0f, 0.0f)));
        shadowTransforms.push_back(shadowProj * glm::lookAt(scene.lightList[i].position, scene.lightList[i].position + glm::vec3(0.0f, 1.0f, 0.0f), glm::vec3(0.0f, 0.0f, 1.0f)));
        shadowTransforms.push_back(shadowProj * glm::lookAt(scene.lightList[i].position, scene.lightList[i].position + glm::vec3(0.0f, -1.0f, 0.0f), glm::vec3(0.0f, 0.0f, -1.0f)));
        shadowTransforms.push_back(shadowProj * glm::lookAt(scene.lightList[i].position, scene.lightList[i].position + glm::vec3(0.0f, 0.0f, 1.0f), glm::vec3(0.0f, -1.0f, 0.0f)));
        shadowTransforms.push_back(shadowProj * glm::lookAt(scene.lightList[i].position, scene.lightList[i].position + glm::vec3(0.0f, 0.0f, -1.0f), glm::vec3(0.0f, -1.0f, 0.0f)));
    }
    simpleDepthShader_instanced.use();
    for (unsigned int i = 0; i < 6 * scene.numLights; ++i)
        simpleDepthShader_instanced.setMat4("shadowMatrices[" + std::to_string(i) + "]", shadowTransforms[i]);
    simpleDepthShader_instanced.setFloat("far_plane", far_plane);
    simpleDepthShader_instanced.setInt("numLights", scene.numLights);
    // setup directional light shadow shader 
     // no setup required ? wait lets feed it a dir light
    DirectionalLight dlight{};
    glm::mat4 lightProjection = glm::ortho(-100.0f, 100.00f, -100.0f, 100.0f, near_plane, far_plane);
    glm::mat4 lightView = glm::lookAt(dlight.direction, glm::vec3(0.0f), glm::vec3(0.0, 1.0, 0.0));
    glm::mat4 lightSpaceMatrix = lightProjection * lightView;
    dirShadow_instanced.use();
    dirShadow_instanced.setMat4("lightSpaceMatrix", lightSpaceMatrix);
    pbrShader_instanced.use();
    pbrShader_instanced.setVec3("dir_direction", dlight.direction);
    pbrShader_instanced.setVec3("dir_color", dlight.color);
    pbrShader_instanced.setFloat("dir_intensity", 100);
    pbrShader_instanced.setMat4("lightSpaceMatrix", lightSpaceMatrix);
    //eventHandler->registerCallback("Hello", &Scene::loadObject , &scene);
    //player->setUpEvents();
    // render loop
    // -----------
    bool shadowUpdate = true;
    while (!glfwWindowShouldClose(window))
    {
        // per-frame time logic
        // --------------------
        if (play)
        {
            //load helmet async
            std::cout << "before thread";
            //std::thread t1(&Scene::loadObject,scene);
           // t1.join();
            //scene.loadObject();
            //Object* helmet = new Object((std::string)("table"), new Model("resource\\newDDSexporter\\node_damagedHelmet_-6514.gltf"));
            //glm::mat4 helmetTrans{ 1.0f };
            //scene.physics.stepSim();
            //scene.updatePhysics();
            //helmet.transform = glm::rotate(helmet.transform,(float)glm::radians(deltaTime*90),glm::vec3(0,1,0));
            play = false;
        }
       

        float currentFrame = (float)glfwGetTime();
        eventHandler->deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;
        // input
        // -----
        //player->ProcessKeyboard();
        processHoldKeys(window);
        processInput(window);
        // render
        // ------
        glClearColor(0, 0, 0, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        // -----------------------------------------------
        //float near_plane = 1.f;
        //float far_plane = 25.0f;
        
        timer.Begin();
        // 1. render scene to depth cubemap
        // --------------------------------
        glViewport(0, 0, SHADOW_MAP_MAX_SIZE, SHADOW_MAP_MAX_SIZE);
        glBindFramebuffer(GL_FRAMEBUFFER, depthMapFBO);
        glClear(GL_DEPTH_BUFFER_BIT);
        //simpleDepthShader.setVec3("lightPos", scene.lightList[0].position);
        glEnable(GL_DEPTH_TEST);
        scene.drawShadowObjectsInstanced(simpleDepthShader_instanced); // point lights shadows

        glViewport(0, 0, SHADOW_MAP_MAX_SIZE_DIR, SHADOW_MAP_MAX_SIZE_DIR);
        glBindFramebuffer(GL_FRAMEBUFFER, depthMapFBO_dir);
        glClear(GL_DEPTH_BUFFER_BIT);
        //simpleDepthShader.setVec3("lightPos", scene.lightList[0].position);
        glEnable(GL_DEPTH_TEST);
        glCullFace(GL_FRONT);
        scene.drawShadowObjectsInstanced(dirShadow_instanced); // directional lights shadows
        glCullFace(GL_BACK);
        //helmet.draw(simpleDepthShader);
         
        glBindFramebuffer(GL_FRAMEBUFFER, 0);
        timer.End();
        try
        {
            std::cout << "\nShadowpass: " << (float)(timer.Elapsed_ns()).value() / 1000000 <<" ms";
        }
        catch (const std::bad_optional_access& e)
        {
            std::cout << e.what() << "\n";
        }
        timer.Begin();
            
        //glClearColor(1,1,1, 1.0f);
        glClearColor(0, 0, 0, 1.0f);
        glViewport(0, 0, User1.SCR_WIDTH ,User1.SCR_HEIGHT);
        glBindFramebuffer(GL_FRAMEBUFFER, postFBO);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        glEnable(GL_DEPTH_TEST);

            pbrShader.use();
            glm::mat4 projection = glm::perspective(glm::radians(90.0f), (float)User1.SCR_WIDTH / (float)User1.SCR_HEIGHT, 0.1f, 100.0f);
            glm::mat4 view = camera.GetViewMatrix();
            pbrShader.setMat4("projection", projection);
            pbrShader.setMat4("view", view);
            pbrShader.setVec3("viewPos", camera.Position);
            pbrShader.setVec3("spotLight.position", camera.Position);
            pbrShader.setVec3("spotLight.direction", camera.Front);
            pbrShader.setInt("doshadows", shadows); // enable/disable shadows by pressing '1'
            pbrShader.setInt("donormals", normals); // enable/disable normals by pressing '2'
            pbrShader.setBool("existnormals", 1);
            pbrShader.setInt("numLights", scene.numLights);
            pbrShader.setFloat("far_plane", far_plane);
            glActiveTexture(GL_TEXTURE11);
            glBindTexture(GL_TEXTURE_CUBE_MAP_ARRAY, depthCubemap);
            //scene.drawObjects(pbrShader);
            //helmet.draw(pbrShader);
            glActiveTexture(GL_TEXTURE0);
            //scene.drawobj(pbrShader);
            //axes.Draw(pbrShader);
            pbrShader_instanced.use();
            //glm::mat4 projection = glm::perspective(glm::radians(90.0f), (float)User1.SCR_WIDTH / (float)User1.SCR_HEIGHT, 0.1f, 100.0f);
            //glm::mat4 view = camera.GetViewMatrix();
            pbrShader_instanced.setMat4("projection", projection);
            pbrShader_instanced.setMat4("view", view);
            pbrShader_instanced.setVec3("viewPos", camera.Position);
            pbrShader_instanced.setVec3("spotLight.position", camera.Position);
            pbrShader_instanced.setVec3("spotLight.direction", camera.Front);
            pbrShader_instanced.setInt("doshadows", shadows); // enable/disable shadows by pressing '1'
            pbrShader_instanced.setInt("donormals", normals); // enable/disable normals by pressing '2'
            pbrShader_instanced.setBool("existnormals", 1);
            pbrShader_instanced.setInt("numLights", scene.numLights);
            pbrShader_instanced.setFloat("far_plane", far_plane);
            glActiveTexture(GL_TEXTURE12);
            glBindTexture(GL_TEXTURE_2D, depthMap);
            glActiveTexture(GL_TEXTURE11);
            glBindTexture(GL_TEXTURE_CUBE_MAP_ARRAY, depthCubemap);
            
            scene.drawObjectsInstanced(pbrShader_instanced);
            //helmet.draw(pbrShader);
            glActiveTexture(GL_TEXTURE0);
            //draw the bulbs
            
            glm::mat4 model1 = glm::mat4(1.0f);
            lightCubeShader.use();
            lightCubeShader.setMat4("projection", projection);
            lightCubeShader.setMat4("view", view);
            for (unsigned int i = 0; i < scene.numLights; i++)
            {
                model1 = glm::mat4(1.0f);
                model1 = glm::translate(model1, scene.lightList[i].position);
                model1 = glm::scale(model1, glm::vec3(1.0f)); // Make it a smaller cube
                // model1 = model1 * world_trans_intitial
                lightCubeShader.setMat4("model", model1);
                bulb.Draw(lightCubeShader);
            }
            model1 = glm::mat4(1.0f);
            model1 = glm::translate(model1, dlight.direction);
            model1 = glm::scale(model1, glm::vec3(0.5f));
            lightCubeShader.setMat4("model", model1);
            bulb.Draw(lightCubeShader);
            model1 = glm::mat4(1.0f);
            model1 = glm::translate(model1, glm::vec3(0));
            model1 = glm::scale(model1, glm::vec3(0.5f));
            lightCubeShader.setMat4("model", model1);
            bulb.Draw(lightCubeShader);
            wireShader.use();
            wireShader.setMat4("projection", projection);
            wireShader.setMat4("view", view);

            glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
            scene.drawHulls(wireShader);
            //helmet.draw(pbrShader);
            glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

        glBindFramebuffer(GL_READ_FRAMEBUFFER, postFBO);
        glBindFramebuffer(GL_DRAW_FRAMEBUFFER, intermediateFBO);
        glBlitFramebuffer(0, 0, User1.SCR_WIDTH, User1.SCR_HEIGHT, 0, 0, User1.SCR_WIDTH, User1.SCR_HEIGHT, GL_COLOR_BUFFER_BIT, GL_NEAREST);
        glBindFramebuffer(GL_FRAMEBUFFER, 0);
        glClear(GL_COLOR_BUFFER_BIT);
        //glDisable(GL_DEPTH_TEST);
        //post processing
        //------------------------------------------------------------------------------------------------------------------------
        
        hdrShader.use();
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, screenTexture);
        hdrShader.setInt("hdr", User1.hdr);
        hdrShader.setFloat("exposure", User1.exposure);
        glDisable(GL_DEPTH_TEST);
        renderQuad();
        timer.End();
        try
        {
            std::cout << "\nMainpass: " << (float)(timer.Elapsed_ns()).value() / 1000000<<" ms";
        }
        catch (const std::bad_optional_access& e)
        {
            std::cout << e.what() << "\n";
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
void processHoldKeys(GLFWwindow* window)
{
    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
        camera.ProcessKeyboard(FORWARD, eventHandler->deltaTime);
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
        camera.ProcessKeyboard(BACKWARD, eventHandler->deltaTime);
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
        camera.ProcessKeyboard(LEFT, eventHandler->deltaTime);
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
        camera.ProcessKeyboard(RIGHT, eventHandler->deltaTime);
}
void processInput(GLFWwindow* window)
{
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);
    if (glfwGetKey(window, GLFW_KEY_UP) == GLFW_PRESS)
        updatelightloc("forward", eventHandler->deltaTime);
    if (glfwGetKey(window, GLFW_KEY_DOWN) == GLFW_PRESS)
        updatelightloc("backward", eventHandler->deltaTime);
    if (glfwGetKey(window, GLFW_KEY_LEFT) == GLFW_PRESS)
        updatelightloc("left", eventHandler->deltaTime);
    if (glfwGetKey(window, GLFW_KEY_RIGHT) == GLFW_PRESS)
        updatelightloc("right", eventHandler->deltaTime);
    if (glfwGetKey(window, GLFW_KEY_Q) == GLFW_PRESS)
        updatelightloc("up", eventHandler->deltaTime);
    if (glfwGetKey(window, GLFW_KEY_E) == GLFW_PRESS)
        updatelightloc("down", eventHandler->deltaTime);
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
    if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS)
    {
        play = true;
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
        lastX = (float)xpos;
        lastY = (float)ypos;
        firstMouse = false;
    }

    float xoffset = (float)(xpos - lastX);
    float yoffset = (float)(lastY - ypos); // reversed since y-coordinates go from bottom to top

    lastX = (float)xpos;
    lastY = (float)ypos;

    camera.ProcessMouseMovement(xoffset, yoffset);
}

// glfw: whenever the mouse scroll wheel scrolls, this callback is called
// ----------------------------------------------------------------------
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
{
    camera.ProcessMouseScroll((float)yoffset);
}

void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
    if (key == GLFW_KEY_W && action == GLFW_PRESS)
    {
        eventHandler->fireCallback("Camera_Move_Forward");
    }
    if (key == GLFW_KEY_A && action == GLFW_PRESS)
    {
        eventHandler->fireCallback("Camera_Move_Left");
    }
    if (key == GLFW_KEY_S && action == GLFW_PRESS)
    {
        eventHandler->fireCallback("Camera_Move_Backward");
    }
    if (key == GLFW_KEY_D && action == GLFW_PRESS)
    {
        eventHandler->fireCallback("Camera_Move_Right");
    }
    if (key == GLFW_KEY_E && action == GLFW_PRESS)
    {
        eventHandler->fireCallback("Player_Move_Forward");
    }
    if (key == GLFW_KEY_GRAVE_ACCENT && action == GLFW_PRESS)
    {
        eventHandler->fireCallback("Update_Settings");
    }
}
void updatelightloc(std::string x, float deltaTime)
{
    float speed = 1;
    speed *= deltaTime;
    if (x == "forward")
    {
        //light[0].z += speed;
    }
    if (x == "backward")
    {
       // pointLightPositions[0].z -= speed;
    }
    if (x =="left")
    {
        //pointLightPositions[0].x += speed;
    }
    if (x =="right")
    {
        //pointLightPositions[0].x -= speed;
    }
    if (x == "up")
    {
        //pointLightPositions[0].y += speed;
    }
    if (x == "down")
    {
        //pointLightPositions[0].y -= speed;
    }

}
//
//void setLights(Scene scene)
//{
//    
//    numLights = scene.numLights;
//    light = scene.lightList;
//    
//}

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
        const float PI = (float)3.14159265359;
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

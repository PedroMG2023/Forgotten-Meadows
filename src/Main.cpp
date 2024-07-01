#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <stb_image.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <model_loading/shader_m.h>
#include <camera/camera.h>
#include <model_loading/model.h>
#include <filesystem.h>

#include <SFML/Audio.hpp>
#include <SFML/OpenGL.hpp>
#include <SFML/Window.hpp>
#include <SFML/Graphics.hpp>

#include <iostream>

// callback functions
void processInput(sf::Window& window);
void scroll_callback(sf::Window& window, double xoffset, double yoffset);
unsigned int loadTexture(const char* path);
unsigned int loadCubemap(vector<std::string> faces);

// settings
const unsigned int SCR_WIDTH = 800;
const unsigned int SCR_HEIGHT = 600;

// camera
Camera camera(glm::vec3(5.0f, 0.8f, 3.0f));
float lastX = SCR_WIDTH / 2.0f;
float lastY = SCR_HEIGHT / 2.0f;
bool firstMouse = true;

// timing
float deltaTime = 0.0f;
float lastFrame = 0.0f;

int main()
{
    // create a window
    sf::RenderWindow window(sf::VideoMode(800, 600), "Forgotten-Meadows", sf::Style::Default, sf::ContextSettings(24));
    window.setFramerateLimit(60);
    window.setActive(true);
    window.setMouseCursorGrabbed(true);
    window.setMouseCursorVisible(false);

    // initialize OpenGL
    if (!gladLoadGLLoader((GLADloadproc)sf::Context::getFunction)) {
        std::cout << "Failed to initialize GLAD" << std::endl;
        return -1;
    }

    // configure global OpenGL state
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    // build and compile shaders
    Shader ourShader("model_loading.vs", "model_loading.fs");
    Shader light_shader("light_cube.vs", "light_cube.fs");

    float vertices[] = {
        -0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,
         0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,
         0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,
         0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,
        -0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,
        -0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,

        -0.5f, -0.5f,  0.5f,  0.0f,  0.0f,  1.0f,
         0.5f, -0.5f,  0.5f,  0.0f,  0.0f,  1.0f,
         0.5f,  0.5f,  0.5f,  0.0f,  0.0f,  1.0f,
         0.5f,  0.5f,  0.5f,  0.0f,  0.0f,  1.0f,
        -0.5f,  0.5f,  0.5f,  0.0f,  0.0f,  1.0f,
        -0.5f, -0.5f,  0.5f,  0.0f,  0.0f,  1.0f,

        -0.5f,  0.5f,  0.5f, -1.0f,  0.0f,  0.0f,
        -0.5f,  0.5f, -0.5f, -1.0f,  0.0f,  0.0f,
        -0.5f, -0.5f, -0.5f, -1.0f,  0.0f,  0.0f,
        -0.5f, -0.5f, -0.5f, -1.0f,  0.0f,  0.0f,
        -0.5f, -0.5f,  0.5f, -1.0f,  0.0f,  0.0f,
        -0.5f,  0.5f,  0.5f, -1.0f,  0.0f,  0.0f,

         0.5f,  0.5f,  0.5f,  1.0f,  0.0f,  0.0f,
         0.5f,  0.5f, -0.5f,  1.0f,  0.0f,  0.0f,
         0.5f, -0.5f, -0.5f,  1.0f,  0.0f,  0.0f,
         0.5f, -0.5f, -0.5f,  1.0f,  0.0f,  0.0f,
         0.5f, -0.5f,  0.5f,  1.0f,  0.0f,  0.0f,
         0.5f,  0.5f,  0.5f,  1.0f,  0.0f,  0.0f,

        -0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,
         0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,
         0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,
         0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,
        -0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,
        -0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,

        -0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,
         0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,
         0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,
         0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,
        -0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,
        -0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f
    };

    glm::vec3 cubePositions[] = {
       glm::vec3(55.0f, 2.0f,  0.0f),
    };

    // positions of the point lights
    glm::vec3 steetlight_position[] = {
        //left to right
        glm::vec3(75.f, 4.0f, -64.44f),
        glm::vec3(75.f,  4.0f, -40.34f),
        glm::vec3(75.f,  4.0f, -16.43f),
        glm::vec3(75.f,  4.0f,  8.32f),
        glm::vec3(75.f,  4.0f, 32.27f),
        glm::vec3(75.f,  4.0f, 56.32f),

        //Torch light
        //church
        glm::vec3(-51.87f,  2.0f,  -4.66f),
        //void
        glm::vec3(-32.30f,  2.0f, 47.72f),
        //grenn
        glm::vec3(-7.72f,  2.0f, -57.33f),
        //bathroom
        glm::vec3(4.62f,  2.0f,  -0.39f),
        //
        glm::vec3(16.57,  2.0f, 38.64f),
        //
        glm::vec3(56.32f,  2.0f, -0.08f),
        //green
        glm::vec3(35.10f,  2.0f, -2.8f),
        //campfire
        glm::vec3(-5.26f,  2.0f, -32.33f)
    };
    unsigned int VBO, cubeVAO;
    glGenVertexArrays(1, &cubeVAO);
    glGenBuffers(1, &VBO);

    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    glBindVertexArray(cubeVAO);

    // position attribute
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    // normal attribute
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);


    // second, configure the light's VAO (VBO stays the same; the vertices are the same for the light object which is also a 3D cube)
    unsigned int lightCubeVAO;
    glGenVertexArrays(1, &lightCubeVAO);
    glBindVertexArray(lightCubeVAO);

    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    // note that we update the lamp's position attribute's stride to reflect the updated buffer data
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);


    // load models
    Model ourModel(FileSystem::getPath("/resources/models/scene.gltf"));


    // shader configuration
    // --------------------
    ourShader.use();
    ourShader.setInt("material.diffuse", 0);


    // Audio ambient
    sf::Music music;
    music.openFromFile(FileSystem::getPath("resources/audio/NighttimeForest.mp3"));

    music.setLoop(true); // Repeat Music
    music.setVolume(30.0f);
    music.play();


    sf::Clock clock;


    // render loop
    while (window.isOpen())
    {

        // per-frame time logic
        float currentFrame = static_cast<float>(clock.getElapsedTime().asSeconds());
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;

        //input
        processInput(window);

        // render
        glClearColor(0.05f, 0.05f, 0.05f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        // be sure to activate shader when setting uniforms/drawing objects
        ourShader.use();
        ourShader.setVec3("viewPos", camera.Position);
        ourShader.setFloat("material.shininess", 32.0f);

        // directional light
        ourShader.setVec3("dirLight.direction", -0.2f, 2.0f, -0.3f);
        ourShader.setVec3("dirLight.ambient", 0.2f, 0.2f, 0.2f);
        ourShader.setVec3("dirLight.diffuse", 0.4f, 0.4f, 0.4f);
        ourShader.setVec3("dirLight.specular", 0.5f, 0.5f, 0.5f);

        for (int i = 0; i < 6; i++)
        {
            ourShader.setVec3("pointLights[" + std::to_string(i) + "].position", steetlight_position[i]);
            ourShader.setVec3("pointLights[" + std::to_string(i) + "].ambient", 0.678f, 0.847f, 0.902f);
            ourShader.setVec3("pointLights[" + std::to_string(i) + "].diffuse", 0.8f, 0.8f, 0.8f);
            ourShader.setVec3("pointLights[" + std::to_string(i) + "].specular", 1.0f, 1.0f, 1.0f);
            ourShader.setFloat("pointLights[" + std::to_string(i) + "].constant", 1.0f);
            ourShader.setFloat("pointLights[" + std::to_string(i) + "].linear", 0.14f);
            ourShader.setFloat("pointLights[" + std::to_string(i) + "].quadratic", 0.07f);
        }

        for (int i = 6; i < 14; i++)
        {
            ourShader.setVec3("pointLights[" + std::to_string(i) + "].position", steetlight_position[i]);
            ourShader.setVec3("pointLights[" + std::to_string(i) + "].ambient", 1.0f, 0.647f, 0.0f);
            ourShader.setVec3("pointLights[" + std::to_string(i) + "].diffuse", 0.8f, 0.8f, 0.8f);
            ourShader.setVec3("pointLights[" + std::to_string(i) + "].specular", 1.0f, 1.0f, 1.0f);
            ourShader.setFloat("pointLights[" + std::to_string(i) + "].constant", 1.0f);
            ourShader.setFloat("pointLights[" + std::to_string(i) + "].linear", 0.22f);
            ourShader.setFloat("pointLights[" + std::to_string(i) + "].quadratic", 0.20f);
        }
        

        // spotLight
        ourShader.setVec3("spotLight.position", camera.Position);
        ourShader.setVec3("spotLight.direction", camera.Front);
        ourShader.setVec3("spotLight.ambient", 0.0f, 0.0f, 0.0f);
        ourShader.setVec3("spotLight.diffuse", 1.0f, 1.0f, 1.0f);
        ourShader.setVec3("spotLight.specular", 1.0f, 1.0f, 1.0f);
        ourShader.setFloat("spotLight.constant", 1.0f);
        ourShader.setFloat("spotLight.linear", 0.09f);
        ourShader.setFloat("spotLight.quadratic", 0.032f);
        ourShader.setFloat("spotLight.cutOff", glm::cos(glm::radians(12.5f)));
        ourShader.setFloat("spotLight.outerCutOff", glm::cos(glm::radians(15.0f)));


        // view/projection transformations
        glm::mat4 projection = glm::perspective(glm::radians(camera.Zoom), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 1000.0f);
        glm::mat4 view = camera.GetViewMatrix();
        ourShader.setMat4("projection", projection);
        ourShader.setMat4("view", view);

        // render the loaded model
        glm::mat4 model = glm::mat4(1.0f);
        model = glm::translate(model, glm::vec3(0.0f, 0.0f, 0.0f));
        model = glm::scale(model, glm::vec3(1.0f, 1.0f, 1.0f));
        ourShader.setMat4("model", model);
        ourModel.Draw(ourShader);

        // also draw the lamp object(s)
        light_shader.use();
        light_shader.setMat4("projection", projection);
        light_shader.setMat4("view", view);

        // we now draw as many light bulbs as we have point lights.
       /* glBindVertexArray(lightCubeVAO);
        for (unsigned int i = 0; i < 4; i++)
        {
            model = glm::mat4(1.0f);
            model = glm::translate(model, pointLightPositions[i]);
            model = glm::scale(model, glm::vec3(0.2f)); // Make it a smaller cube
            light_shader.setMat4("model", model);
            glDrawArrays(GL_TRIANGLES, 0, 36);
        }*/

        // swap buffers and poll IO events
        window.display();

        sf::Event event;
        while (window.pollEvent(event))
        {

            if (event.type == sf::Event::Closed) {
                window.close();
            }

            if (event.type == sf::Event::Resized)
            {
                glViewport(0, 0, event.size.width, event.size.height);

            }

            if (event.type == sf::Event::MouseMoved)

            {
                sf::Vector2i center(window.getSize().x / 2, window.getSize().y / 2);

                float xoffset = sf::Mouse::getPosition(window).x - (float)center.x;
                float yoffset = (float)center.y - sf::Mouse::getPosition(window).y;

                camera.ProcessMouseMovement(xoffset, yoffset);

                sf::Mouse::setPosition(sf::Vector2<int>((int)window.getSize().x / 2, (int)window.getSize().y / 2), window);
                //mouse_callback(window, event.mouseMove.x, event.mouseMove.y);

            }

            if (event.type == sf::Event::MouseWheelScrolled)
                scroll_callback(window, event.mouseWheelScroll.delta, 0);

        }

    }
    return 0;
}


void processInput(sf::Window& window)
{
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Escape))
        window.close();

    if (sf::Keyboard::isKeyPressed(sf::Keyboard::W))
        camera.ProcessKeyboard(FORWARD, deltaTime);

    if (sf::Keyboard::isKeyPressed(sf::Keyboard::S))
        camera.ProcessKeyboard(BACKWARD, deltaTime);

    if (sf::Keyboard::isKeyPressed(sf::Keyboard::A))
        camera.ProcessKeyboard(LEFT, deltaTime);

    if (sf::Keyboard::isKeyPressed(sf::Keyboard::D))
        camera.ProcessKeyboard(RIGHT, deltaTime);

}

void scroll_callback(sf::Window& window, double xoffset, double yoffset)
{
    camera.ProcessMouseScroll(static_cast<float>(yoffset));
}

unsigned int loadTexture(char const* path)
{
    unsigned int textureID;
    glGenTextures(1, &textureID);

    int width, height, nrComponents;
    unsigned char* data = stbi_load(path, &width, &height, &nrComponents, 0);
    if (data)
    {
        GLenum format;
        if (nrComponents == 1)
            format = GL_RED;
        else if (nrComponents == 3)
            format = GL_RGB;
        else if (nrComponents == 4)
            format = GL_RGBA;

        glBindTexture(GL_TEXTURE_2D, textureID);
        glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
        glGenerateMipmap(GL_TEXTURE_2D);

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

        stbi_image_free(data);
    }
    else
    {
        std::cout << "Texture failed to load at path: " << path << std::endl;
        stbi_image_free(data);
    }

    return textureID;
}

// loads a cubemap texture from 6 individual texture faces
// order:
// +X (right)
// -X (left)
// +Y (top)
// -Y (bottom)
// +Z (front) 
// -Z (back)
// -------------------------------------------------------
unsigned int loadCubemap(vector<std::string> faces)
{
    unsigned int textureID;
    glGenTextures(1, &textureID);
    glBindTexture(GL_TEXTURE_CUBE_MAP, textureID);

    int width, height, nrChannels;
    for (unsigned int i = 0; i < faces.size(); i++)
    {
        unsigned char* data = stbi_load(faces[i].c_str(), &width, &height, &nrChannels, 0);
        if (data)
        {
            glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
            stbi_image_free(data);
        }
        else
        {
            std::cout << "Cubemap texture failed to load at path: " << faces[i] << std::endl;
            stbi_image_free(data);
        }
    }
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

    return textureID;
}
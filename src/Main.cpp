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
void processInput(sf::Window& window, const Model& model, const Model& model2);
void scroll_callback(sf::Window& window, double xoffset, double yoffset);
unsigned int loadTexture(const char* path);
unsigned int loadCubemap(vector<std::string> faces);
bool CheckCollision(const Camera& camera, const Model& model, bool val);
void resolveCollision(Camera& camera, const Model& model);


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

bool flashlightOn = false;
bool fKeyPressed = false;

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
    //Shader light_shader("light_cube.vs", "light_cube.fs");
    Shader skyboxShader("skybox.vs", "skybox.fs");


    float skyboxVertices[] = {
        // positions          
        -1.0f,  1.0f, -1.0f,
        -1.0f, -1.0f, -1.0f,
         1.0f, -1.0f, -1.0f,
         1.0f, -1.0f, -1.0f,
         1.0f,  1.0f, -1.0f,
        -1.0f,  1.0f, -1.0f,

        -1.0f, -1.0f,  1.0f,
        -1.0f, -1.0f, -1.0f,
        -1.0f,  1.0f, -1.0f,
        -1.0f,  1.0f, -1.0f,
        -1.0f,  1.0f,  1.0f,
        -1.0f, -1.0f,  1.0f,

         1.0f, -1.0f, -1.0f,
         1.0f, -1.0f,  1.0f,
         1.0f,  1.0f,  1.0f,
         1.0f,  1.0f,  1.0f,
         1.0f,  1.0f, -1.0f,
         1.0f, -1.0f, -1.0f,

        -1.0f, -1.0f,  1.0f,
        -1.0f,  1.0f,  1.0f,
         1.0f,  1.0f,  1.0f,
         1.0f,  1.0f,  1.0f,
         1.0f, -1.0f,  1.0f,
        -1.0f, -1.0f,  1.0f,

        -1.0f,  1.0f, -1.0f,
         1.0f,  1.0f, -1.0f,
         1.0f,  1.0f,  1.0f,
         1.0f,  1.0f,  1.0f,
        -1.0f,  1.0f,  1.0f,
        -1.0f,  1.0f, -1.0f,

        -1.0f, -1.0f, -1.0f,
        -1.0f, -1.0f,  1.0f,
         1.0f, -1.0f, -1.0f,
         1.0f, -1.0f, -1.0f,
        -1.0f, -1.0f,  1.0f,
         1.0f, -1.0f,  1.0f
    };

    // skybox VAO
    unsigned int skyboxVAO, skyboxVBO;
    glGenVertexArrays(1, &skyboxVAO);
    glGenBuffers(1, &skyboxVBO);
    glBindVertexArray(skyboxVAO);
    glBindBuffer(GL_ARRAY_BUFFER, skyboxVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(skyboxVertices), &skyboxVertices, GL_STATIC_DRAW);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);

    // load textures
    // -------------

    vector<std::string> faces
    {

        FileSystem::getPath("resources/skybox images/Scene_-_Root_baseColor.right.png"), // Derecha
        FileSystem::getPath("resources/skybox images/Scene_-_Root_baseColor.left.png"), // Izquierda
        FileSystem::getPath("resources/skybox images/Scene_-_Root_baseColor.up.png"), // Arriba
        FileSystem::getPath("resources/skybox images/Scene_-_Root_baseColor.down.png"), // Abajo
        FileSystem::getPath("resources/skybox images/Scene_-_Root_baseColor.front.png"), // Frente
        FileSystem::getPath("resources/skybox images/Scene_-_Root_baseColor.back.png")  // Atr�s

    };
    unsigned int cubemapTexture = loadCubemap(faces);

    // shader configuration

    skyboxShader.use();
    skyboxShader.setInt("skybox", 0);


    glm::vec3 street_light_bad[] =
    {
        glm::vec3(75.f,  4.0f, -40.34f),
        glm::vec3(75.f, 4.0f, -64.44f),
        glm::vec3(75.f,  4.0f, 56.32f),

    };

    // positions of the point lights
    glm::vec3 steetlight_position[] = {
        //left to right
        glm::vec3(75.f,  4.0f, -40.34f),
        glm::vec3(75.f,  4.0f, -16.43f),
        glm::vec3(75.f,  4.0f,  8.32f),
        glm::vec3(75.f,  4.0f, 32.27f),

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
   

    // load models
    Model ourModel(FileSystem::getPath("/resources/models/SceneWSM.gltf"));
    Model ourModel2(FileSystem::getPath("/resources/models/SomeModels.gltf"));
    Model ourModel3(FileSystem::getPath("/resources/models/blades.gltf"));


    // shader configuration
    // --------------------
    ourShader.use();
    ourShader.setInt("material.diffuse", 0);


    // Audio ambient
    sf::SoundBuffer buffer;
    if (!buffer.loadFromFile(FileSystem::getPath("resources/audio/NighttimeForest.mp3"))) {
        return -1;
    }

    sf::Sound sound_ambient;
    sound_ambient.setBuffer(buffer);
    sound_ambient.setVolume(25.f);
    sound_ambient.setLoop(true);
    sound_ambient.play();

    //Steps Audio
    sf::SoundBuffer buffer2;
    if (!buffer2.loadFromFile(FileSystem::getPath("resources/audio/Grass_Footsteps.mp3"))) {
        return -1;
    }

    sf::Sound sound_steps;
    sound_steps.setBuffer(buffer2);
    sound_steps.setVolume(40.0f);
    sound_steps.setLoop(true);

    sf::Clock clock;

    
    // render loop
    while (window.isOpen())
    {

        // per-frame time logic
        float currentFrame = static_cast<float>(clock.getElapsedTime().asSeconds());
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;

        //input
        processInput(window, ourModel, ourModel2);

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
        

        for (int i = 0; i < 4; i++)
        {
            ourShader.setVec3("pointLights[" + std::to_string(i) + "].position", steetlight_position[i]);
            ourShader.setVec3("pointLights[" + std::to_string(i) + "].ambient", 0.678f, 0.847f, 0.902f);
            ourShader.setVec3("pointLights[" + std::to_string(i) + "].diffuse", 0.8f, 0.8f, 0.8f);
            ourShader.setVec3("pointLights[" + std::to_string(i) + "].specular", 1.0f, 1.0f, 1.0f);
            ourShader.setFloat("pointLights[" + std::to_string(i) + "].constant", 1.0f);
            ourShader.setFloat("pointLights[" + std::to_string(i) + "].linear", 0.14f);
            ourShader.setFloat("pointLights[" + std::to_string(i) + "].quadratic", 0.07f);
        }

        for (int i = 4; i < 14; i++)
        {
            ourShader.setVec3("pointLights[" + std::to_string(i) + "].position", steetlight_position[i]);
            ourShader.setVec3("pointLights[" + std::to_string(i) + "].ambient", 1.0f, 0.647f, 0.0f);
            ourShader.setVec3("pointLights[" + std::to_string(i) + "].diffuse", 0.8f, 0.8f, 0.8f);
            ourShader.setVec3("pointLights[" + std::to_string(i) + "].specular", 1.0f, 1.0f, 1.0f);
            ourShader.setFloat("pointLights[" + std::to_string(i) + "].constant", 1.0f);
            ourShader.setFloat("pointLights[" + std::to_string(i) + "].linear", 0.22f);
            ourShader.setFloat("pointLights[" + std::to_string(i) + "].quadratic", 0.20f);
        }

        int randomNumber = rand();
        for (int i = 0; i < 2; i++)
        {
            if (randomNumber % 2 == 0) {
                ourShader.setVec3("pointLights[" + std::to_string(1) + "].position", street_light_bad[i]);
                ourShader.setVec3("pointLights[" + std::to_string(1) + "].ambient", 0.678f, 0.847f, 0.902f);
                ourShader.setVec3("pointLights[" + std::to_string(1) + "].diffuse", 0.8f, 0.8f, 0.8f);
                ourShader.setVec3("pointLights[" + std::to_string(1) + "].specular", 1.0f, 1.0f, 1.0f);
                ourShader.setFloat("pointLights[" + std::to_string(1) + "].constant", 1.0f);
                ourShader.setFloat("pointLights[" + std::to_string(1) + "].linear", 0.35f);
                ourShader.setFloat("pointLights[" + std::to_string(1) + "].quadratic", 0.44f);
            }
        }


        // spotLight

        if (sf::Keyboard::isKeyPressed(sf::Keyboard::F)) {
            if (!fKeyPressed) {
                flashlightOn = !flashlightOn;
                fKeyPressed = true;
            }
        }
        else
        {
            fKeyPressed = false;
        }

        ourShader.setVec3("spotLight.position", camera.Position);
        ourShader.setVec3("spotLight.direction", camera.Front);
        ourShader.setVec3("spotLight.ambient", 0.0f, 0.0f, 0.0f);
        ourShader.setVec3("spotLight.diffuse", 0.0f, 0.0f, 0.0f);
        ourShader.setVec3("spotLight.specular", 0.0f, 0.0f, 0.0f);
        ourShader.setFloat("spotLight.constant", 1.0f);
        ourShader.setFloat("spotLight.linear", 0.09f);
        ourShader.setFloat("spotLight.quadratic", 0.032f);
        ourShader.setFloat("spotLight.cutOff", glm::cos(glm::radians(12.5f)));
        ourShader.setFloat("spotLight.outerCutOff", glm::cos(glm::radians(15.0f)));

        if (flashlightOn) {
            ourShader.setVec3("spotLight.position", camera.Position);
            ourShader.setVec3("spotLight.direction", camera.Front);
            ourShader.setVec3("spotLight.ambient", 0.1f, 0.1f, 0.1f);
            ourShader.setVec3("spotLight.diffuse", 1.0f, 1.0f, 1.0f);
            ourShader.setVec3("spotLight.specular", 1.0f, 1.0f, 1.0f);
            ourShader.setFloat("spotLight.constant", 1.0f);
            ourShader.setFloat("spotLight.linear", 0.09f);
            ourShader.setFloat("spotLight.quadratic", 0.032f);
            ourShader.setFloat("spotLight.cutOff", glm::cos(glm::radians(12.5f)));
            ourShader.setFloat("spotLight.outerCutOff", glm::cos(glm::radians(15.0f)));
        }
        else {
            ourShader.setVec3("spotLight.ambient", 0.0f, 0.0f, 0.0f);
            ourShader.setVec3("spotLight.diffuse", 0.0f, 0.0f, 0.0f);
            ourShader.setVec3("spotLight.specular", 0.0f, 0.0f, 0.0f);
        }


        // view/projection transformations
        glm::mat4 projection = glm::perspective(glm::radians(camera.Zoom), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 1000.0f);
        glm::mat4 view = camera.GetViewMatrix();
        ourShader.setMat4("projection", projection);
        ourShader.setMat4("view", view);

        // render the loaded models
        glm::mat4 model = glm::mat4(1.0f);
        model = glm::translate(model, glm::vec3(0.0f, 0.0f, 0.0f));
        model = glm::scale(model, glm::vec3(1.0f, 1.0f, 1.0f));
        ourShader.setMat4("model", model);
        ourModel.Draw(ourShader);

        glm::mat4 model2 = glm::mat4(1.0f);
        model2 = glm::translate(model2, glm::vec3(0.0f, 0.0f, 0.0f));
        model2 = glm::scale(model2, glm::vec3(1.0f, 1.0f, 1.0f));
        ourShader.setMat4("model", model2);
        ourModel2.Draw(ourShader);

        glm::mat4 model3 = glm::mat4(1.0f);
        model3 = glm::translate(model3, glm::vec3(-52.067f, 12.151f, 18.089f));
        model3 = glm::scale(model3, glm::vec3(1.0f, 1.0f, 1.0f));
        static float angle = 0.0f;
        angle += 1.0f;  // rotation velocity
        model3 = glm::rotate(model3, glm::radians(angle), glm::vec3(1.0f, 0.0f, 0.0f));
        ourShader.setMat4("model", model3);
        ourModel3.Draw(ourShader);

        /*also draw the lamp object(s)
        light_shader.use();
        light_shader.setMat4("projection", projection);
        light_shader.setMat4("view", view);

        // we now draw as many light bulbs as we have point lights.
        glBindVertexArray(lightCubeVAO);
        for (unsigned int i = 0; i < 4; i++)
        {
            model = glm::mat4(1.0f);
            model = glm::translate(model, pointLightPositions[i]);
            model = glm::scale(model, glm::vec3(0.2f)); // Make it a smaller cube
            light_shader.setMat4("model", model);
            glDrawArrays(GL_TRIANGLES, 0, 36);
        }*/


        // draw skybox as last
        glDepthFunc(GL_LEQUAL);  // change depth function so depth test passes when values are equal to depth buffer's content
        skyboxShader.use();
        view = glm::mat4(glm::mat3(camera.GetViewMatrix())); // remove translation from the view matrix
        skyboxShader.setMat4("view", view);
        skyboxShader.setMat4("projection", projection);
        // skybox cube
        glBindVertexArray(skyboxVAO);
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_CUBE_MAP, cubemapTexture);
        glDrawArrays(GL_TRIANGLES, 0, 36);
        glBindVertexArray(0);
        glDepthFunc(GL_LESS); // set depth function back to default


        window.display();

        // swap buffers and poll IO events

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

            if (!sf::Keyboard::isKeyPressed(sf::Keyboard::W) && !sf::Keyboard::isKeyPressed(sf::Keyboard::S) && !sf::Keyboard::isKeyPressed(sf::Keyboard::A) && !sf::Keyboard::isKeyPressed(sf::Keyboard::D))
            {
                sound_steps.play();
            }
        }

    }
    return 0;
}

void resolveCollision(Camera& camera, const Model& model) {
    glm::vec3 cameraPos = camera.Position;

    for (const auto& mesh : model.meshes)
    {
        glm::vec3 minBox = mesh.boundingBox.min;
        glm::vec3 maxBox = mesh.boundingBox.max;

        if (camera.Position.x >= minBox.x && camera.Position.x <= maxBox.x &&
            camera.Position.y >= minBox.y && camera.Position.y <= maxBox.y &&
            camera.Position.z >= minBox.z && camera.Position.z <= maxBox.z)
        {
            glm::vec3 collisionVector = (minBox + maxBox) * 0.5f - camera.Position;

            collisionVector = glm::normalize(collisionVector);

            glm::vec3 closestPoint = glm::clamp(camera.Position, minBox, maxBox);
            camera.Position = closestPoint + collisionVector * 0.08f; 
            break;

        }
    }
}

bool CheckCollision(const Camera& camera, const Model& model, bool val) {
    glm::vec3 cameraPos = camera.Position;

    for (const auto& mesh : model.meshes)
    {
        glm::vec3 minBox = mesh.boundingBox.min;
        glm::vec3 maxBox = mesh.boundingBox.max;

        if (val) {
            maxBox.y = maxBox.y + camera.Position.y;
        }

        if (cameraPos.x >= minBox.x && cameraPos.x <= maxBox.x &&
            cameraPos.y >= minBox.y && cameraPos.y <= maxBox.y &&
            cameraPos.z >= minBox.z && cameraPos.z <= maxBox.z)
        {
            return true;
        }

    }

    return false;
}


void processInput(sf::Window& window, const Model& model, const Model& model2)
{
    if (sf::Keyboard::isKeyPressed(sf::Keyboard::Escape))
        window.close();

    if (true) {

        if (sf::Keyboard::isKeyPressed(sf::Keyboard::W)) {
            glm::vec3 front = camera.Front;
            glm::vec3 newPos = camera.Position + front * camera.MovementSpeed * deltaTime;
            if (!CheckCollision(newPos, model, false) && !CheckCollision(newPos, model2, true))
                camera.Position = newPos;
        }
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::S)) {
            glm::vec3 front = camera.Front;
            glm::vec3 newPos = camera.Position - front * camera.MovementSpeed * deltaTime;
            if (!CheckCollision(newPos, model, false) && !CheckCollision(newPos, model2, true))
                camera.Position = newPos;
        }
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::A)) {
            glm::vec3 right = camera.Right;
            glm::vec3 newPos = camera.Position - right * camera.MovementSpeed * deltaTime;
            if (!CheckCollision(newPos, model, false) && !CheckCollision(newPos, model2, true))
                camera.Position = newPos;
        }
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::D)) {
            glm::vec3 right = camera.Right;
            glm::vec3 newPos = camera.Position + right * camera.MovementSpeed * deltaTime;
            if (!CheckCollision(newPos, model, false) && !CheckCollision(newPos, model2, true))
                camera.Position = newPos;
        }

    }

    else {

        if (sf::Keyboard::isKeyPressed(sf::Keyboard::W))
            camera.ProcessKeyboard(FORWARD, deltaTime);

        if (sf::Keyboard::isKeyPressed(sf::Keyboard::S))
            camera.ProcessKeyboard(BACKWARD, deltaTime);

        if (sf::Keyboard::isKeyPressed(sf::Keyboard::A))
            camera.ProcessKeyboard(LEFT, deltaTime);

        if (sf::Keyboard::isKeyPressed(sf::Keyboard::D))
            camera.ProcessKeyboard(RIGHT, deltaTime);

    }

    camera.Position.y = 2.0f;
    
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
            glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
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
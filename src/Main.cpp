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

// settings
const unsigned int SCR_WIDTH = 800;
const unsigned int SCR_HEIGHT = 600;

// camera
Camera camera(glm::vec3(0.0f, 0.0f, 3.0f));
float lastX = SCR_WIDTH / 2.0f;
float lastY = SCR_HEIGHT / 2.0f;
bool firstMouse = true;

// timing
float deltaTime = 0.0f;
float lastFrame = 0.0f;

int main()
{
    // create a window
    sf::RenderWindow window( sf::VideoMode(800, 600), "Forgotten-Meadows", sf::Style::Default, sf::ContextSettings(24));
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

    // build and compile shaders
    Shader ourShader("model_loading.vs", "model_loading.fs");


    // load models
    Model ourModel(FileSystem::getPath("/resources/models/casa.gltf"));


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

/*
    ============================================================
    Tank Battle - OpenGL

    Author: Leonardo Moura
    Date: 6/12/2026

    Description:

    This application implements a 2D top-down tank battle game
    using modern OpenGL.

    The project demonstrates:

        • OpenGL initialization
        • Shader management
        • Game loop architecture
        • Delta time based movement
        • Keyboard input handling
        • Real-time rendering
        • Enemy AI systems
        • Collision detection
        • Procedural map generation

    Technologies:

        • OpenGL 3.3 Core Profile
        • GLFW
        • GLEW
        • GLM
        • C++

    ============================================================
*/

#include <glew.h>
#include <glfw3.h>

#include <iostream>

#include "Shader.h"
#include "Game.h"

//--------------------------------------------------
// Fixed game resolution.
//
// The entire game world is designed around
// a 1280x720 viewport.
//--------------------------------------------------

constexpr int WINDOW_WIDTH = 1280;
constexpr int WINDOW_HEIGHT = 720;

int main()
{
    //--------------------------------------------------
    // Initialize GLFW.
    //
    // GLFW is responsible for:
    //
    //  • Window creation
    //  • OpenGL context creation
    //  • Input handling
    //  • Event processing
    //--------------------------------------------------

    if (!glfwInit())
    {
        std::cout
            << "Failed to initialize GLFW"
            << std::endl;

        return -1;
    }

    //--------------------------------------------------
    // Configure the OpenGL version.
    //
    // We are using:
    //
    //      OpenGL 3.3 Core Profile
    //
    // The Core Profile removes legacy OpenGL
    // functionality and forces modern rendering.
    //--------------------------------------------------

    glfwWindowHint(
        GLFW_CONTEXT_VERSION_MAJOR,
        3);

    glfwWindowHint(
        GLFW_CONTEXT_VERSION_MINOR,
        3);

    glfwWindowHint(
        GLFW_OPENGL_PROFILE,
        GLFW_OPENGL_CORE_PROFILE);

    //--------------------------------------------------
    // Create the game window.
    //
    // Parameters:
    //
    //  Width
    //  Height
    //  Window Title
    //  Monitor (nullptr = windowed mode)
    //  Shared Context (nullptr = none)
    //--------------------------------------------------

    GLFWwindow* window =
        glfwCreateWindow(
            WINDOW_WIDTH,
            WINDOW_HEIGHT,
            "Tank Battle",
            nullptr,
            nullptr);

    //--------------------------------------------------
    // Verify that the window was successfully created.
    //--------------------------------------------------

    if (!window)
    {
        std::cout
            << "Failed to create window"
            << std::endl;

        glfwTerminate();

        return -1;
    }

    //--------------------------------------------------
    // Make the OpenGL context associated with the
    // newly created window the current context.
    //
    // OpenGL commands executed after this point
    // will affect this window.
    //--------------------------------------------------

    glfwMakeContextCurrent(
        window);

    //--------------------------------------------------
    // Initialize GLEW.
    //
    // GLEW loads modern OpenGL functions such as:
    //
    //      glCreateShader()
    //      glCreateProgram()
    //      glGenVertexArrays()
    //      glBindVertexArray()
    //
    // Without GLEW these functions would not be
    // available through function pointers.
    //--------------------------------------------------

    glewExperimental =
        GL_TRUE;

    GLenum glewResult =
        glewInit();

    //--------------------------------------------------
    // Verify GLEW initialization.
    //--------------------------------------------------

    if (glewResult != GLEW_OK)
    {
        std::cout
            << "GLEW ERROR: "
            << glewGetErrorString(
                glewResult)
            << std::endl;

        glfwDestroyWindow(
            window);

        glfwTerminate();

        return -1;
    }

    //--------------------------------------------------
    // Configure the OpenGL viewport.
    //
    // The viewport defines the portion of the
    // window where rendering will occur.
    //--------------------------------------------------

    glViewport(
        0,
        0,
        WINDOW_WIDTH,
        WINDOW_HEIGHT);

    //--------------------------------------------------
    // Enable alpha blending.
    //
    // This allows transparent objects, particles,
    // explosions, UI elements and future visual
    // effects to be rendered correctly.
    //--------------------------------------------------

    glEnable(
        GL_BLEND);

    //--------------------------------------------------
    // Configure the blending equation.
    //
    // FinalColor =
    //      SourceColor * Alpha
    //    + DestinationColor * (1 - Alpha)
    //--------------------------------------------------

    glBlendFunc(
        GL_SRC_ALPHA,
        GL_ONE_MINUS_SRC_ALPHA);

    //--------------------------------------------------
    // Load and compile the shader program.
    //
    // Vertex Shader:
    //      Processes vertex positions.
    //
    // Fragment Shader:
    //      Processes pixel colors.
    //--------------------------------------------------

    Shader shader(
        "Src/Shaders/vertex.glsl",
        "Src/Shaders/fragment.glsl");

    //--------------------------------------------------
    // Create the main game object.
    //
    // The Game class owns:
    //
    //  • Player
    //  • Enemies
    //  • Bullets
    //  • Map
    //  • Renderer
    //--------------------------------------------------

    Game game(
        window,
        WINDOW_WIDTH,
        WINDOW_HEIGHT);

    //--------------------------------------------------
    // Initialize all game systems.
    //
    // This includes:
    //
    //  • Renderer setup
    //  • Procedural map generation
    //  • Player spawning
    //  • Enemy spawning
    //--------------------------------------------------

    if (!game.Initialize(
        &shader))
    {
        std::cout
            << "Game initialization failed"
            << std::endl;

        glfwDestroyWindow(
            window);

        glfwTerminate();

        return -1;
    }

    //--------------------------------------------------
    // Store the initial timestamp.
    //
    // This value is used to calculate delta time.
    //--------------------------------------------------

    float lastTime =
        static_cast<float>(
            glfwGetTime());

    //--------------------------------------------------
    // Main Game Loop.
    //
    // The game runs continuously until the window
    // is closed by the player.
    //--------------------------------------------------

    while (
        !glfwWindowShouldClose(
            window))
    {
        //--------------------------------------------------
        // Get current time.
        //--------------------------------------------------

        float currentTime =
            static_cast<float>(
                glfwGetTime());

        //--------------------------------------------------
        // Compute frame delta time.
        //
        // Delta time allows movement and gameplay
        // speed to remain independent of frame rate.
        //--------------------------------------------------

        float deltaTime =
            currentTime -
            lastTime;

        //--------------------------------------------------
        // Store current time for the next frame.
        //--------------------------------------------------

        lastTime =
            currentTime;

        //--------------------------------------------------
        // Allow the player to exit the game by
        // pressing the ESC key.
        //--------------------------------------------------

        if (
            glfwGetKey(
                window,
                GLFW_KEY_ESCAPE)
            ==
            GLFW_PRESS)
        {
            glfwSetWindowShouldClose(
                window,
                GLFW_TRUE);
        }

        //--------------------------------------------------
        // Process keyboard input.
        //
        // Handles:
        //
        //  • Movement
        //  • Rotation
        //  • Shooting
        //--------------------------------------------------

        game.ProcessInput(
            deltaTime);

        //--------------------------------------------------
        // Update all game systems.
        //
        // Includes:
        //
        //  • Player update
        //  • Enemy AI
        //  • Projectile movement
        //  • Collision detection
        //  • Gameplay logic
        //--------------------------------------------------

        game.Update(
            deltaTime);

        //--------------------------------------------------
        // Render the current frame.
        //
        // Draw order:
        //
        //  1. Map
        //  2. Player
        //  3. Enemies
        //  4. Projectiles
        //--------------------------------------------------

        game.Render();

        //--------------------------------------------------
        // Swap front and back buffers.
        //
        // Double buffering prevents flickering
        // and provides smooth rendering.
        //--------------------------------------------------

        glfwSwapBuffers(
            window);

        //--------------------------------------------------
        // Process operating system events.
        //
        // Examples:
        //
        //  • Keyboard input
        //  • Window movement
        //  • Window resize
        //  • Close requests
        //--------------------------------------------------

        glfwPollEvents();
    }

    //--------------------------------------------------
    // Shutdown game systems and release resources.
    //--------------------------------------------------

    game.Shutdown();

    //--------------------------------------------------
    // Destroy the application window.
    //--------------------------------------------------

    glfwDestroyWindow(
        window);

    //--------------------------------------------------
    // Shutdown GLFW.
    //--------------------------------------------------

    glfwTerminate();

    //--------------------------------------------------
    // Exit successfully.
    //--------------------------------------------------

    return 0;
}
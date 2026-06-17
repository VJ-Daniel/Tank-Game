/*
    ============================================================
    Shader.cpp

    Author: Leonardo Moura
    Date: 6/12/2026

    Description:

    Implements the Shader class responsible for:

        • Loading shader source files
        • Compiling vertex shaders
        • Compiling fragment shaders
        • Linking shader programs
        • Activating shader programs
        • Reporting compilation errors

    Shader Pipeline:

        Vertex Shader
              ↓
        Fragment Shader
              ↓
        Program Linking
              ↓
        GPU Execution

    This class provides a simple interface for
    creating and using OpenGL shader programs.

    ============================================================
*/

#include "Shader.h"

#include <glew.h>

#include <fstream>
#include <sstream>
#include <iostream>
#include <string>

/*
    Constructor

    Loads, compiles and links a complete
    OpenGL shader program.

    Parameters:

        vertexPath
            Path to vertex shader source file

        fragmentPath
            Path to fragment shader source file

    Steps:

        1. Load source files
        2. Compile vertex shader
        3. Compile fragment shader
        4. Link shader program
        5. Delete temporary shader objects
*/
Shader::Shader(
    const char* vertexPath,
    const char* fragmentPath)
{
    //--------------------------------------------------
    // Open shader source files.
    //--------------------------------------------------

    std::ifstream vertexFile(
        vertexPath);

    std::ifstream fragmentFile(
        fragmentPath);

    //--------------------------------------------------
    // Verify vertex shader file.
    //--------------------------------------------------

    if (!vertexFile.is_open())
    {
        std::cout
            << "ERROR OPENING "
            << vertexPath
            << std::endl;
    }

    //--------------------------------------------------
    // Verify fragment shader file.
    //--------------------------------------------------

    if (!fragmentFile.is_open())
    {
        std::cout
            << "ERROR OPENING "
            << fragmentPath
            << std::endl;
    }

    //--------------------------------------------------
    // Read shader files into memory.
    //--------------------------------------------------

    std::stringstream vertexStream;
    std::stringstream fragmentStream;

    vertexStream
        << vertexFile.rdbuf();

    fragmentStream
        << fragmentFile.rdbuf();

    //--------------------------------------------------
    // Convert streams into strings.
    //--------------------------------------------------

    std::string vertexCode =
        vertexStream.str();

    std::string fragmentCode =
        fragmentStream.str();

    //--------------------------------------------------
    // Convert strings into C-style strings
    // required by OpenGL.
    //--------------------------------------------------

    const char* vertexSource =
        vertexCode.c_str();

    const char* fragmentSource =
        fragmentCode.c_str();

    //--------------------------------------------------
    // Variables used during compilation
    // and linking validation.
    //--------------------------------------------------

    GLint success;

    GLchar infoLog[1024];

    //--------------------------------------------------
    // Vertex Shader Compilation
    //
    // The vertex shader processes each vertex
    // before rasterization.
    //
    // Responsibilities:
    //
    //      • Position transformations
    //      • MVP matrix application
    //      • Vertex attribute processing
    //--------------------------------------------------

    GLuint vertexShader =
        glCreateShader(
            GL_VERTEX_SHADER);

    //--------------------------------------------------
    // Provide source code to OpenGL.
    //--------------------------------------------------

    glShaderSource(
        vertexShader,
        1,
        &vertexSource,
        nullptr);

    //--------------------------------------------------
    // Compile vertex shader.
    //--------------------------------------------------

    glCompileShader(
        vertexShader);

    //--------------------------------------------------
    // Check compilation status.
    //--------------------------------------------------

    glGetShaderiv(
        vertexShader,
        GL_COMPILE_STATUS,
        &success);

    //--------------------------------------------------
    // Display compilation errors if any exist.
    //--------------------------------------------------

    if (!success)
    {
        glGetShaderInfoLog(
            vertexShader,
            sizeof(infoLog),
            nullptr,
            infoLog);

        std::cout
            << "VERTEX SHADER ERROR:"
            << std::endl
            << infoLog
            << std::endl;
    }

    //--------------------------------------------------
    // Fragment Shader Compilation
    //
    // The fragment shader determines the final
    // color of each rendered pixel.
    //
    // Responsibilities:
    //
    //      • Color output
    //      • Lighting calculations
    //      • Transparency handling
    //--------------------------------------------------

    GLuint fragmentShader =
        glCreateShader(
            GL_FRAGMENT_SHADER);

    //--------------------------------------------------
    // Provide source code to OpenGL.
    //--------------------------------------------------

    glShaderSource(
        fragmentShader,
        1,
        &fragmentSource,
        nullptr);

    //--------------------------------------------------
    // Compile fragment shader.
    //--------------------------------------------------

    glCompileShader(
        fragmentShader);

    //--------------------------------------------------
    // Check compilation status.
    //--------------------------------------------------

    glGetShaderiv(
        fragmentShader,
        GL_COMPILE_STATUS,
        &success);

    //--------------------------------------------------
    // Display compilation errors if any exist.
    //--------------------------------------------------

    if (!success)
    {
        glGetShaderInfoLog(
            fragmentShader,
            sizeof(infoLog),
            nullptr,
            infoLog);

        std::cout
            << "FRAGMENT SHADER ERROR:"
            << std::endl
            << infoLog
            << std::endl;
    }

    //--------------------------------------------------
    // Program Creation
    //
    // Combine vertex and fragment shaders
    // into a complete executable GPU program.
    //--------------------------------------------------

    programID =
        glCreateProgram();

    //--------------------------------------------------
    // Attach vertex shader.
    //--------------------------------------------------

    glAttachShader(
        programID,
        vertexShader);

    //--------------------------------------------------
    // Attach fragment shader.
    //--------------------------------------------------

    glAttachShader(
        programID,
        fragmentShader);

    //--------------------------------------------------
    // Link shader program.
    //
    // This verifies compatibility between
    // vertex and fragment stages.
    //--------------------------------------------------

    glLinkProgram(
        programID);

    //--------------------------------------------------
    // Check link status.
    //--------------------------------------------------

    glGetProgramiv(
        programID,
        GL_LINK_STATUS,
        &success);

    //--------------------------------------------------
    // Display linker errors if they exist.
    //--------------------------------------------------

    if (!success)
    {
        glGetProgramInfoLog(
            programID,
            sizeof(infoLog),
            nullptr,
            infoLog);

        std::cout
            << "PROGRAM LINK ERROR:"
            << std::endl
            << infoLog
            << std::endl;
    }

    //--------------------------------------------------
    // Shader objects are no longer needed
    // after successful linking.
    //
    // The program now owns the compiled code.
    //--------------------------------------------------

    glDeleteShader(
        vertexShader);

    glDeleteShader(
        fragmentShader);
}

/*
    Activate Shader Program

    Makes this shader program the currently
    active program used by OpenGL.

    All subsequent rendering operations will
    use this program until another shader is
    activated.
*/
void Shader::Use()
{
    glUseProgram(
        programID);
}

/*
    Returns the OpenGL program identifier.

    Used when:

        • Setting uniforms
        • Querying locations
        • Debugging OpenGL state
*/
unsigned int Shader::GetID() const
{
    return programID;
}
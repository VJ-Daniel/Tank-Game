/*
    ============================================================
    Shader.h

    Author: Leonardo Moura
    Date: 6/12/2026

    Description:

    Defines the Shader class used to manage OpenGL
    shader programs.

    Responsibilities:

        • Load shader source files
        • Compile vertex shaders
        • Compile fragment shaders
        • Link shader programs
        • Activate shader programs
        • Provide access to OpenGL program IDs

    OpenGL Shader Pipeline:

        Vertex Shader
              ↓
        Primitive Assembly
              ↓
        Rasterization
              ↓
        Fragment Shader
              ↓
        Framebuffer
              ↓
           Screen

    This class encapsulates the creation and
    management of a complete OpenGL shader
    program.

    ============================================================
*/

#ifndef SHADER_H
#define SHADER_H

class Shader
{
private:

    //--------------------------------------------------
    // OpenGL Program ID
    //
    // Represents the linked shader program
    // stored on the GPU.
    //
    // This program contains:
    //
    //      • Vertex Shader
    //      • Fragment Shader
    //
    // Used by OpenGL during rendering.
    //--------------------------------------------------

    unsigned int programID;

public:

    //--------------------------------------------------
    // Constructor
    //
    // Loads, compiles and links a complete
    // shader program.
    //
    // Parameters:
    //
    //      vertexPath
    //          Path to vertex shader file.
    //
    //      fragmentPath
    //          Path to fragment shader file.
    //
    // Responsibilities:
    //
    //      • Read shader files
    //      • Compile shaders
    //      • Link program
    //      • Validate program
    //--------------------------------------------------

    Shader(
        const char* vertexPath,
        const char* fragmentPath);

    //--------------------------------------------------
    // Activate Shader
    //
    // Makes this shader program the currently
    // active OpenGL program.
    //
    // All rendering performed after calling
    // this function will use this shader until
    // another program is activated.
    //--------------------------------------------------

    void Use();

    //--------------------------------------------------
    // Program Accessor
    //
    // Returns the OpenGL program identifier.
    //
    // Used for:
    //
    //      • Uniform lookups
    //      • OpenGL queries
    //      • Debugging
    //      • Renderer integration
    //--------------------------------------------------

    unsigned int GetID() const;
};

#endif
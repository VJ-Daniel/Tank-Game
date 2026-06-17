/*
    ============================================================
    Mesh.cpp

    Author: Leonardo Moura
    Date: 6/12/2026

    Description:

    Implements the Mesh class responsible for storing
    and rendering OpenGL geometry.

    Responsibilities:

        • Create Vertex Array Objects (VAO)
        • Create Vertex Buffer Objects (VBO)
        • Upload vertex data to the GPU
        • Configure vertex attributes
        • Render geometry
        • Release GPU resources

    Vertex Layout:

        Position:
            x, y, z

        Color:
            r, g, b

    Total Floats Per Vertex:

        6 floats

    This class is used by the renderer to draw
    all game objects including:

        • Player tank
        • Enemy tanks
        • Bullets
        • Map blocks

    ============================================================
*/

#include "Mesh.h"

#include <glew.h>

/*
    Constructor

    Creates GPU resources and uploads vertex data.

    Steps:

        1. Calculate vertex count
        2. Create VAO
        3. Create VBO
        4. Upload vertex data
        5. Configure vertex attributes

    Vertex Format:

        Position:
            x, y, z

        Color:
            r, g, b
*/
Mesh::Mesh(
    const std::vector<float>& vertices)
{
    //--------------------------------------------------
    // Determine how many vertices exist in the mesh.
    //
    // Each vertex contains:
    //
    //      x y z r g b
    //
    // Total:
    //      6 floats per vertex
    //--------------------------------------------------

    vertexCount =
        static_cast<unsigned int>(
            vertices.size() / 6);

    //--------------------------------------------------
    // Create Vertex Array Object.
    //
    // The VAO stores all vertex attribute
    // configurations for this mesh.
    //--------------------------------------------------

    glGenVertexArrays(
        1,
        &VAO);

    //--------------------------------------------------
    // Create Vertex Buffer Object.
    //
    // The VBO stores the actual vertex data
    // inside GPU memory.
    //--------------------------------------------------

    glGenBuffers(
        1,
        &VBO);

    //--------------------------------------------------
    // Bind the VAO so subsequent configuration
    // commands are stored inside it.
    //--------------------------------------------------

    glBindVertexArray(
        VAO);

    //--------------------------------------------------
    // Bind the VBO as the active vertex buffer.
    //--------------------------------------------------

    glBindBuffer(
        GL_ARRAY_BUFFER,
        VBO);

    //--------------------------------------------------
    // Upload vertex data from CPU memory
    // to GPU memory.
    //
    // GL_STATIC_DRAW indicates that the data
    // will rarely change after creation.
    //--------------------------------------------------

    glBufferData(
        GL_ARRAY_BUFFER,
        vertices.size()
        * sizeof(float),

        vertices.data(),

        GL_STATIC_DRAW);

    //--------------------------------------------------
    // Vertex Attribute 0
    //
    // Position:
    //
    //      x
    //      y
    //      z
    //
    // Layout Location:
    //      0
    //--------------------------------------------------

    glVertexAttribPointer(
        0,
        3,
        GL_FLOAT,
        GL_FALSE,
        6 * sizeof(float),
        (void*)0);

    //--------------------------------------------------
    // Enable Position Attribute.
    //--------------------------------------------------

    glEnableVertexAttribArray(
        0);

    //--------------------------------------------------
    // Vertex Attribute 1
    //
    // Color:
    //
    //      r
    //      g
    //      b
    //
    // Layout Location:
    //      1
    //
    // Offset:
    //      Skip first 3 floats
    //      (x,y,z)
    //--------------------------------------------------

    glVertexAttribPointer(
        1,
        3,
        GL_FLOAT,
        GL_FALSE,
        6 * sizeof(float),
        (void*)(3 * sizeof(float)));

    //--------------------------------------------------
    // Enable Color Attribute.
    //--------------------------------------------------

    glEnableVertexAttribArray(
        1);

    //--------------------------------------------------
    // Unbind VAO.
    //
    // Prevents accidental modification by
    // later OpenGL calls.
    //--------------------------------------------------

    glBindVertexArray(
        0);
}

/*
    Draw

    Renders the mesh using the currently
    active shader program.

    Rendering Mode:

        GL_TRIANGLES

    Every group of three vertices forms
    a triangle.
*/
void Mesh::Draw() const
{
    //--------------------------------------------------
    // Activate the mesh VAO.
    //--------------------------------------------------

    glBindVertexArray(
        VAO);

    //--------------------------------------------------
    // Render all vertices as triangles.
    //--------------------------------------------------

    glDrawArrays(
        GL_TRIANGLES,
        0,
        vertexCount);

    //--------------------------------------------------
    // Unbind VAO.
    //--------------------------------------------------

    glBindVertexArray(
        0);
}

/*
    Destructor

    Releases GPU resources allocated by
    this mesh.

    Resources Released:

        • VAO
        • VBO

    This prevents GPU memory leaks.
*/
Mesh::~Mesh()
{
    //--------------------------------------------------
    // Delete Vertex Array Object.
    //--------------------------------------------------

    glDeleteVertexArrays(
        1,
        &VAO);

    //--------------------------------------------------
    // Delete Vertex Buffer Object.
    //--------------------------------------------------

    glDeleteBuffers(
        1,
        &VBO);
}
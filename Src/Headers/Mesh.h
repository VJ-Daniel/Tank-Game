/*
    ============================================================
    Mesh.h

    Author: Leonardo Moura
    Date: 6/12/2026

    Description:

    Defines the Mesh class used by the rendering
    system to store and draw OpenGL geometry.

    Responsibilities:

        • Store GPU geometry resources
        • Upload vertex data
        • Manage VAO and VBO objects
        • Render geometry
        • Release GPU resources

    OpenGL Objects:

        VAO
            Vertex Array Object

        VBO
            Vertex Buffer Object

    Vertex Format:

        Position:
            x, y, z

        Color:
            r, g, b

    Total:

        6 floats per vertex

    Used By:

        • Player Tank
        • Enemy Tanks
        • Bullets
        • Breakable Blocks
        • Steel Blocks

    Rendering Flow:

        CPU Vertex Data
                ↓
             Mesh
                ↓
              VBO
                ↓
              VAO
                ↓
          glDrawArrays()
                ↓
             Screen

    ============================================================
*/

#ifndef MESH_H
#define MESH_H

#include <vector>

class Mesh
{
private:

    //--------------------------------------------------
    // Vertex Array Object
    //
    // Stores vertex attribute configuration.
    //
    // The VAO remembers:
    //
    //      • Attribute locations
    //      • Buffer bindings
    //      • Vertex layout
    //--------------------------------------------------

    unsigned int VAO;

    //--------------------------------------------------
    // Vertex Buffer Object
    //
    // Stores vertex data inside GPU memory.
    //
    // Contains:
    //
    //      Position Data
    //      Color Data
    //--------------------------------------------------

    unsigned int VBO;

    //--------------------------------------------------
    // Vertex Count
    //
    // Total number of vertices contained
    // inside the mesh.
    //
    // Used when rendering with:
    //
    //      glDrawArrays()
    //--------------------------------------------------

    unsigned int vertexCount;

public:

    //--------------------------------------------------
    // Constructor
    //
    // Creates a mesh from vertex data.
    //
    // Responsibilities:
    //
    //      • Create VAO
    //      • Create VBO
    //      • Upload vertex data
    //      • Configure attributes
    //
    // Vertex Format:
    //
    //      x y z r g b
    //--------------------------------------------------

    Mesh(
        const std::vector<float>& vertices);

    //--------------------------------------------------
    // Draw
    //
    // Renders the mesh using the currently
    // active shader program.
    //
    // Rendering Mode:
    //
    //      GL_TRIANGLES
    //--------------------------------------------------

    void Draw() const;

    //--------------------------------------------------
    // Destructor
    //
    // Releases GPU resources.
    //
    // Resources Released:
    //
    //      • VAO
    //      • VBO
    //--------------------------------------------------

    ~Mesh();
};

#endif
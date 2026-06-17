#version 330 core

/*
    ============================================================
    Vertex Shader

    Purpose:

    This shader processes every vertex sent to the GPU.

    It is responsible for transforming object-space
    coordinates into clip-space coordinates so OpenGL
    knows where to draw the geometry on screen.

    Rendering Pipeline:

        CPU Vertex Data
              ↓
        Vertex Shader  ← You are here
              ↓
        Rasterization
              ↓
        Fragment Shader
              ↓
           Screen

    Vertex Layout:

        Location 0:
            Position (x, y, z)

        Location 1:
            Color (r, g, b)

    Uniforms:

        MVP Matrix

            MVP =
                Projection
                *
                View
                *
                Model

            This matrix transforms vertices from
            local object space into screen space.

    Outputs:

        vertexColor

            Passed to the Fragment Shader.

            OpenGL automatically interpolates the
            color between vertices across the
            triangle surface.

    Current Behavior:

        • Applies MVP transformation
        • Passes vertex color to Fragment Shader
        • No lighting calculations
        • No texture coordinates
        • No normal vectors

    ============================================================
*/

//--------------------------------------------------
// Vertex Attributes
//
// Data received from the currently bound VBO.
//--------------------------------------------------

/*
    Vertex Position

    Layout Location:
        0

    Format:
        x, y, z

    Example:

        100.0
        200.0
        0.0
*/
layout(location = 0)
in vec3 aPos;

/*
    Vertex Color

    Layout Location:
        1

    Format:
        r, g, b

    Example:

        1.0
        0.0
        0.0

    Represents red.
*/
layout(location = 1)
in vec3 aColor;

//--------------------------------------------------
// Outputs
//
// Sent to the Fragment Shader.
//--------------------------------------------------

/*
    Interpolated color.

    OpenGL automatically blends this value
    across triangle surfaces.
*/
out vec3 vertexColor;

//--------------------------------------------------
// Uniforms
//--------------------------------------------------

/*
    Model View Projection Matrix

    Combines:

        • Model Transform
        • View Transform
        • Projection Transform

    Used to convert local mesh coordinates
    into clip-space coordinates.
*/
uniform mat4 mvp;

//--------------------------------------------------
// Main
//
// Executes once for every vertex.
//--------------------------------------------------

void main()
{
    //--------------------------------------------------
    // Transform vertex position into clip space.
    //
    // Formula:
    //
    //      ClipPosition =
    //          MVP *
    //          LocalPosition
    //--------------------------------------------------

    gl_Position =
        mvp *
        vec4(
            aPos,
            1.0);

    //--------------------------------------------------
    // Pass vertex color to the Fragment Shader.
    //
    // This value will be interpolated across
    // the rendered primitive.
    //--------------------------------------------------

    vertexColor =
        aColor;
}
#version 330 core

/*
    ============================================================
    Fragment Shader

    Purpose:

    This shader determines the final color of every
    pixel generated from the rendered geometry.

    Rendering Pipeline:

        Vertex Data
              ↓
        Vertex Shader
              ↓
        Rasterization
              ↓
        Fragment Shader  ← You are here
              ↓
        Final Pixel Color
              ↓
           Screen

    Input:

        vertexColor

            Interpolated color received from the
            Vertex Shader.

            OpenGL automatically blends the color
            values across the surface of triangles.

    Output:

        FragColor

            Final RGBA color written into the
            framebuffer.

    Current Behavior:

        • Uses the interpolated vertex color
        • Forces full opacity (alpha = 1.0)
        • No lighting calculations
        • No textures
        • No transparency effects

    Example:

        Vertex 1 = Red
        Vertex 2 = Green
        Vertex 3 = Blue

        OpenGL interpolates colors between vertices,
        creating smooth color transitions across
        the triangle surface.

    ============================================================
*/

//--------------------------------------------------
// Input from Vertex Shader
//
// Interpolated color for the current fragment.
//--------------------------------------------------

in vec3 vertexColor;

//--------------------------------------------------
// Final Output Color
//
// Written to the framebuffer.
//--------------------------------------------------

out vec4 FragColor;

//--------------------------------------------------
// Main
//
// Executes once for every generated fragment
// (potential pixel).
//--------------------------------------------------

void main()
{
    //--------------------------------------------------
    // Output the interpolated RGB color.
    //
    // Alpha is fixed at 1.0 (fully opaque).
    //--------------------------------------------------

    FragColor =
        vec4(
            vertexColor,
            1.0);
}
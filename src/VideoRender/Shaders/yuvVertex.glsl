#version 410 core

in vec4 aVertex; // vertex coordinates
in vec2 aTexCoord; // texture coordinates of the vertex
uniform mat4 qt_Matrix; // transformation matrix
out vec2 vTexCoord; // texture coordinates passed to the fragment shader

void main() 
{                           
    gl_Position = qt_Matrix * aVertex;
    vTexCoord = aTexCoord; 
}
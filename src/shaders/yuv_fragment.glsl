#version 410 core

// Input from vertex shader
in vec2 vTexCoord;

// Output to framebuffer
out vec4 FragColor;

// Uniforms (samplers)
uniform sampler2DRect s_texture_y;
uniform sampler2DRect s_texture_u;
uniform sampler2DRect s_texture_v;

void main() {
    float Y = texture(s_texture_y, vTexCoord).r;
    float U = texture(s_texture_u, vTexCoord / 2.0).r - 0.5;
    float V = texture(s_texture_v, vTexCoord / 2.0).r - 0.5;

    float R = Y + 1.402 * V;
    float G = Y - 0.344 * U - 0.714 * V;
    float B = Y + 1.772 * U;

    FragColor = vec4(R, G, B, 1.0);
}

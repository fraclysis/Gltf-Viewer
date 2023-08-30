#version 330 core
out vec4 FragColor;

in vec2 UV;
uniform sampler2D lightTexture;

void main()
{
    FragColor = texture(lightTexture, UV);
}
#version 330 core
layout(location = 0) in vec3 POSITION;
layout(location = 1) in vec2 TEXCOORD_0;

out vec2 UV;

uniform mat4 mvp[4];

void main()
{
    UV = TEXCOORD_0;
    vec4 pos = mvp[gl_InstanceID] * vec4(POSITION, 1.0);
    gl_Position = pos;
}
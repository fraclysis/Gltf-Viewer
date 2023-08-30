#version 450 core

layout(location = 0) in vec3 POSITION;
layout(location = 1) in vec3 NORMAL;
layout(location = 2) in vec3 TANGENT;
layout(location = 3) in vec2 TEXCOORD_0;
layout(location = 4) in vec2 TEXCOORD_1;
layout(location = 5) in vec4 COLOR_0;
layout(location = 6) in vec4 JOINTS_0;
layout(location = 7) in vec4 WEIGHTS_0;

out vec4 fCol;
out vec3 fPos;
out vec3 fNorm;
out vec2 fTEXCOORD_0;

uniform mat4 model;
uniform mat4 viewProjection;

uniform vec3 emissiveFactor;
uniform sampler2D normalTexture;
uniform sampler2D occlusionTexture;
uniform sampler2D emissiveTexture;

uniform vec4 baseColorFactor;
uniform sampler2D baseColorTexture;
uniform float metallicFactor;
uniform float roughnessFactor;
uniform sampler2D metallicRoughnessTexture;

uniform int jointMatSize;
uniform mat4 jointMat[200];

void main()
{
    vec4 pos;
    if (jointMatSize > 0) {
        mat4 skinMat =
            WEIGHTS_0.x * jointMat[int(JOINTS_0.x)] +
            WEIGHTS_0.y * jointMat[int(JOINTS_0.y)] +
            WEIGHTS_0.z * jointMat[int(JOINTS_0.z)] +
            WEIGHTS_0.w * jointMat[int(JOINTS_0.w)];

        pos = model * skinMat * vec4(POSITION, 1.0);
        fNorm = normalize(transpose(inverse(mat3(model * skinMat))) * NORMAL);
    } else {
        pos = model * vec4(POSITION, 1.0);
        fNorm = normalize(transpose(inverse(mat3(model))) * NORMAL);
    }

    fPos = pos.xyz;
    fTEXCOORD_0 = TEXCOORD_0;

    gl_Position = viewProjection * pos;
}
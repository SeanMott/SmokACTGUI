#version 460
#extension GL_EXT_nonuniform_qualifier : require

layout (location = 0) in vec3 Position;
layout (location = 1) in vec3 Normal;
layout (location = 2) in vec2 TextureCords;
layout (location = 3) in vec4 Color;
layout (location = 4) in vec4 Metadata;

layout(location = 0) out vec4 outColor;

//camera uniform buffer
layout(binding = 0) uniform CameraBuffer 
{
	mat4 P[4]; //the projection matrix
    mat4 V[4]; //the view matrix
	mat4 PV[4]; //the projection * view matrix
} cameraBuffer;

layout(set = 2, binding = 0) uniform sampler2D textures[];

void main() {
    outColor = texture(textures[int(Metadata.y)], TextureCords) * Color;
}
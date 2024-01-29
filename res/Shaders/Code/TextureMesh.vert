#version 460
#extension GL_EXT_nonuniform_qualifier : require

layout (location = 0) in vec3 position;
layout (location = 1) in vec3 normal;
layout (location = 2) in vec2 textureCords;
layout (location = 3) in vec4 color;

layout (location = 0) out vec3 Position;
layout (location = 1) out vec3 Normal;
layout (location = 2) out vec2 TextureCords;
layout (location = 3) out vec4 Color;
layout (location = 4) out vec4 Metadata;

//camera uniform buffer
layout(binding = 0) uniform CameraBuffer 
{
	mat4 P[4]; //the projection matrix
    mat4 V[4]; //the view matrix
	mat4 PV[4]; //the projection * view matrix
} cameraBuffer;

struct ObjectData
{
	mat4 M; //model

	vec4 metadata;
	/*
	x = camera index
	y = texture index
	z = not used
	w = not used
	*/
};

//all object matrices
layout(std140,set = 1, binding = 0) readonly buffer ObjectBuffer{

	ObjectData objects[];
} objectBuffer;

void main() {

	//calculates the PVM
	uint camIndex = int(objectBuffer.objects[gl_BaseInstance].metadata.x);
    gl_Position = cameraBuffer.PV[camIndex] * objectBuffer.objects[gl_BaseInstance].M * vec4(position, 1.0);
    
	//pass data to fragment
	Position = position;
	Normal = normal;
	TextureCords = textureCords;
	Color = color;
	Metadata = objectBuffer.objects[gl_BaseInstance].metadata;
}
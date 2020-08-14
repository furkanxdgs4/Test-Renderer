#version 460
layout(location = 0) in vec3 Vertex_Pos;
layout(location = 1) in vec2 TextCoord;
layout(location = 2) in vec3 Vertex_Nor;
layout(location = 3) in vec3 Tangents;
layout(location = 4) in vec3 Bitangents;

out vec2 TextureCoords;
out vec3 VertexNormals;
out vec3 Tangent;
out vec3 Bitangent;
out vec3 FragPos;

#define MAX_WORLDOBJECTNUMBER 1000

layout (std430) buffer VIEW_MATRICES{
	mat4 PROJ_MATRIX;
	mat4 VIEW_MATRIX;
};

layout (std430) buffer MODEL_MATRICES{
	mat4 MODEL_MATRICes[MAX_WORLDOBJECTNUMBER];
};

//Each shader instance will only specify this for vertex shader!
uniform uint OBJECT_INDEX;

void main(){
	gl_Position = PROJ_MATRIX * VIEW_MATRIX * MODEL_MATRICes[OBJECT_INDEX] * vec4(Vertex_Pos, 1.0f);
	TextureCoords = TextCoord;
	VertexNormals = Vertex_Nor;
	Tangent = Tangents;
	Bitangent = Bitangents;
}
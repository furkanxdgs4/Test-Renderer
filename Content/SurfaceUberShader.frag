#version 460 core
in vec2 TextureCoords;
in vec3 VertexNormals;
in vec3 Tangent;
in vec3 Bitangent;
out vec4 Fragment_Color;
#define MAX_MATINSTNUMBER 1000
#define MAX_SPOTLIGHTs 10
#define MAX_POINTLIGHTs 10
#define MAX_DIRECTIONALLIGHTs 1

struct PointLight{
	vec3 POSITION;
};

struct SpotLight{
	vec3 POSITION;
};

struct DirectionalLight{
	vec3 DIRECTION;
	vec3 COLOR;
};

layout (std430) buffer LIGHTs{
	DirectionalLight DIRECTIONALLIGHTs[MAX_DIRECTIONALLIGHTs];
	PointLight POINTLIGHTs[MAX_POINTLIGHTs];
	SpotLight SPOTLIGHTs[MAX_SPOTLIGHTs];
	uint DIRECTIONALLIGHTs_COUNT, POINTLIGHTs_COUNT, SPOTLIGHTs_COUNT;
};

layout(binding = 0) uniform sampler2D DIFFUSETEXTURE; 
layout(binding = 1) uniform sampler2D NORMALSTEXTURE; 
layout(binding = 2) uniform sampler2D SPECULARTEXTURE; 
layout(binding = 3) uniform sampler2D OPACITYTEXTURE;
uniform uint MATINST_INDEX;

void main(){
	vec4 DIFFUSE = texture(DIFFUSETEXTURE, TextureCoords);
	//vec4 DIFFUSE = vec4((VertexNormals + 1) / 2, 1.0f);
	Fragment_Color = vec4(DIFFUSE.rgb,1.0f);
}
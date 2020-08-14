#version 460
out vec4 Fragment_Color;
layout (depth_less) out float gl_FragDepth;
#define MAX_MATINSTNUMBER 1000
#define MAX_SPOTLIGHTs 10
#define MAX_POINTLIGHTs 10
#define MAX_DIRECTIONALLIGHTs 1


uniform vec3 POINTCOLOR;
uniform uint POINTLIGHT_INDEX, SPOTLIGHT_INDEX;

void main(){
	if(POINTLIGHT_INDEX > 0){
		Fragment_Color = vec4(0,1,0,1);
	}
	else if(SPOTLIGHT_INDEX > 0){
		Fragment_Color = vec4(1,0,0,1);
	}
	else{
		Fragment_Color = vec4(POINTCOLOR, 1.0f);
		gl_FragDepth = gl_FragCoord.z - 0.0001f;
	}
}
#pragma once
#include "GFX/GFX_Includes.h"
#include "GFX/GFX_FileSystem/Resource_Type/Texture_Resource.h"

/*
	All resources have GL_ID variable. This variable is a pointer to GL specific structure that's used by GL functions (For example; unsigned ints for Textures, Buffers etc. in OpenGL)
	
*/



namespace GFX_API {
	//Create Render Targets as textures
	struct GFXAPI RenderTarget {
	public:
		TEXTURE_DIMENSIONs DIMENSION;
		TEXTURE_TYPEs FORMAT;
		UNIFORMTYPE FORMAT_VALUETYPE;
		unsigned int ID, WIDTH, HEIGHT;
		bool Usable_as_Texture;
		void* GL_ID;
	};

	struct GFXAPI Framebuffer {
		struct RT_SLOT {
			unsigned int RT_ID, WIDTH, HEIGTH;
			RT_ATTACHMENTs ATTACHMENT_TYPE;
			OPERATION_TYPE RT_OPERATIONTYPE;
			RT_READSTATE RT_READTYPE;
			vec3 CLEAR_COLOR;
		};
	public:
		unsigned int ID;
		void* GL_ID;
		vector<RT_SLOT> BOUND_RTs;

		Framebuffer();
	};

	//Please read GFX_Point if you use this with any Point Buffer
	struct GFXAPI GFX_Mesh {
		unsigned int BUFFER_ID, VERTEX_COUNT, INDEX_COUNT;
		void* GL_ID;	//In OpenGL4 we have to use VAO, VBO and EBO so this points to OGL4_MESH structure that holds all of these. 
						//I still don't know about buffers in Vulkan, so that's all for now
	};

	//There is a one-way relationship between GFX_Point and GFX_Mesh
	//If Point Buffer uses first vertex attribute of Mesh Buffer, point buffer uses the GFX_Mesh's GL_ID to point at the buffer in GPU-side.
	//That means if you unload the GFX_Mesh that Point Buffer uses its first vertex attribute, you shouldn't draw GFX_Point. Otherwise undefined behaviour.
	//If you use any other vertex attribute of GFX_Mesh, we re-upload the data to GPU. That means even if you delete GFX_Mesh, you can draw GFX_Point without any issue.
	struct GFXAPI GFX_Point {
		unsigned int BUFFER_ID, POINT_COUNT;
		void* GL_ID;
	};

	struct GFXAPI GFX_Texture {
		unsigned int ASSET_ID;
		void* GL_ID;
	};

	struct GFXAPI GFX_Buffer {
		string NAME;		//GLSL uses shader name reflection to get Buffer, so this is the name used in GLSL
		unsigned int ID;
		void* DATA,
			*GL_ID,			//Buffer's ID to access
			*BINDING_POINT; //Shaders uses binding point to access the buffer, so store it.
		unsigned int DATA_SIZE;
		GLOBALBUFFER_USAGE USAGE;
	};




	//This represents Rasterization Shader Stages, not Compute
	struct GFXAPI GFX_ShaderSource {
		unsigned int ASSET_ID;
		void* GL_ID;
	};

	struct GFXAPI GFX_ShaderProgram {
		unsigned int ASSET_ID;
		void* GL_ID;
	};

	struct GFXAPI GFX_ComputeShader {
		unsigned int ASSET_ID;
		void* GL_ID;
	};

	struct GFXAPI RenderState {
		DEPTH_MODEs DEPTH_MODE;
		DEPTH_TESTs DEPTH_TEST_FUNC;
		//There should be Stencil and Blending options, but for now leave it like this
	};
}
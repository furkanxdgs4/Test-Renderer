#pragma once
#include "GFX/GFX_Includes.h"
#include "GFX/GFX_ENUMs.h"

enum class RESOURCETYPEs : char;

namespace GFX_API {
	struct GFXAPI Material_Uniform {
		string VARIABLE_NAME;
		UNIFORMTYPE VARIABLE_TYPE;
		void* DATA = nullptr;
		Material_Uniform(const char* variable_name, UNIFORMTYPE variable_type);
		bool Verify_UniformData();
		Material_Uniform();
	};


	//This class doesn't support Compute, only the Rasterization stages
	class GFXAPI ShaderSource_Resource {
	public:
		ShaderSource_Resource();
		SHADER_STAGE STAGE;
		string SOURCE_CODE;
		SHADER_LANGUAGEs LANGUAGE;
	};
	
	enum class TEXTURE_ACCESS : unsigned char {
		SAMPLER_OPERATION,	//OP_TYPE = READ_ONLY is neccessary
		IMAGE_OPERATION,	//Any OP_TYPE is available
		FRAGMENT_OUTPUT		//OP_TYPE should be WRITE_ONLY or READ_WRITE
	};

	struct GFXAPI Texture_Access {
		TEXTURE_DIMENSIONs DIMENSIONs;
		TEXTURE_CHANNELs CHANNELs;
		OPERATION_TYPE OP_TYPE;
		TEXTURE_ACCESS ACCESS_TYPE;		//Don't care if OP_TYPE is READ_ONLY
		unsigned int BINDING_POINT;		
		unsigned int TEXTURE_ID;		//Care if OP_TYPE is READ_ONLY or WRITE_ACCESS is IMAGE_OPERATION
	};

	struct GFXAPI GlobalBuffer_Access {
		unsigned int BUFFER_ID;
		OPERATION_TYPE ACCESS_TYPE;
	};

	class GFXAPI Material_Type {
	public:
		Material_Type();

		unsigned int VERTEXSOURCE_ID, FRAGMENTSOURCE_ID;

		vector<Texture_Access> TEXTUREs;
		vector<Material_Uniform> UNIFORMs;
		vector<GlobalBuffer_Access> GLOBALBUFFERs;
	};
	
	class GFXAPI ComputeShader_Resource {
	public:
		SHADER_LANGUAGEs LANGUAGE;
		string SOURCE_CODE;
		vector<Material_Uniform> UNIFORMs;
		vector<unsigned int> GLOBALBUFFER_IDs;
	};


	//Don't forget, data handling for each uniform type is the responsibility of the user!
	class GFXAPI Material_Instance {
	public:
		Material_Instance();
		
		//Uniforms won't change at run-time because we are defining uniforms at compile-time, but it is an easier syntax for now!
		//This list will be defined per material type (for example: Surface_PBR, Surface_Phong, Texture View etc.)
		unsigned int Material_Type;
		vector<Texture_Access> TEXTURE_LIST;
		vector<Material_Uniform> UNIFORM_LIST;

		unsigned int Find_Uniform_byName(const char* uniform_name);
	public:
		void Set_Uniform_Data(const char* uniform_name, void* pointer_to_data);
	};
}

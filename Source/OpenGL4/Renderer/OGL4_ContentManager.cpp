#include "OGL4_GPUContentManager.h"
#include "OpenGL4/OGL4_ENUMs.h"

#include <GLFW/glfw3.h>
#include <glad/glad.h>
#include <string>

namespace OpenGL4 {
	//If there is at least one Continuous Attribute before, that means there should be a Start_Offset in one of the them!
	size_t OGL4_API Get_WherePreviousAttribute_Ends(const GFX_API::VertexAttributeLayout* attributelayout, size_t vertex_count, unsigned int currentattribute_index) {
		if (currentattribute_index == 0) {
			return 0;
		}
		const GFX_API::VertexAttribute& previous_attribute = attributelayout->Attributes[currentattribute_index - 1];
		if (previous_attribute.Start_Offset == 0) {
			return Get_WherePreviousAttribute_Ends(attributelayout, vertex_count, currentattribute_index - 1) + (GFX_API::Get_UNIFORMTYPEs_SIZEinbytes(previous_attribute.DATATYPE) * vertex_count);
		}
		else {
			return previous_attribute.Start_Offset + (GFX_API::Get_UNIFORMTYPEs_SIZEinbytes(previous_attribute.DATATYPE) * vertex_count);
		}
	}
	void OGL4_API Set_VertexAttribPointer(const GFX_API::VertexAttribute* attribute, size_t Start_Offset) {
		switch (attribute->DATATYPE)
		{
		case GFX_API::UNIFORMTYPE::VAR_FLOAT32:
			glVertexAttribPointer(attribute->Index, 1, GL_FLOAT, GL_FALSE, sizeof(float32), (void*)(Start_Offset));
			break;
		case GFX_API::UNIFORMTYPE::VAR_INT32:
			glVertexAttribPointer(attribute->Index, 1, GL_INT, GL_FALSE, sizeof(int), (void*)(Start_Offset));
			break;
		case GFX_API::UNIFORMTYPE::VAR_UINT32:
			glVertexAttribPointer(attribute->Index, 1, GL_UNSIGNED_INT, GL_FALSE, sizeof(unsigned int), (void*)(Start_Offset));
			break;
		case GFX_API::UNIFORMTYPE::VAR_BYTE8:
			glVertexAttribPointer(attribute->Index, 1, GL_BYTE, GL_FALSE, sizeof(char), (void*)(Start_Offset));
			break;
		case GFX_API::UNIFORMTYPE::VAR_UBYTE8:
			glVertexAttribPointer(attribute->Index, 1, GL_UNSIGNED_BYTE, GL_FALSE, sizeof(unsigned char), (void*)(Start_Offset));
			break;
		case GFX_API::UNIFORMTYPE::VAR_VEC2:
			glVertexAttribPointer(attribute->Index, 2, GL_FLOAT, GL_FALSE, sizeof(vec2), (void*)(Start_Offset));
			break;
		case GFX_API::UNIFORMTYPE::VAR_VEC3:
			glVertexAttribPointer(attribute->Index, 3, GL_FLOAT, GL_FALSE, sizeof(vec3), (void*)(Start_Offset));
			break;
		case GFX_API::UNIFORMTYPE::VAR_VEC4:
			glVertexAttribPointer(attribute->Index, 4, GL_FLOAT, GL_FALSE, sizeof(vec4), (void*)(Start_Offset));
			break;
		case GFX_API::UNIFORMTYPE::VAR_MAT4x4:
			glVertexAttribPointer(attribute->Index, 16, GL_FLOAT, GL_FALSE, sizeof(mat4x4), (void*)(Start_Offset));
			break;
		default:
			TuranAPI::LOG_CRASHING("Uniform Type isn't supported to set to Vertex Attribute!");
			break;
		}
	}

	GPU_ContentManager::GPU_ContentManager() : UNIFORMBUFFER_BINDINGPOINTs(100){

	}
	GPU_ContentManager::~GPU_ContentManager() {
		std::cout << "OpenGL4's GPU_ContentManager Destructor is called!\n";
	}
	void GPU_ContentManager::Unload_AllResources() {
		TuranAPI::LOG_STATUS("Unloading the GPU Resources!");
		for (unsigned int i = 0; i < MESHBUFFERs.size(); i++) {
			Unload_MeshBuffer(MESHBUFFERs[i].BUFFER_ID);
		}
		for (unsigned int i = 0; i < POINTBUFFERs.size(); i++) {
			Unload_PointBuffer(POINTBUFFERs[i].BUFFER_ID);
		}
		for (unsigned int i = 0; i < GLOBALBUFFERs.size(); i++) {
			Unload_GlobalBuffer(GLOBALBUFFERs[i].ID);
		}
		for (unsigned int i = 0; i < TEXTUREs.size(); i++) {
			Unload_Texture(TEXTUREs[i].ASSET_ID);
		}
		for (unsigned int i = 0; i < SHADERPROGRAMs.size(); i++) {
			Delete_MaterialType(SHADERPROGRAMs[i].ASSET_ID);
		}
		for (unsigned int i = 0; i < SHADERSOURCEs.size(); i++) {
			Delete_ShaderSource(SHADERSOURCEs[i].ASSET_ID);
		}
		for (unsigned int i = 0; i < FBs.size(); i++) {
			Delete_Framebuffer(FBs[i].ID);
		}
		for (unsigned int i = 0; i < RTs.size(); i++) {
			Delete_RenderTarget(RTs[i].ID);
		}
		TuranAPI::LOG_STATUS("Unloading has finished!");
	}


	unsigned int GPU_ContentManager::Upload_MeshBuffer(const GFX_API::VertexAttributeLayout& attributelayout, const void* vertex_data, unsigned int data_size, unsigned int vertex_count,
		const void* index_data, unsigned int index_count) {
		if (!attributelayout.Does_BufferFits_Layout(vertex_data, data_size, vertex_count)) {
			return 0;
		}
		
		GFX_API::GFX_Mesh MESH;
		OGL4_MESH* GL_MESH = new OGL4_MESH;
		MESH.GL_ID = GL_MESH;
		MESH.VERTEX_COUNT = vertex_count;
		MESH.INDEX_COUNT = index_count;
		glGenVertexArrays(1, &GL_MESH->VAO);
		glGenBuffers(1, &GL_MESH->VBO);
		glGenBuffers(1, &GL_MESH->EBO);
		glBindVertexArray(GL_MESH->VAO);
		glBindBuffer(GL_ARRAY_BUFFER, GL_MESH->VBO);
		glBufferData(GL_ARRAY_BUFFER, data_size, vertex_data, GL_STATIC_DRAW);

		for (unsigned int attribute_index = 0; attribute_index < attributelayout.Attributes.size(); attribute_index++) {
			const GFX_API::VertexAttribute& attribute = attributelayout.Attributes[attribute_index];

			glEnableVertexAttribArray(attribute.Index);
			size_t Start_Offset = 0;
			if (attribute.Start_Offset == 0) {
				Start_Offset = Get_WherePreviousAttribute_Ends(&attributelayout, vertex_count, attribute_index);
				Set_VertexAttribPointer(&attribute, Start_Offset);
			}
			else {
				Set_VertexAttribPointer(&attribute, attribute.Start_Offset);
			}
		}
		if (index_data) {
			glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, GL_MESH->EBO);
			glBufferData(GL_ELEMENT_ARRAY_BUFFER, 4 * index_count, index_data, GL_STATIC_DRAW);
		}
		MESH.BUFFER_ID = Create_MeshBufferID();
		MESHBUFFERs.push_back(MESH);
		glBindVertexArray(0);
		return MESH.BUFFER_ID;
	}
	void GPU_ContentManager::Unload_MeshBuffer(unsigned int MeshBuffer_ID) {
		GFX_API::GFX_Mesh* MESH = nullptr;
		unsigned int vector_index = 0;
		MESH = Find_MeshBuffer_byBufferID(MeshBuffer_ID, &vector_index);
		if (MESH) {
			OGL4_MESH* OGL_MESH = (OGL4_MESH*)MESH->GL_ID;
			if (OGL_MESH) {
				glDeleteVertexArrays(1, &OGL_MESH->VAO);
				glDeleteBuffers(1, &OGL_MESH->VBO);
				glDeleteBuffers(1, &OGL_MESH->EBO);
				delete OGL_MESH;
			}
			MESH->GL_ID = nullptr;
			MESHBUFFERs.erase(MESHBUFFERs.begin() + vector_index);
			return;
		}

		TuranAPI::LOG_WARNING("Unload has failed because intended Mesh Buffer isn't found in OpenGL::GPU_ContentManager!");
	}

	unsigned int GPU_ContentManager::Upload_PointBuffer(const void* point_data, unsigned int data_size, unsigned int point_count) {
		TuranAPI::LOG_NOTCODED("OpenGL4::GPU_ContentManager::Upload_PointBuffer() isn't coded yet!", true);
		return 0;
	}
	unsigned int GPU_ContentManager::CreatePointBuffer_fromMeshBuffer(unsigned int MeshBuffer_ID, unsigned int AttributeIndex_toUseAsPointBuffer) {
		GFX_API::GFX_Mesh* MESH = nullptr;
		MESH = Find_MeshBuffer_byBufferID(MeshBuffer_ID);
		if (!MESH) {
			return 0;
		}

		GFX_API::GFX_Point PointBuffer;
		//If first vertex attribute is gonna be used, there is no need to create a Vertex Array-Buffer and upload the data.
		if (AttributeIndex_toUseAsPointBuffer == 0) {
			PointBuffer.GL_ID = MESH->GL_ID;
			PointBuffer.BUFFER_ID = Create_PointBufferID();
			if (MESH->INDEX_COUNT) {
				PointBuffer.POINT_COUNT = MESH->INDEX_COUNT;
			}
			else {
				PointBuffer.POINT_COUNT = MESH->VERTEX_COUNT;
			}
			POINTBUFFERs.push_back(PointBuffer);
			return PointBuffer.BUFFER_ID;
		}
		//First vertex attribute isn't gonna be used, so re-upload the data
		else {
			TuranAPI::LOG_NOTCODED("OpenGL4::GPU_ContentManager::CreatePointBuffer_fromMeshBuffer() only supports using the first vertex attribute as Point Buffer for now!", true);
			return 0;
		}

	}
	void GPU_ContentManager::Unload_PointBuffer(unsigned int PointBuffer_ID) {
		GFX_API::GFX_Point* POINT = nullptr;
		unsigned int vector_index = 0;
		POINT = Find_PointBuffer_byBufferID(PointBuffer_ID, &vector_index);
		if (POINT) {
			for (unsigned int i = 0; i < MESHBUFFERs.size(); i++) {
				if (POINT->GL_ID == MESHBUFFERs[i].GL_ID) {		//If the Point Buffer uses a Mesh Buffer's first vertex attribute
					POINT->GL_ID = nullptr;
					POINTBUFFERs.erase(POINTBUFFERs.begin() + vector_index);
					return;
				}
			}
			//If arrives here, that means Point Buffer doesn't use any Mesh Buffer's data or at least doesn't use its first vertex attribute

			if (POINT->GL_ID) {
				OGL4_MESH* POINTBUFFER = (OGL4_MESH*)POINT->GL_ID;
				if (POINTBUFFER) {
					glDeleteVertexArrays(1, &POINTBUFFER->VAO);
					glDeleteBuffers(1, &POINTBUFFER->VBO);
				}
				POINT->GL_ID = nullptr;
				POINTBUFFERs.erase(POINTBUFFERs.begin() + vector_index);
				return;
			}
		}
		TuranAPI::LOG_WARNING("Unload has failed because intended Point Buffer isn't found in OpenGL::GPU_ContentManager!");
	}


	void GPU_ContentManager::Upload_Texture(GFX_API::Texture_Resource* ASSET, unsigned int Asset_ID, bool Generate_Mipmap) {
		if (Find_GFXTexture_byID(Asset_ID)) {
			return;
		}
		unsigned int TEXTURE_DIMENSION = Find_Texture_Dimension(ASSET->Properties.DIMENSION);
		unsigned int TEXTURE_CHANNELs = Find_Texture_Channel_Type(ASSET->Properties.CHANNEL_TYPE);
		unsigned int TEXTURE_WRAPPING = Find_Texture_Wrapping(ASSET->Properties.WRAPPING);
		unsigned int TEXTURE_MIPMAPFILTER = Find_Texture_Mipmap_Filtering(ASSET->Properties.MIPMAP_FILTERING);
		unsigned int TEXTURE_VALUETYPE = Find_Texture_Value_Type(ASSET->Properties.VALUE_TYPE);


		unsigned int* TEXTUREID = new unsigned int(0);
		glGenTextures(1, TEXTUREID);
		glBindTexture(TEXTURE_DIMENSION, *TEXTUREID);


		glTexParameteri(TEXTURE_DIMENSION, GL_TEXTURE_WRAP_S, TEXTURE_WRAPPING);
		glTexParameteri(TEXTURE_DIMENSION, GL_TEXTURE_WRAP_T, TEXTURE_WRAPPING);
		glTexParameteri(TEXTURE_DIMENSION, GL_TEXTURE_MIN_FILTER, TEXTURE_MIPMAPFILTER);
		glTexParameteri(TEXTURE_DIMENSION, GL_TEXTURE_MAG_FILTER, TEXTURE_MIPMAPFILTER);

		glTexImage2D(TEXTURE_DIMENSION, 0, TEXTURE_CHANNELs, ASSET->WIDTH, ASSET->HEIGHT, 0, TEXTURE_CHANNELs, TEXTURE_VALUETYPE, ASSET->DATA);
		if (Generate_Mipmap) {
			glGenerateMipmap(TEXTURE_DIMENSION);
		}

		TuranAPI::LOG_NOTCODED("Texture_Resource should be able to store its own Mipmaps in Disk and we should upload them one by one here!\n", false);

		GFX_API::GFX_Texture TEXTURE;
		TEXTURE.GL_ID = TEXTUREID;
		TEXTURE.ASSET_ID = Asset_ID;
		TEXTUREs.push_back(TEXTURE);
	}
	void GPU_ContentManager::Unload_Texture(unsigned int TEXTURE_ID) {
		unsigned int vector_index = 0;
		GFX_API::GFX_Texture* TEXTURE = Find_GFXTexture_byID(TEXTURE_ID, &vector_index);
		if (TEXTURE) {
			if (TEXTURE->GL_ID) {
				glDeleteTextures(1, (unsigned int*)TEXTURE->GL_ID);
				delete (unsigned int*)TEXTURE->GL_ID;
				TEXTURE->GL_ID = nullptr;
			}
			TEXTURE->GL_ID = nullptr;
			TEXTUREs.erase(TEXTUREs.begin() + vector_index);
			return;
		}
		TuranAPI::LOG_WARNING("Unload has failed because intended Texture isn't found in OpenGL::GPU_ContentManager!");
	}


	unsigned int GPU_ContentManager::Create_GlobalBuffer(const char* NAME, void* DATA, unsigned int DATA_SIZE, GFX_API::GLOBALBUFFER_USAGE USAGE) {
		unsigned int* BUFFER_GLID = new unsigned int(0);
		glGenBuffers(1, BUFFER_GLID);
		glBindBuffer(GL_SHADER_STORAGE_BUFFER, *BUFFER_GLID);
		glBufferData(GL_SHADER_STORAGE_BUFFER, DATA_SIZE, DATA, Find_BUFFERUSAGE(USAGE));
		unsigned int* BindingPoint = new unsigned int(Create_BindingPoint());
		glBindBufferBase(GL_SHADER_STORAGE_BUFFER, *BindingPoint, *BUFFER_GLID);
		glBindBuffer(GL_SHADER_STORAGE_BUFFER, 0);

		GFX_API::GFX_Buffer BUFFER;
		BUFFER.DATA = DATA;
		BUFFER.DATA_SIZE = DATA_SIZE;
		BUFFER.ID = Create_GlobalBufferID();
		BUFFER.USAGE = USAGE;
		BUFFER.GL_ID = BUFFER_GLID;
		BUFFER.BINDING_POINT = BindingPoint;
		BUFFER.NAME = NAME;
		GLOBALBUFFERs.push_back(BUFFER);
		return BUFFER.ID;
	}
	void GPU_ContentManager::Upload_GlobalBuffer(unsigned int BUFFER_ID, void* DATA, unsigned int DATA_SIZE) {
		GFX_API::GFX_Buffer* BUFFER = Find_GlobalBuffer_byBufferID(BUFFER_ID);
		if (BUFFER) {
			if (BUFFER->GL_ID) {
				glBindBuffer(GL_SHADER_STORAGE_BUFFER, *(unsigned int*)BUFFER->GL_ID);
				if (DATA) {
					delete BUFFER->DATA;
					BUFFER->DATA = DATA;
				}
				if (DATA_SIZE) {
					BUFFER->DATA_SIZE = DATA_SIZE;
				}
				glBufferData(GL_SHADER_STORAGE_BUFFER, BUFFER->DATA_SIZE, BUFFER->DATA, Find_BUFFERUSAGE(BUFFER->USAGE));
			}
			else {
				TuranAPI::LOG_ERROR("You shouldn't call Upload_GlobalBuffer(), if you didn't create it with Create_GlobalBuffer()!");
				return;
			}
		}
	}
	void GPU_ContentManager::Unload_GlobalBuffer(unsigned int BUFFER_ID) {
		GFX_API::GFX_Buffer* BUFFER = nullptr;
		unsigned int vector_index = 0;
		BUFFER = Find_GlobalBuffer_byBufferID(BUFFER_ID, &vector_index);
		if (BUFFER) {
			if (BUFFER->GL_ID) {
				glDeleteBuffers(1, (unsigned int*)BUFFER->GL_ID);
				delete (unsigned int*)BUFFER->GL_ID;
				BUFFER->GL_ID = nullptr;
			}
			GLOBALBUFFERs.erase(GLOBALBUFFERs.begin() + vector_index);
			return;
		}
		TuranAPI::LOG_WARNING("Unload has failed because intended Global Buffer isn't found in OpenGL::GPU_ContentManager!");
	}



	void GPU_ContentManager::Compile_ShaderSource(GFX_API::ShaderSource_Resource* SHADER, unsigned int Asset_ID, string* compilation_status) {
		unsigned int STAGE = Find_ShaderStage(SHADER->STAGE);

		unsigned int* SHADER_o = new unsigned int(0);
		*SHADER_o = glCreateShader(STAGE);
		auto x = SHADER->SOURCE_CODE.c_str();
		glShaderSource(*SHADER_o, 1, &x, NULL);
		glCompileShader(*SHADER_o);

		//Check compile issues!
		int success;
		char vert_infolog[512];
		glGetShaderiv(*SHADER_o, GL_COMPILE_STATUS, &success);
		if (!success) {
			glGetShaderInfoLog(*SHADER_o, 512, NULL, vert_infolog);
			compilation_status->append(vert_infolog);
			TuranAPI::LOG_ERROR(compilation_status->c_str());
			return;
		}
		GFX_API::GFX_ShaderSource shadersource;
		shadersource.ASSET_ID = Asset_ID;
		shadersource.GL_ID = SHADER_o;
		SHADERSOURCEs.push_back(shadersource);
		compilation_status->append("Succesfully compiled!");
	}
	void GPU_ContentManager::Delete_ShaderSource(unsigned int Asset_ID) {
		GFX_API::GFX_ShaderSource* SHADER = nullptr;
		unsigned int vector_index = 0;
		SHADER = Find_GFXShaderSource_byID(Asset_ID, &vector_index);
		if (SHADER) {
			if (SHADER->GL_ID) {
				glDeleteShader(*(unsigned int*)SHADER->GL_ID);
				delete (unsigned int*)SHADER->GL_ID;
				SHADER->GL_ID = nullptr;
			}
			SHADER->GL_ID = nullptr;
			SHADERSOURCEs.erase(SHADERSOURCEs.begin() + vector_index);
			return;
		}
		TuranAPI::LOG_WARNING("Unload has failed because intended Shader Source isn't found in OpenGL::GPU_ContentManager!");
	}
	void GPU_ContentManager::Compile_ComputeShader(GFX_API::ComputeShader_Resource* SHADER, unsigned int Asset_ID, string* compilation_status) {
		OGL4_ComputeShader* OGL_CS = new OGL4_ComputeShader;

		//Compile Compute Shader Object
		{
			OGL_CS->ComputeShader_ID = glCreateShader(GL_COMPUTE_SHADER);
			auto x = SHADER->SOURCE_CODE.c_str();
			glShaderSource(OGL_CS->ComputeShader_ID, 1, &x, NULL);
			glCompileShader(OGL_CS->ComputeShader_ID);

			//Check compile issues!
			int success;
			char vert_infolog[512];
			glGetShaderiv(OGL_CS->ComputeShader_ID, GL_COMPILE_STATUS, &success);
			if (!success) {
				glGetShaderInfoLog(OGL_CS->ComputeShader_ID, 512, NULL, vert_infolog);
				compilation_status->append(vert_infolog);
				TuranAPI::LOG_ERROR(compilation_status->c_str());
				return;
			}
		}

		//Link to a Shader Program
		{
			OGL_CS->ShaderProgram_ID = glCreateProgram();

			glAttachShader(OGL_CS->ShaderProgram_ID, OGL_CS->ComputeShader_ID);
			glLinkProgram(OGL_CS->ShaderProgram_ID);

			//Check linking issues
			int link_success;
			char link_infolog[512];
			glGetProgramiv(OGL_CS->ShaderProgram_ID, GL_LINK_STATUS, &link_success);
			if (!link_success) {
				glGetProgramInfoLog(OGL_CS->ShaderProgram_ID, 512, NULL, link_infolog);
				compilation_status->append(link_infolog);
				TuranAPI::LOG_CRASHING(link_infolog);
				return;
			}

			//Bind Global Buffers
			for (unsigned int i = 0; i < SHADER->GLOBALBUFFER_IDs.size(); i++) {
				GFX_API::GFX_Buffer* BUFFER = GFXContentManager->Find_GlobalBuffer_byBufferID(SHADER->GLOBALBUFFER_IDs[i]);
				unsigned int BUFFER_BINDINGPOINT = *(unsigned int*)BUFFER->BINDING_POINT;
				unsigned int buffer_index = glGetProgramResourceIndex(OGL_CS->ShaderProgram_ID, GL_SHADER_STORAGE_BLOCK, BUFFER->NAME.c_str());
				glShaderStorageBlockBinding(OGL_CS->ShaderProgram_ID, buffer_index, BUFFER_BINDINGPOINT);
			}
		}

		GFX_API::GFX_ComputeShader shadersource;
		shadersource.ASSET_ID = Asset_ID;
		shadersource.GL_ID = OGL_CS;
		COMPUTESHADERs.push_back(shadersource);
		compilation_status->append("Succesfully compiled!");
	}
	void GPU_ContentManager::Delete_ComputeShader(unsigned int Asset_ID) {
		GFX_API::GFX_ComputeShader* SHADER = nullptr;
		unsigned int vector_index = 0;
		SHADER = Find_GFXComputeShader_byID(Asset_ID, &vector_index);
		if (SHADER) {
			if (SHADER->GL_ID) {
				OGL4_ComputeShader* OGL_CS = (OGL4_ComputeShader*)SHADER->GL_ID;
				glDeleteShader(OGL_CS->ComputeShader_ID);
				glDeleteProgram(OGL_CS->ShaderProgram_ID);
				delete OGL_CS;
			}
			SHADER->GL_ID = nullptr;
			SHADERSOURCEs.erase(SHADERSOURCEs.begin() + vector_index);
			return;
		}
		TuranAPI::LOG_WARNING("Unload has failed because intended Shader Source isn't found in OpenGL::GPU_ContentManager!");
	}
	void GPU_ContentManager::Link_MaterialType(GFX_API::Material_Type* MATTYPE_ASSET, unsigned int Asset_ID, string* compilation_status) {
		//Link and return the Shader Program!
		unsigned int* program_id = new unsigned int;
		*program_id = glCreateProgram();

		void* VS_ID = Find_GFXShaderSource_byID(MATTYPE_ASSET->VERTEXSOURCE_ID)->GL_ID;
		if (!VS_ID) {
			TuranAPI::LOG_WARNING("Vertex Shader isn't uploaded to GPU, so Shader Program linking failed!");
		}
		void* FS_ID = Find_GFXShaderSource_byID(MATTYPE_ASSET->FRAGMENTSOURCE_ID)->GL_ID;
		if (!FS_ID) {
			TuranAPI::LOG_WARNING("Vertex Shader isn't uploaded to GPU, so Shader Program linking failed!");
		}
		//Link Vertex and Fragment Shader to Shader Program and set ID
		glAttachShader(*program_id, *(unsigned int*)VS_ID);
		glAttachShader(*program_id, *(unsigned int*)FS_ID);
		glLinkProgram(*program_id);

		//Check linking issues
		int link_success;
		char link_infolog[512];
		glGetProgramiv(*program_id, GL_LINK_STATUS, &link_success);
		if (!link_success) {
			glGetProgramInfoLog(*program_id, 512, NULL, link_infolog);
			compilation_status->append(link_infolog);
			TuranAPI::LOG_CRASHING(link_infolog);
			return;
		}

		//Bind Global Buffers
		for (unsigned int i = 0; i < MATTYPE_ASSET->GLOBALBUFFERs.size(); i++) {
			GFX_API::GFX_Buffer* BUFFER = GFXContentManager->Find_GlobalBuffer_byBufferID(MATTYPE_ASSET->GLOBALBUFFERs[i].BUFFER_ID);
			unsigned int BUFFER_BINDINGPOINT = *(unsigned int*)BUFFER->BINDING_POINT;
			unsigned int buffer_index = glGetProgramResourceIndex(*program_id, GL_SHADER_STORAGE_BLOCK, BUFFER->NAME.c_str());
			glShaderStorageBlockBinding(*program_id, buffer_index, BUFFER_BINDINGPOINT);
		}

		GFX_API::GFX_ShaderProgram SHADERPROGRAM;
		SHADERPROGRAM.ASSET_ID = Asset_ID;
		SHADERPROGRAM.GL_ID = program_id;
		SHADERPROGRAMs.push_back(SHADERPROGRAM);
		compilation_status->append("Succesfully linked!");
	}
	void GPU_ContentManager::Delete_MaterialType(unsigned int Asset_ID) {
		GFX_API::GFX_ShaderProgram* PROGRAM = nullptr;
		unsigned int vector_index = 0;
		PROGRAM = Find_GFXShaderProgram_byID(Asset_ID, &vector_index);
		if (PROGRAM) {
			if (PROGRAM->GL_ID) {
				glDeleteProgram(*(unsigned int*)PROGRAM->GL_ID);
				delete (unsigned int*)PROGRAM->GL_ID;
				PROGRAM->GL_ID = nullptr;
			}
			PROGRAM->GL_ID = nullptr;
			SHADERPROGRAMs.erase(SHADERPROGRAMs.begin() + vector_index);
			return;
		}
		TuranAPI::LOG_WARNING("Unload has failed because intended Material Type isn't found in OpenGL::GPU_ContentManager!");
	}


	unsigned int GPU_ContentManager::Create_RenderTarget(unsigned int WIDTH, unsigned int HEIGTH, GFX_API::TEXTURE_DIMENSIONs DIMENSION,
		GFX_API::TEXTURE_TYPEs FORMAT, GFX_API::UNIFORMTYPE VALUETYPE, bool Usable_as_Texture) {
		GFX_API::RenderTarget RT;
		RT.WIDTH = WIDTH; RT.HEIGHT = HEIGTH; RT.DIMENSION = DIMENSION; RT.FORMAT = FORMAT;
		RT.FORMAT_VALUETYPE = VALUETYPE; RT.Usable_as_Texture = Usable_as_Texture;

		//Create a Texture!
		if (Usable_as_Texture) {
			unsigned int GL_FORMAT = Find_RenderTarget_Format_Type(FORMAT);
			unsigned int GL_DIMENSION = Find_Texture_Dimension(DIMENSION);
			unsigned int GL_VALUE_TYPE = Find_Texture_Value_Type(VALUETYPE);

			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

			RT.GL_ID = new unsigned int(0);
			glGenTextures(1, (unsigned int*)RT.GL_ID);
			glBindTexture(GL_TEXTURE_2D, *(unsigned int*)RT.GL_ID);
			glTexImage2D(GL_TEXTURE_2D, 0, GL_FORMAT, WIDTH, HEIGTH, 0, GL_FORMAT, GL_VALUE_TYPE, NULL);
			{
				string status;
				status.append("Created a Render Target's OpenGL ID: ");
				status.append(std::to_string(*(unsigned int*)RT.GL_ID).c_str());
				TuranAPI::LOG_STATUS(status);
			}

			GFX->Check_Errors();
		}
		//Create a Render Buffer
		else {
			TuranAPI::LOG_NOTCODED("Render Buffer creation isn't supported, you should set Usable_as_Texture to true!\n", true);
			return 0;
		}

		RT.ID = Create_RenderTargetID();
		RTs.push_back(RT);
		return RT.ID;
	}
	void GPU_ContentManager::Delete_RenderTarget(unsigned int RT_ID) {
		GFX_API::RenderTarget* RT = nullptr;
		unsigned int vector_index = 0;
		RT = Find_RenderTarget_byID(RT_ID, &vector_index);
		if (RT) {
			if (RT->GL_ID) {
				if (RT->Usable_as_Texture) {
					glDeleteTextures(1, (unsigned int*)RT->GL_ID);
				}
				else {
					glDeleteRenderbuffers(1, (unsigned int*)RT->GL_ID);
				}
				delete (unsigned int*)RT->GL_ID;
				RT->GL_ID = nullptr;
			}
			RTs.erase(RTs.begin() + vector_index);
			return;
		}
		TuranAPI::LOG_WARNING("Unload has failed because intended Render Target isn't found in OpenGL::GPU_ContentManager!");
	}

	unsigned int GPU_ContentManager::Create_Framebuffer() {
		GFX_API::Framebuffer FB;
		
		FB.GL_ID = new unsigned int(0);
		glGenFramebuffers(1, (unsigned int*)FB.GL_ID);
		GFX->Check_Errors();
		FB.ID = Create_FrameBufferID();
		FBs.push_back(FB);
		return FB.ID;
	}
	void GPU_ContentManager::Delete_Framebuffer(unsigned int Framebuffer_ID) {
		GFX_API::Framebuffer* FB = nullptr;
		unsigned int vector_index = 0;
		FB = Find_Framebuffer_byGFXID(Framebuffer_ID, &vector_index);
		if (FB) {
			if (FB->GL_ID) {
				glDeleteFramebuffers(1, (unsigned int*)FB->GL_ID);
				delete (unsigned int*)FB->GL_ID;
			}
			FB->GL_ID = nullptr;
			FBs.erase(FBs.begin() + vector_index);
			return;
		}
		TuranAPI::LOG_WARNING("Unload has failed because intended Framebuffer isn't found in OpenGL::GPU_ContentManager!");
	}
	void GPU_ContentManager::Attach_RenderTarget_toFramebuffer(const GFX_API::Framebuffer::RT_SLOT* RT_SLOT, GFX_API::RT_ATTACHMENTs ATTACHMENT_TYPE, unsigned int FB_ID) {
		GFX_API::Framebuffer* FB = nullptr;
		FB = Find_Framebuffer_byGFXID(FB_ID);

		GFX_API::RenderTarget* RT = nullptr;
		RT = Find_RenderTarget_byID(RT_SLOT->RT_ID);
		unsigned int RT_GLID = *(unsigned int*)RT->GL_ID;

		glBindFramebuffer(GL_FRAMEBUFFER, *(unsigned int*)Find_Framebuffer_byGFXID(FB_ID)->GL_ID);
		if (RT->Usable_as_Texture) {
			glBindTexture(GL_TEXTURE_2D, RT_GLID);
			glFramebufferTexture2D(GL_FRAMEBUFFER, Find_RenderTarget_AttachmentType(ATTACHMENT_TYPE), GL_TEXTURE_2D, RT_GLID, 0);
		}
		else {
			TuranAPI::LOG_CRASHING("Creating a Render Buffer for RT isn't supported for now, so you can't bind it to Framebuffer! There is a problem somewhere");
			return;
		}
		Check_ActiveFramebuffer_Status(std::to_string(FB->ID).c_str());


		FB->BOUND_RTs.push_back(*RT_SLOT);
	}


	unsigned int GPU_ContentManager::Create_BindingPoint() {
		//We can use the Binding Point 0!
		unsigned int ID = UNIFORMBUFFER_BINDINGPOINTs.GetIndex_FirstFalse();
		UNIFORMBUFFER_BINDINGPOINTs.SetBit_True(ID);
		return ID;
	}
	void GPU_ContentManager::Delete_BindingPoint(unsigned int ID) {
		UNIFORMBUFFER_BINDINGPOINTs.SetBit_False(ID);
	}
}
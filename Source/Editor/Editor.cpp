#include "Editor.h"
using namespace TuranEditor;

int main() {
	Editor_System Systems;
	Game_RenderGraph First_RenderGraph;
	Main_Window* main_window = new Main_Window(&First_RenderGraph);

	const char* MODEL_PATH = "C:/Users/furka/Desktop/Models/sponza/sponza.obj";
	unsigned int MODEL_ID = Model_Importer::Import_Model(MODEL_PATH);

	if (MODEL_ID) {
		Static_Model* MODEL = (Static_Model*)EDITOR_FILESYSTEM->Find_ResourceIdentifier_byID(MODEL_ID)->DATA;
		for (unsigned int i = 0; i < MODEL->MESHes.size(); i++) {
			Static_Mesh_Data* MESH = MODEL->MESHes[i];
			unsigned int MESHBUFFER_ID = GFXContentManager->Upload_MeshBuffer(MESH->DataLayout, MESH->VERTEX_DATA, MESH->VERTEXDATA_SIZE, MESH->VERTEX_NUMBER, MESH->INDEX_DATA, MESH->INDICES_NUMBER);

			GFX_API::DrawCall Mesh_DrawCall;
			Mesh_DrawCall.MeshBuffer_ID = MESHBUFFER_ID;
			Mesh_DrawCall.ShaderInstance_ID = MODEL->MATINST_IDs[MESH->Material_Index];
			Mesh_DrawCall.JoinedDrawPasses = 0xffffffff;	//Join all draw passes for now!

			First_RenderGraph.Register_DrawCall(Mesh_DrawCall);
		}
	}

	while (main_window->Get_Is_Window_Open()) {
		TURAN_PROFILE_SCOPE("Run Loop");
		
		Editor_RendererDataManager::Update_GPUResources();
		IMGUI->New_Frame();
		IMGUI_RUNWINDOWS();
		GFX->Swapbuffers_ofMainWindow();
		//IMGUI can use main renderer's contents, so render IMGUI later!
		IMGUI->Render_Frame();
		GFX->Run_RenderGraphs();

		//Take inputs by GFX API specific library that supports input (For now, just GLFW for OpenGL4) and send it to Engine!
		//In final product, directly OS should be used to take inputs!
		Editor_System::Take_Inputs();
	}

	return 1;
}
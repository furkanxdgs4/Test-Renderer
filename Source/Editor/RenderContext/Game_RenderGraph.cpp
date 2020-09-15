#include "Game_RenderGraph.h"
#include "Draw Passes/Draw_Passes.h"
#include "Compute Passes/Compute_Passes.h"
#include "Editor/FileSystem/EditorFileSystem_Core.h"
#include "Editor/FileSystem/ResourceTypes/ResourceTYPEs.h"
#include "Editor/FileSystem/ResourceTypes/Resource_Identifier.h"
#include "GFX/GFX_Core.h"

namespace TuranEditor {


	Game_RenderGraph::Game_RenderGraph() : RenderGraph("Game RenderGraph") {
		TuranAPI::LOG_STATUS("Game RenderGraph object is created!");
		vector<GFX_API::Framebuffer::RT_SLOT> MainDrawPass_SLOTs;
		GFX_API::DrawPass* MainPass = new Main_DrawPass(DrawCalls, PointDrawCallBuffer, MainDrawPass_SLOTs);
		RENDER_NODEs.push_back(MainPass);
		
		GFX_API::Texture_Resource* COLOR_RT = new GFX_API::Texture_Resource;
		Resource_Identifier* COLORRT_RESOURCE = new Resource_Identifier;
		{
			COLOR_RT->DATA = nullptr;
			COLOR_RT->DATA_SIZE = 1920 * 1080 * 4;
			COLOR_RT->Has_Mipmaps = false;
			COLOR_RT->HEIGHT = 1080;
			COLOR_RT->WIDTH = 1920;
			COLOR_RT->OP_TYPE = GFX_API::BUFFER_VISIBILITY::CPUEXISTENCE_GPUREADWRITE;
			COLOR_RT->Properties.CHANNEL_TYPE = GFX_API::TEXTURE_CHANNELs::API_TEXTURE_RGBA8UB;
			COLOR_RT->Properties.TYPE = GFX_API::TEXTURE_TYPEs::COLORRT_TEXTURE;

			COLORRT_RESOURCE->DATA = COLOR_RT;
			COLORRT_RESOURCE->TYPE = RESOURCETYPEs::GFXAPI_TEXTURE;
			EDITOR_FILESYSTEM->Add_anAsset_toAssetList(COLORRT_RESOURCE);
		}
		GFXContentManager->Create_Texture(COLOR_RT, COLORRT_RESOURCE->ID);
		MainDrawPass_SLOTs[0].RT_ID = COLORRT_RESOURCE->ID;


		GFX_API::Texture_Resource* DEPTH_RT = new GFX_API::Texture_Resource;
		Resource_Identifier* DEPTHRT_RESOURCE = new Resource_Identifier;
		{
			DEPTH_RT->DATA = nullptr;
			DEPTH_RT->DATA_SIZE = 1920 * 1080;
			DEPTH_RT->Has_Mipmaps = false;
			DEPTH_RT->HEIGHT = 1080;
			DEPTH_RT->WIDTH = 1920;
			DEPTH_RT->OP_TYPE = GFX_API::BUFFER_VISIBILITY::CPUEXISTENCE_GPUREADWRITE;
			DEPTH_RT->Properties.CHANNEL_TYPE = GFX_API::TEXTURE_CHANNELs::API_TEXTURE_R32F;
			DEPTH_RT->Properties.TYPE = GFX_API::TEXTURE_TYPEs::DEPTHTEXTURE;

			DEPTHRT_RESOURCE->DATA = DEPTH_RT;
			DEPTHRT_RESOURCE->TYPE = RESOURCETYPEs::GFXAPI_TEXTURE;
			EDITOR_FILESYSTEM->Add_anAsset_toAssetList(DEPTHRT_RESOURCE);
		}
		GFXContentManager->Create_Texture(DEPTH_RT, DEPTHRT_RESOURCE->ID);
		MainDrawPass_SLOTs[1].RT_ID = DEPTHRT_RESOURCE->ID;
		MainPass->RenderGraph_SetupPhase(MainDrawPass_SLOTs);

		RENDER_NODEs.push_back(new FirstCompute);
	}

	void Game_RenderGraph::Run_RenderGraph() {
		if (RENDER_NODEs.size() == 0) {
			TuranAPI::LOG_CRASHING("RenderGraph isn't constructed properly, there is no Render Node!");
			return;
		}


		TuranAPI::LOG_STATUS("Running Game_RenderGraph!");
		GFX_API::DrawPass* MainPass = (GFX_API::DrawPass*)RENDER_NODEs[0];
		MainPass->ResourceUpdatePhase();
		MainPass->Execute();

		((GFX_API::ComputePass*)RENDER_NODEs[1])->Execute();

		//Display the final result on main window!
		//GFX_API::RENDERER->Show_RenderTarget_onWindow(GFX_API::ONSCREEN_Windows[0], DRAW_PASSes[0]->Get_Framebuffer()->BOUND_RTs[0]);
		TuranAPI::LOG_STATUS("Finished running the Game_RenderGraph!");
	}
}
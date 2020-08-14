#pragma once
#include "GFX/GFX_Includes.h"
#include "GFX/Renderer/GFX_Resource.h"
#include "GFX/Renderer/GFX_RenderGraph.h"
#include "GFX/GFX_FileSystem/Resource_Type/Material_Type_Resource.h"


class Main_DrawPass : public GFX_API::DrawPass {
	void Create_LineRendererMatInst();
	GFX_API::Material_Instance* LineRenderer_MatInst = nullptr;
public:
	Main_DrawPass(const vector<GFX_API::DrawCall>& RG_DrawCalls, const vector<GFX_API::PointLineDrawCall>& i_RG_PointDrawCallBuffer, vector<GFX_API::Framebuffer::RT_SLOT>& Needed_RTSlots);

	static unsigned int Get_BitMaskFlag();

	virtual void RenderGraph_SetupPhase(vector<GFX_API::Framebuffer::RT_SLOT>& RTs) override;
	virtual void ResourceUpdatePhase() override;
	virtual void Execute() override;
};

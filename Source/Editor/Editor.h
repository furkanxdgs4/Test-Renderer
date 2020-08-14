#pragma once
#include "Editor_Includes.h"
#include "Editors/Main_Window.h"
#include "RenderContext/Game_RenderGraph.h"
#include "GFX/IMGUI/IMGUI_Core.h"
#include "GFX/GFX_Core.h"
#include "FileSystem/EditorFileSystem_Core.h"

#include "FileSystem/ResourceImporters/Model_Loader.h"
#include "Editor/RenderContext/Editor_DataManager.h"

namespace TuranEditor {

	class Editor_System {
		TuranAPI::Logging::Logger LOGGING;
		Editor_FileSystem FileSystem;
		static bool Should_Close;
	public:
		Editor_System();
		~Editor_System();
		static void Take_Inputs();
		static bool Should_EditorClose();
	};


}
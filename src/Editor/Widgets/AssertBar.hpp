#pragma once

//defines a bar widget for managing assets loaded and modifiying them

#include <SmokAssetConvertionTool/Mesh/MeshConvertionTool.hpp>

#include <SmokRenderers/AssetManager.hpp>

#include <SmokTyGUI/DrawCommands.hpp>

namespace SmokTool::ACT::GUI
{
	//draws a window for selecting a mesh
	static inline bool DrawMeshSelectWindow(Smok::Renderers::AssetManager* assetManager, uint64& assetID)
	{
		ImGui::Begin("Asset Select");

		for (auto& asset : assetManager->staticMeshAssets)
		{
			std::string name = assetManager->GetNameByID(asset.first);
			if (ImGui::Button(name.c_str(), Smok::TyGUI::Util::CalculateLabelSize(name)))
			{
				ImGui::End();
				assetID = asset.first;
				return true;
			}
		}

		ImGui::End();

		return false;
	}

	//draws a window for selecting a graphics shader
	static inline bool DrawGraphicsShaderSelectWindow(Smok::Renderers::AssetManager* assetManager, uint64& assetID)
	{
		ImGui::Begin("Asset Select");

		for (auto& asset : assetManager->GShaderAssets)
		{
			std::string name = assetManager->GetNameByID(asset.first);
			if (ImGui::Button(name.c_str(), Smok::TyGUI::Util::CalculateLabelSize(name)))
			{
				ImGui::End();
				assetID = asset.first;
				return true;
			}
		}

		ImGui::End();

		return false;
	}

	//defines a struct for importing raw mesh settings
	struct StaticMeshImportMenuSettings
	{
		uint32 LODCount = 0;

		BTD::Math::FVec3 baseMeshScale = BTD::Math::FVec3::One();

		std::string rawMeshFile = "",
			assetName = "";

		//resets the settings
		inline void Reset()
		{
			LODCount = 0;
			baseMeshScale = BTD::Math::FVec3::One();
			rawMeshFile = ""; assetName = "";
		}
	};

	//menu for importing a raw static mesh
	static inline bool DrawRawStaticMeshImportMenu(Smok::Renderers::AssetManager* assetManager,
		StaticMeshImportMenuSettings* settings, void* window)
	{
		ImGui::Begin("Static Mesh Raw Import");

		//exit the menu
		if (ImGui::Button("X", Smok::TyGUI::Util::CalculateLabelSize("X")))
		{
			ImGui::End();
			settings->Reset();
			return true;
		}

		//file path for raw mesh
		if (ImGui::Button("Browse", Smok::TyGUI::Util::CalculateLabelSize("Browse")))
		{
			std::string filePath = BTD::IO::OpenFileDialog("", window);
			if (!filePath.empty())
				settings->rawMeshFile = filePath;
		}
		ImGui::SameLine();
		ImGui::Text("Raw: %s", settings->rawMeshFile.c_str());

		//asset name
		char str[999]; strcpy(str, settings->assetName.c_str());
		if (ImGui::InputText("Name", str, sizeof(char) * 999))
			settings->assetName = str;

		//base mesh scale
		float* d = (float*)&settings->baseMeshScale;
		ImGui::DragFloat3("Base Mesh Scale", d, 0.01f, 10.0f);

		//how many LODs to generate using Atomic

		//import the mesh
		if (ImGui::Button("Import", Smok::TyGUI::Util::CalculateLabelSize("Import")))
		{
			//imports mesh
			std::vector<Smok::Mesh::Mesh> meshes;
			if(SmokTools::AssetConvertion::Mesh::ConvertStaticMeshToSmokFormat(settings->rawMeshFile.c_str(), meshes))
			{
				const size_t meshCount = meshes.size();
				for (uint32 i = 0; i < meshCount; ++i)
					meshes[i].baseScale = settings->baseMeshScale;

				//write decl data
				Smok::Mesh::Mesh_WriteMeshDataToFile("res/Meshes", settings->assetName, meshes);

				//create asset
				Smok::Renderers::StaticMesh* m = assetManager->RegisterStaticMesh(settings->assetName.c_str(),
					std::string("res/Meshes/" + settings->assetName + SMOK_MESH_MESH_DATA_DECL_EXTENTION).c_str());
			}
			else
			{
				BTD_LogError("Smok ACT GUI", "Asset Bar", "DrawRawStaticMeshImportMenu",
					std::string("Failed to convert a mesh to one Smok underdstands from \"" + settings->assetName + "\"").c_str());
			}

			ImGui::End();
			settings->Reset();
			return true;
		}

		ImGui::SameLine();

		//cancel the import menu
		if (ImGui::Button("Cancel", Smok::TyGUI::Util::CalculateLabelSize("Cancel")))
		{
			ImGui::End();
			settings->Reset();
			return true;
		}

		ImGui::End();
	
		return false;
	}

//#define SmokTool_Act_GUI_RenderAssetButton(assetList, setSelectedAssetFuncName) \
//for (auto& asset : assetList) \
//{\
//	/*render snap shot photo of asset*/\
//\
//	/*button for selecting, make it the selected asset*/\
//	std::string name = project->GetAssetNameByID(asset.second.ACTDatabaseID);\
//	if (ImGui::Button(name.c_str(), Smok::TyGUI::Util::CalculateLabelSize(name)))\
//		project->setSelectedAssetFuncName(&asset.second);\
//}

#define SMOKTOOL_ACT_DATA_ASSET_TYPE_COUNT 5
	const char* assetTypeStrs[SMOKTOOL_ACT_DATA_ASSET_TYPE_COUNT] = {
		"Texture",
		"Sampler",

		"GShader",
		"GPipeline",

		"StaticMesh"
	};
#define SMOKTOOL_ACT_DATA_ASSET_TYPE_TEXTURE_INDEX 0
#define SMOKTOOL_ACT_DATA_ASSET_TYPE_SAMPLER_INDEX 1
#define SMOKTOOL_ACT_DATA_ASSET_TYPE_GSHADER_INDEX 2
#define SMOKTOOL_ACT_DATA_ASSET_TYPE_GPIPELINE_INDEX 3
#define SMOKTOOL_ACT_DATA_ASSET_TYPE_STATICMESH_INDEX 4

	//defines the asset bar
	struct AssetBarWidget
	{
		Smok::Renderers::AssetManager* assetManager = nullptr;

		bool actveFilter[SMOKTOOL_ACT_DATA_ASSET_TYPE_COUNT]; //the filters

		AssetBarWidget()
		{
			memset(actveFilter, 0, sizeof(actveFilter));
		}

		//renders the bar and all the assets
		inline void Draw(void* window)
		{
			ImGui::Begin("Asset Bar");

			//drop down for selecting any filters
			static int selectedItem = 0;
			if (ImGui::Combo("Asset Type", &selectedItem, assetTypeStrs, SMOKTOOL_ACT_DATA_ASSET_TYPE_COUNT))
				actveFilter[selectedItem] = !actveFilter[selectedItem];

			ImGui::SameLine();
			ImGui::LabelText("Filter: %s", assetTypeStrs[selectedItem]);

			//checks if any filters are active
			bool isFilter = false;
			for (uint32 i = 0; i < SMOKTOOL_ACT_DATA_ASSET_TYPE_COUNT; ++i)
			{
				if (actveFilter[i])
				{
					isFilter = true;
					break;
				}
			}

			ImGui::Spacing(); ImGui::Spacing(); ImGui::Spacing();

			//if no filters are set, render everything
			if (!isFilter)
			{
				//SmokTool_Act_GUI_RenderAssetButton(project->staticMeshes, SetSelectedStaticMesh)
				for (auto& asset : assetManager->staticMeshAssets)
				{
					/*render snap shot photo of asset*/
					
					/*button for selecting, make it the selected asset*/
					std::string name = assetManager->GetNameByID(asset.first);
					if (ImGui::Button(name.c_str(), Smok::TyGUI::Util::CalculateLabelSize(name))) {}
						//project->setSelectedAssetFuncName(&asset.second); 
				}

					//SmokTool_Act_GUI_RenderAssetButton(project->texture2Ds, SetSelectedTexture2D)
				for (auto& asset : assetManager->textureAssets)
				{
					/*render snap shot photo of asset*/
					
					/*button for selecting, make it the selected asset*/
					std::string name = assetManager->GetNameByID(asset.first);
					if (ImGui::Button(name.c_str(), Smok::TyGUI::Util::CalculateLabelSize(name))) {}
						//project->setSelectedAssetFuncName(&asset.second);
				}

					//SmokTool_Act_GUI_RenderAssetButton(project->sampler2Ds, SetSelectedSampler2D)
				for (auto& asset : assetManager->samplerAssets)
				{
					/*render snap shot photo of asset*/
					
					/*button for selecting, make it the selected asset*/
					std::string name = assetManager->GetNameByID(asset.first);
					if (ImGui::Button(name.c_str(), Smok::TyGUI::Util::CalculateLabelSize(name))) {}
						//project->setSelectedAssetFuncName(&asset.second);
				}

					//SmokTool_Act_GUI_RenderAssetButton(project->materials, SetSelectedMaterial)
				//for (auto& asset : assetManager \
				//{\
				//	/*render snap shot photo of asset*/\
				//	\
				//	/*button for selecting, make it the selected asset*/\
				//	std::string name = project->GetAssetNameByID(asset.second.ACTDatabaseID); \
				//	if (ImGui::Button(name.c_str(), Smok::TyGUI::Util::CalculateLabelSize(name)))\
				//		project->setSelectedAssetFuncName(&asset.second); \
				//}

					//SmokTool_Act_GUI_RenderAssetButton(project->graphicsPipeline, SetSelectedGraphicsPipeline)
				for (auto& asset : assetManager->GPipelineAssets)
				{
					/*render snap shot photo of asset*/
					
					/*button for selecting, make it the selected asset*/
					std::string name = assetManager->GetNameByID(asset.first);
					if (ImGui::Button(name.c_str(), Smok::TyGUI::Util::CalculateLabelSize(name))) {}
						//project->setSelectedAssetFuncName(&asset.second);
				}

					//SmokTool_Act_GUI_RenderAssetButton(project->graphicsShader, SetSelectedGraphicsShader)
				for (auto& asset : assetManager->GShaderAssets)
				{
					/*render snap shot photo of asset*/
					
					/*button for selecting, make it the selected asset*/
					std::string name = assetManager->GetNameByID(asset.first);
					if (ImGui::Button(name.c_str(), Smok::TyGUI::Util::CalculateLabelSize(name))) {}
						//project->setSelectedAssetFuncName(&asset.second); 
				}
			}

			//else, render only what is set to render
			else
			{
				if (actveFilter[SMOKTOOL_ACT_DATA_ASSET_TYPE_STATICMESH_INDEX])
				{
					for (auto& asset : assetManager->staticMeshAssets)
					{
						/*render snap shot photo of asset*/

						/*button for selecting, make it the selected asset*/
						std::string name = assetManager->GetNameByID(asset.first);
						if (ImGui::Button(name.c_str(), Smok::TyGUI::Util::CalculateLabelSize(name))) {}
						//project->setSelectedAssetFuncName(&asset.second); 
					}
				}

				if (actveFilter[SMOKTOOL_ACT_DATA_ASSET_TYPE_TEXTURE_INDEX])
				{
					for (auto& asset : assetManager->textureAssets)
					{
						/*render snap shot photo of asset*/

						/*button for selecting, make it the selected asset*/
						std::string name = assetManager->GetNameByID(asset.first);
						if (ImGui::Button(name.c_str(), Smok::TyGUI::Util::CalculateLabelSize(name))) {}
						//project->setSelectedAssetFuncName(&asset.second);
					}
				}

				if (actveFilter[SMOKTOOL_ACT_DATA_ASSET_TYPE_SAMPLER_INDEX])
				{
					for (auto& asset : assetManager->samplerAssets)
					{
						/*render snap shot photo of asset*/

						/*button for selecting, make it the selected asset*/
						std::string name = assetManager->GetNameByID(asset.first);
						if (ImGui::Button(name.c_str(), Smok::TyGUI::Util::CalculateLabelSize(name))) {}
						//project->setSelectedAssetFuncName(&asset.second);
					}
				}

				//if (actveFilter[3])
				//{
				//	SmokTool_Act_GUI_RenderAssetButton(project->materials, SetSelectedMaterial)
				//		for (auto& asset : assetList) \
				//		{\
				//			/*render snap shot photo of asset*/\
				//			\
				//			/*button for selecting, make it the selected asset*/\
				//			std::string name = project->GetAssetNameByID(asset.second.ACTDatabaseID); \
				//			if (ImGui::Button(name.c_str(), Smok::TyGUI::Util::CalculateLabelSize(name)))\
				//				project->setSelectedAssetFuncName(&asset.second); \
				//		}
				//}

				if (actveFilter[SMOKTOOL_ACT_DATA_ASSET_TYPE_GPIPELINE_INDEX])
				{
					for (auto& asset : assetManager->GPipelineAssets)
					{
						/*render snap shot photo of asset*/

						/*button for selecting, make it the selected asset*/
						std::string name = assetManager->GetNameByID(asset.first);
						if (ImGui::Button(name.c_str(), Smok::TyGUI::Util::CalculateLabelSize(name))) {}
						//project->setSelectedAssetFuncName(&asset.second);
					}
				}

				if (actveFilter[SMOKTOOL_ACT_DATA_ASSET_TYPE_GSHADER_INDEX])
				{
					for (auto& asset : assetManager->GShaderAssets)
					{
						/*render snap shot photo of asset*/

						/*button for selecting, make it the selected asset*/
						std::string name = assetManager->GetNameByID(asset.first);
						if (ImGui::Button(name.c_str(), Smok::TyGUI::Util::CalculateLabelSize(name))) {}
						//project->setSelectedAssetFuncName(&asset.second); 
					}
				}
			}

			//context menu for right clicking a asset

			//context menu for if empty space right click
			static bool rawStaticMeshImportMenuOpen = false;
			if (ImGui::BeginPopupContextWindow())
			{
				//imports a raw static mesh
				if (ImGui::MenuItem("New Raw Static Mesh"))
				{
					//open a pop up menu for importing a raw static mesh
					rawStaticMeshImportMenuOpen = true;
				}

				//imports a prexisting Smok static mesh

				//imports a static mesh

				//creates a new Graphics Pipeline

				//creates a new Graphics Shader

				ImGui::EndPopup();
			}

			ImGui::End();

			//renders the menu for importing the raw static mesh
			if (rawStaticMeshImportMenuOpen)
			{
				static StaticMeshImportMenuSettings staticMeshImportSettings;
				if (DrawRawStaticMeshImportMenu(assetManager, &staticMeshImportSettings, window))
				{
					////registers the imported asset
					//assetManager->RegisterStaticMesh(staticMeshImportSettings.assetName.c_str(), std::string("res/Meshes/" + staticMeshImportSettings.assetName + ".smeshdecl").c_str());
					//assetManager->RegisterStaticMesh(staticMeshImportSettings.assetName.c_str(),
						//std::string(std::string("res/Meshes/") + staticMeshImportSettings.assetName + "/smeshdecl").c_str());
					//rawStaticMeshImportMenuOpen = false;
				}
			}
		}
	};
}
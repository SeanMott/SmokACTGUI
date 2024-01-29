#pragma once

//defines a inspector for modifying selected entities

#include <Editor/Widgets/AssertBar.hpp>

#include <ECS/Scene.hpp>

#include <SmokTyGUI/DrawCommands.hpp>
#include <SmokTyGUI/Widgets/Button.hpp>

namespace SmokTool::ACT::GUI
{
	//defines a widget for modifying entities
	struct EntityInspectorWidget
	{
		Smok::Renderers::AssetManager* assetManager;

		//draws the inspector
		inline void Draw(TableToolkit::ECS::Scene::RenderObject* selectedEntity, bool& sceneIsDirty)
		{
			static bool selectWindowIsOpen[4];
#define SMOKTOOL_ACT_GUI_ENTITY_INSPECTOR_SELECT_WINDOW_BOOL_ARRAY_INDEX_STATIC_MESH 0
#define SMOKTOOL_ACT_GUI_ENTITY_INSPECTOR_SELECT_WINDOW_BOOL_ARRAY_INDEX_GRAPHICS_SHADER 1
#define SMOKTOOL_ACT_GUI_ENTITY_INSPECTOR_SELECT_WINDOW_BOOL_ARRAY_INDEX_GRAPHICS_PIPELINE 2
#define SMOKTOOL_ACT_GUI_ENTITY_INSPECTOR_SELECT_WINDOW_BOOL_ARRAY_INDEX_TEXTURE 3

			//render data
			if (selectedEntity)
			{
				ImGui::Begin("Inspector");

				//entity name
				ImGui::LabelText("Name", selectedEntity->entityInfo.entityName.c_str());

				ImGui::Spacing(); ImGui::Spacing(); ImGui::Spacing();

				//transform
				if (ImGui::SliderFloat3("Position", (float*)&selectedEntity->transform.position,
					0.01f, 10.0f))
					sceneIsDirty = true;
				if(ImGui::SliderFloat4("Rotation", (float*)&selectedEntity->transform.rotation,
					0.01f, 10.0f))
					sceneIsDirty = true;
				if(ImGui::SliderFloat3("Scale", (float*)&selectedEntity->transform.scale,
					0.01f, 10.0f))
					sceneIsDirty = true;

				ImGui::Spacing(); ImGui::Spacing(); ImGui::Spacing();

				//mesh
				if(ImGui::Checkbox("is Enabled", &selectedEntity->mesh.isEnabled)) //should the mesh render
					sceneIsDirty = true; 

				//goes through the indices and stores what indexes should be rendered
				//std::vector<uint32> meshIndexsToRender; //the individual meshes of the larger mesh to render

				//let the mesh be selected
				if (ImGui::Button(selectedEntity->mesh.staticMeshName.c_str(), Smok::TyGUI::Util::CalculateLabelSize(selectedEntity->mesh.staticMeshName)))
				{
					bool state = selectWindowIsOpen[SMOKTOOL_ACT_GUI_ENTITY_INSPECTOR_SELECT_WINDOW_BOOL_ARRAY_INDEX_STATIC_MESH];
					memset(selectWindowIsOpen, false, sizeof(selectWindowIsOpen));
					selectWindowIsOpen[SMOKTOOL_ACT_GUI_ENTITY_INSPECTOR_SELECT_WINDOW_BOOL_ARRAY_INDEX_STATIC_MESH] = !state;
				}

				//let the graphics shader be selected
				if (ImGui::Button(selectedEntity->mesh.graphicsShaderName.c_str(), Smok::TyGUI::Util::CalculateLabelSize(selectedEntity->mesh.graphicsShaderName)))
				{
					bool state = selectWindowIsOpen[SMOKTOOL_ACT_GUI_ENTITY_INSPECTOR_SELECT_WINDOW_BOOL_ARRAY_INDEX_GRAPHICS_SHADER];
					memset(selectWindowIsOpen, false, sizeof(selectWindowIsOpen));
					selectWindowIsOpen[SMOKTOOL_ACT_GUI_ENTITY_INSPECTOR_SELECT_WINDOW_BOOL_ARRAY_INDEX_GRAPHICS_SHADER] = !state;
				}

				//let the graphics pipeline be selected
				if (ImGui::Button(selectedEntity->mesh.graphicsPipelineName.c_str(), Smok::TyGUI::Util::CalculateLabelSize(selectedEntity->mesh.graphicsPipelineName)))
				{
					bool state = selectWindowIsOpen[SMOKTOOL_ACT_GUI_ENTITY_INSPECTOR_SELECT_WINDOW_BOOL_ARRAY_INDEX_GRAPHICS_PIPELINE];
					memset(selectWindowIsOpen, false, sizeof(selectWindowIsOpen));
					selectWindowIsOpen[SMOKTOOL_ACT_GUI_ENTITY_INSPECTOR_SELECT_WINDOW_BOOL_ARRAY_INDEX_GRAPHICS_PIPELINE] = !state;
				}

				//let the texture be selected
				if (ImGui::Button(selectedEntity->mesh.textureName.c_str(), Smok::TyGUI::Util::CalculateLabelSize(selectedEntity->mesh.textureName)))
				{
					bool state = selectWindowIsOpen[SMOKTOOL_ACT_GUI_ENTITY_INSPECTOR_SELECT_WINDOW_BOOL_ARRAY_INDEX_TEXTURE];
					memset(selectWindowIsOpen, false, sizeof(selectWindowIsOpen));
					selectWindowIsOpen[SMOKTOOL_ACT_GUI_ENTITY_INSPECTOR_SELECT_WINDOW_BOOL_ARRAY_INDEX_TEXTURE] = !state;
				}

				//let the sampler be selected

				ImGui::End();

				//lets the user select a mesh to replace it
				uint64 assetID = 0;
				if (selectWindowIsOpen[SMOKTOOL_ACT_GUI_ENTITY_INSPECTOR_SELECT_WINDOW_BOOL_ARRAY_INDEX_STATIC_MESH])
				{
					if (DrawMeshSelectWindow(assetManager, assetID))
					{
						Smok::Renderers::StaticMesh* asset = assetManager->GetStaticMesh(assetID, true);
						//if (!asset)
						//	selectedEntity->mesh.staticMeshName = "";
						if(asset)
						{
							selectedEntity->mesh.staticMeshAssetID = assetID;
							selectedEntity->mesh.staticMeshName = assetManager->GetNameByID(assetID);
							selectedEntity->mesh.meshIndexsToRender = asset->megaMeshBufferIndexes;
							BTD_LogMessage(std::string("New Mesh: " + selectedEntity->mesh.staticMeshName).c_str());
							sceneIsDirty = true;
						}

						selectWindowIsOpen[SMOKTOOL_ACT_GUI_ENTITY_INSPECTOR_SELECT_WINDOW_BOOL_ARRAY_INDEX_STATIC_MESH] = false;
					}
				}

				////lets the user select a graphics shader to replace it
				//if (selectWindowIsOpen[SMOKTOOL_ACT_GUI_ENTITY_INSPECTOR_SELECT_WINDOW_BOOL_ARRAY_INDEX_GRAPHICS_SHADER])
				//{
				//	Data::ACTAssetData_GraphicsShader* shader = nullptr;
				//	if (DrawGraphicsShaderSelectWindow(project, shader))
				//	{
				//		selectedEntity->mesh.graphicsShaderName = project->GetAssetNameByID(shader->ACTDatabaseID);
				//		sceneIsDirty = true;
				//	}
				//}
			}
		}
	};
}
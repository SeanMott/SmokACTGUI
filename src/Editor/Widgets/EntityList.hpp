#pragma once

//defines the entity list that allows the user to build a scene of models

#include <ECS/Scene.hpp>

#include <SmokTyGUI/DrawCommands.hpp>

#include <SmokRenderers/AssetManager.hpp>

namespace SmokTool::ACT::GUI
{
	//defines a widget for entities
	struct EntityListWidget
	{
		Smok::Renderers::AssetManager* assetManager = nullptr;

		TableToolkit::ECS::Scene::RenderObject* selectedEntity = nullptr;

		TableToolkit::ECS::Scene::SceneRenderData sceneRenderData;

		//draws the entities and updates their data
		inline void Draw()
		{
			ImGui::Begin("Entity List");

			//adds a entity
			if (ImGui::Button("+", Smok::TyGUI::Util::CalculateLabelSize("+")))
			{
				sceneRenderData.CreateEntity();

				////checks if name can be used
				//uint32 count = 0;
				//std::string name = "Entity";
				//while (sceneRenderData.IsNameAlready(name.c_str()))
				//{
				//	count++;
				//	name = "Entity " + BTD::Util::ToString(count);
				//}

				////creates entity
				//TableToolkit::ECS::Scene::RenderObject* obj = &sceneRenderData.renderObjects.emplace_back(TableToolkit::ECS::Scene::RenderObject());
				//obj->entityInfo.entityName = name;

				//obj->transform.position = { 0.0f, 0.0f, 0.0f };
				//obj->transform.scale = { 1.0f, 1.0f, 1.0f };

				////obj->mesh.meshDeclPath = project->GetStaticMesh("Table")->declDataPath;
				//obj->mesh.staticMeshName = "Table";

				////SmokTool::ACT::Data::ACTAssetData_GraphicsShader* shader = project->GetGraphicsShader("DefaultGShader");
				////obj->mesh.vertShaderPath = shader->compiledVertFilePath;
				////obj->mesh.fragShaderPath = shader->compiledFragFilePath;
				//obj->mesh.graphicsShaderName = "DefaultGShader";

				////obj->mesh.textureDeclPath = project->GetTexture2D("BlankTexture")->handle.declPath;
				//obj->mesh.textureName = "BlankTexture";
				////obj->mesh.sampler2DDeclPath = project->GetSampler2D("BlankSampler")->handle.declPath;
				//obj->mesh.samplerNane = "BlankSampler";

				//sceneRenderData.sceneIsDirty = true;
			}

			ImGui::SameLine();

			//saves the scene to a file
			//if (ImGui::Button("Save", Smok::TyGUI::Util::CalculateLabelSize("Save")))
			//{
				//sceneRenderData.SaveToFile("res/Projects", "Save");
			//}

			//ImGui::SameLine();

			//loads the scene from a file
			//if (ImGui::Button("Load", Smok::TyGUI::Util::CalculateLabelSize("Load")))
			//{
			//}

			ImGui::Spacing(); ImGui::Spacing(); ImGui::Spacing();

			//renders the list of entities
			for (uint32 i = 0; i < sceneRenderData.renderObjects.size(); ++i)
			{
				if (ImGui::Button(sceneRenderData.renderObjects[i].entityInfo.entityName.c_str(),
					Smok::TyGUI::Util::CalculateLabelSize(sceneRenderData.renderObjects[i].entityInfo.entityName)))
				{
					selectedEntity = &sceneRenderData.renderObjects[i];
				}
			}

			//right click context

			ImGui::End();
		}
	};
}
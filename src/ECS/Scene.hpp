#pragma once

//defines various structs for rendering and scene construction in Table Toolkit

#include <SmokRenderers/Renderers/GPUBasedMeshRenderer.hpp>

#include <SmokECS/Components/Mesh.hpp>

#include <BTDSTD/IO/YAML.hpp>

namespace TableToolkit::ECS::Scene
{
	//defines a renderable object in the table top
	struct RenderObject
	{
		BTD::Math::Transform transform;
		Smok::ECS::Component::Mesh mesh;
		Smok::ECS::Component::EntityInfo entityInfo;

		//converts the Render Object into YAML data
		inline void ConvertToYAML(YAML::Emitter& emitter)
		{
			emitter << YAML::BeginMap;

			Smok::ECS::Component::EntityInfo_ConvertToYAML(emitter, &entityInfo);

			BTD::Math::Transform_ConvertToYAML(emitter, &transform);

			Smok::ECS::Component::Mesh_ConvertToYAML(emitter, &mesh);

			emitter << YAML::EndMap;
		}

		//converts YAML Data into a Render Object
		inline void ConvertFromYAML(YAML::Node& data)
		{
			Smok::ECS::Component::EntityInfo_ConvertFromYAML(&entityInfo, data);

			BTD::Math::Transform_ConvertFromYAML(&transform, data);

			Smok::ECS::Component::Mesh_ConvertFromYAML(&mesh, data);
		}
	};

	//defines a base render data for a scene
	struct SceneRenderData
	{
		bool sceneIsDirty = false;

		//the raw objects
		std::vector<RenderObject> renderObjects;

		//the structs making up the batched render data to send to the GPU
		std::vector<Smok::Renderers::GPUBased::MeshRenderer::RenderBatch> renderBatches;
		std::vector<Smok::Renderers::GPUBased::MeshRenderer::ObjectBuffer_Object> objectBufferObjects;
		std::vector<Smok::Renderers::GPUBased::MeshRenderer::ObjectBatch_Object> batchObjects;

		//validates if a name already exists in the raw render objects
		inline bool IsNameAlready(const char* name)
		{
			//if the name already exists
			for (uint32 i = 0; i < renderObjects.size(); ++i)
			{
				if (!strcmp(name, renderObjects[i].entityInfo.entityName.c_str()))
					return true;
			}

			return false;
		}

		//adds a entity
		inline TableToolkit::ECS::Scene::RenderObject* AddEntity(const RenderObject& _obj)
		{
			//checks if name can be used
			uint32 count = 0;
			std::string name = _obj.entityInfo.entityName;
			while (IsNameAlready(name.c_str()))
			{
				count++;
				name = _obj.entityInfo.entityName + BTD::Util::ToString(count);
			}

			//creates entity
			TableToolkit::ECS::Scene::RenderObject* obj = &renderObjects.emplace_back(TableToolkit::ECS::Scene::RenderObject());
			obj->entityInfo.entityName = name;

			obj->transform = _obj.transform;

			obj->mesh = _obj.mesh;

			sceneIsDirty = true;
		
			return obj;
		}

		//removes a entity

		//gets a entity
		inline RenderObject* GetEntityByName(const char* name)
		{
			for (uint32 i = 0; i < renderObjects.size(); ++i)
			{
				if (!strcmp(name, renderObjects[i].entityInfo.entityName.c_str()))
					return &renderObjects[i];
			}

			return nullptr;
		}

		//creates a default entity
		inline RenderObject* CreateEntity(const char* entityName = "Entity")
		{
			TableToolkit::ECS::Scene::RenderObject obj;
			obj.entityInfo.entityName = entityName;
			return AddEntity(obj);
		}

		////saves the scene to a file
		//inline bool SaveToFile(const std::string& dir, const std::string& sceneName)
		//{
		//	YAML::Emitter emitter;

		//	emitter << YAML::BeginMap;

		//	emitter << YAML::Key << "sceneName" << YAML::DoubleQuoted << sceneName;

		//	//writes the entities
		//	emitter << YAML::Key << "Entities" << YAML::Value << YAML::BeginSeq;
		//	for (uint32 i = 0; i < renderObjects.size(); ++i)
		//		renderObjects[i].ConvertToYAML(emitter);

		//	emitter << YAML::EndSeq;

		//	emitter << YAML::EndMap;

		//	BTD::IO::File file;
		//	if (!file.Open(dir + "/" + sceneName + ".scene", BTD::IO::FileOP::TextWrite_OpenCreateStart))
		//	{
		//		BTD_LogError("Smok ACT GUI", "Scene", "SaveToFile", std::string("Failed to open/create a file at \"" + dir + "/" + sceneName + "\"").c_str());
		//		return false;
		//	}
		//	file.Write(emitter.c_str());

		//	return true;
		//}

		////loads the scene from a file
		//inline bool LoadFromFile(const std::string& filePath)
		//{
		//	BTD::IO::File file;
		//	if (!file.Open(filePath, BTD::IO::FileOP::TextRead_OpenExisting))
		//	{
		//		BTD_LogError("Smok ACT GUI", "Scene", "LoadFromFile", std::string("failed to open Scene file at \"" + filePath + "\"").c_str());
		//		return false;
		//	}

		//	auto data = YAML::Load(file.Read());
		//	file.Close();
		//	if (!data)
		//	{
		//		BTD_LogError("Smok ACT GUI", "Scene", "LoadFromFile", "Scene Data was mal formed from file!");
		//		return false;
		//	}

		//	std::string sceneName = data["sceneName"].as<std::string>();

		//	//goes through the entities
		//	renderObjects.reserve(10);
		//	auto entities = data["Entities"];
		//	for (auto& entity : entities)
		//	{
		//		RenderObject obj;
		//		obj.ConvertFromYAML(entity);
		//		AddEntity(obj);
		//	}

		//	return true;
		//}
	};
}
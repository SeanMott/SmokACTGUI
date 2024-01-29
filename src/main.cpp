/*
The GUI tool for converting assets from others to a Smok format
*/

#include <BTDSTD/Logger.hpp>

#include <BTDSTD_C/IO/Time.h>
#include <SmokGraphics/RenderCommands/RenderPass.h>

#include <BTDSTD/Math/RenderMath.hpp>
#include <BTDSTD/Math/Camera.hpp>
#include <BTDSTD/Input/Input.hpp>

#include <SmokGraphics/Core/GraphicContext.hpp>
#include <SmokGraphics/Pools/CommandPool.h>
#include <SmokGraphics/Pipeline/GraphicsPipeline.hpp>
#include <SmokGraphics/Utils/MeshBuffer.hpp>
#include <SmokGraphics/Pools/Descriptor.hpp>

#include <SmokWindow/Desktop/DesktopWindow.h>

#include <SmokMesh/MegaMeshBuffer.hpp>

#include <SmokTexture/Texture.hpp>

#include <SmokRenderers/RenderManager.hpp>
#include <SmokRenderers/Renderers/GPUBasedMeshRenderer.hpp>

#include <SmokECS/Components/EntityInfo.hpp>
#include <SmokECS/Components/Mesh.hpp>

#include <SmokTyGUI/WidgetRenderer.hpp>
#include <SmokTyGUI/Widgets/Button.hpp>
#include <SmokTyGUI/DrawCommands.hpp>

#include <Editor/Widgets/AssertBar.hpp>
#include <Editor/Widgets/EntityList.hpp>
#include <Editor/Widgets/EntityInspector.hpp>
#include <ECS/Scene.hpp>


BTD::Input::Input input; //input global

//gets controller input from the window

//gets mouse input from the window

//gets keyboard input from the window
void OnWindowKeyboardInputCallback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
	//updates keyboard
	input.lastKeyboardKey[key] = input.currentKeyboardKey[key];
	input.currentKeyboardKey[key] = action;
}

//when the window is attempted to close
void OnCloseWindowCallback(GLFWwindow* window)
{
	SMWindow_Desktop_Window_ShutdownWindow(((SMWindow_Desktop_Window*)glfwGetWindowUserPointer(window)));
}

//when the window is resized
void OnResizeWindowCallback(GLFWwindow* window, int width, int height)
{
	SMWindow_Desktop_Window* w = (SMWindow_Desktop_Window*)glfwGetWindowUserPointer(window);
	w->size = { width, height };
	w->isResized = true;
}

//defines a engine for storing all the boiler plate engine code
struct Engine
{
	Smok::Graphics::Core::GraphicsContext GContext;
	SMWindow_Desktop_Window window;
	SMGraphics_Core_GPU GPU;
	SMWindow_Desktop_Swapchain swapchain;
	VmaAllocator vulkanAllocator;

	BTD_IO_Time time;

	char* appName; bool isDebug = true;
};

//shutsdown the engine
static inline void ShutdownEngine(Engine* engine)
{
	if (engine->swapchain.swapchain != VK_NULL_HANDLE)
		SMWindow_Desktop_Swapchain_DestroySwapchain(&engine->swapchain, engine->GPU.device, engine->vulkanAllocator);

	if (engine->vulkanAllocator != VK_NULL_HANDLE)
		vmaDestroyAllocator(engine->vulkanAllocator);

	if (engine->GPU.gpu != VK_NULL_HANDLE)
		SMGraphics_Core_GPU_Destroy(&engine->GPU);

	if (engine->window.window != nullptr)
		SMWindow_Desktop_Window_DestroyWindow(&engine->window, engine->GContext.vulkanInstance);

	if(engine->GContext.vulkanInstance != VK_NULL_HANDLE)
		Smok::Graphics::Core::DeinitalizeGraphicsContext(engine->GContext);
}

//initalizes the engine
static BTD_Compiler_ForceInline bool InitalizeEngine(Engine* engine)
{
	//defines a struct of application data and initalizes the OS data
	Smok::Graphics::Core::GraphicsContextCreateInfo GContextCreateInfo;
	GContextCreateInfo.appName = "Smok ACT GUI";
	printf("%s\n", GContextCreateInfo.appName);
	GContextCreateInfo.platform = Smok::Graphics::Core::Platform::Windows_X64_Ver10;
	GContextCreateInfo.renderAPI = Smok::Graphics::Core::RenderAPI::Vulkan;
	GContextCreateInfo.isDebug = true;
	
	if (!Smok::Graphics::Core::InitalizeGraphicsContext(engine->GContext, GContextCreateInfo))
	{
		BTD::Logger::LogError(engine->GContext.applicationName, "Smok Graphics Graphic Context", "InitalizeEngine", "Failed to initalize the Graphics Context!");
		return false;
	}

	//creates window
	SMWindow_Desktop_Window_WindowCreateInfo windowCreateInfo;
	windowCreateInfo.windowName = GContextCreateInfo.appName;
	windowCreateInfo.size = { 1000, 800 };
	windowCreateInfo.onWindowCloseCallback = OnCloseWindowCallback;
	windowCreateInfo.onWindowResizeCallback = OnResizeWindowCallback;
	windowCreateInfo.onWindowKeyboardInputCallback = OnWindowKeyboardInputCallback;
	windowCreateInfo.shouldBeResizable = true;

	if (!SMWindow_Desktop_Window_Create(&engine->window, &windowCreateInfo, engine->GContext.vulkanInstance))
	{
		Smok::Graphics::Core::DeinitalizeGraphicsContext(engine->GContext);
		BTD::Logger::LogError(engine->GContext.applicationName, "Smok Graphics Window", "InitalizeEngine", "Failed to create a window!");
		return false;
	}

	//gets a GPU
	SMGraphics_Core_GPU_CreateInfo GPUCreateInfo;
	SMGraphics_Core_GPU_CreateInfo_InitalizeDefaultValues(&GPUCreateInfo);
	GPUCreateInfo.isRenderingToTheScreen = true;
	GPUCreateInfo.mainFeatures.samplerAnisotropy = true;
	
	//makes sure the device can use unbound arrays in shaders
	VkPhysicalDeviceDescriptorIndexingFeaturesEXT indexingFeatures{};
	indexingFeatures.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_DESCRIPTOR_INDEXING_FEATURES_EXT;
	indexingFeatures.pNext = nullptr;
	indexingFeatures.descriptorBindingPartiallyBound = VK_TRUE;
	indexingFeatures.runtimeDescriptorArray = VK_TRUE;
	GPUCreateInfo.deviceDescriptorIndexingFeaturesNeeded = &indexingFeatures;
	
	//make sure draw parameter is supported since it was moved to the Vulkan core
	VkPhysicalDeviceVulkan11Features vk11Features{};
	vk11Features.sType = VK_STRUCTURE_TYPE_PHYSICAL_DEVICE_VULKAN_1_1_FEATURES;
	vk11Features.shaderDrawParameters = VK_TRUE;
	vk11Features.pNext = &indexingFeatures;
	GPUCreateInfo.vk11Features = &vk11Features;

	if (!SMGraphics_Core_GPU_Create(&engine->GPU, GPUCreateInfo, engine->window.surface, engine->GContext.vulkanInstance))
	{
		ShutdownEngine(engine);
		BTD::Logger::LogError(engine->GContext.applicationName, "Smok Graphics GPU", "InitalizeEngine", "Failed to get a desired GPU!");
		return false;
	}

	//creates a Vulkan Memory Allocator
	VmaAllocatorCreateInfo allocatorInfo;
	allocatorInfo.flags = 0;
	
	allocatorInfo.physicalDevice = engine->GPU.gpu;
	allocatorInfo.device = engine->GPU.device;
	allocatorInfo.instance = engine->GContext.vulkanInstance;
	
	allocatorInfo.preferredLargeHeapBlockSize = 0;
	allocatorInfo.pAllocationCallbacks = NULL;
	allocatorInfo.pDeviceMemoryCallbacks = NULL;
	
	allocatorInfo.pHeapSizeLimit = NULL;
	allocatorInfo.pVulkanFunctions = NULL;
	allocatorInfo.vulkanApiVersion = VK_API_VERSION_1_3;
	allocatorInfo.pTypeExternalMemoryHandleTypes = NULL;
	
	if (vmaCreateAllocator(&allocatorInfo, &engine->vulkanAllocator) != VK_SUCCESS)
	{
		BTD::Logger::LogError(engine->GContext.applicationName, "Smok Graphics Vulkan Memory Allocator", "InitalizeEngine", "Failed to create Vulkan Memory Allocator!");
		ShutdownEngine(engine);
		return false;
	}

	//creates swapchain
	SMWindow_Desktop_Swapchain_InitalizeDefaultValues(&engine->swapchain);
	if (!SMWindow_Desktop_Swapchain_CreateSwapchain(&engine->swapchain, engine->GPU.gpu, engine->GPU.device,
		engine->GPU.graphicsQueueIndex, engine->GPU.presentQueueIndex, 
		engine->window.window, engine->window.surface, engine->vulkanAllocator, VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL))
	{
		ShutdownEngine(engine);
		BTD::Logger::LogError(engine->GContext.applicationName, "Smok Graphics Swapchain", "InitalizeEngine", "Failed to create swapchain!");
		return false;
	}

	//sets the time step and initalizes the time struct
	engine->time.deltaTime = 0.0f;
	engine->time.lastFrame = 0.0f;

	engine->time.fixedDeltaTime = 0.0f;
	engine->time.accumulator = 0.0f;
	engine->time.timeStep = 0.2f;

	return true;
}

//entry point
int main()
{
	//---init

	//initalize boiler plate engine stuff
	Engine engine;
	if (!InitalizeEngine(&engine))
	{
		getchar();
		return -1;
	}

	SMWindow_Desktop_Window* window = &engine.window; SMWindow_Desktop_Swapchain* swapchain = &engine.swapchain;
	SMGraphics_Core_GPU* GPU = &engine.GPU;

	//initalize the render manager
	Smok::Renderers::RenderManager renderManager;
	if (!Smok::Renderers::InitRenderManager(&renderManager, GPU))
	{
		ShutdownEngine(&engine);
		BTD::Logger::LogFatalError(engine.appName, "Smok Render Graph Render Manager", "main", "Failed to init render manager!");
	}

	//creates a command buffer pool
	SMGraphics_Pool_CommandPool commandPool;
	SMGraphics_Pool_CommandPool_CreateCommandPool(&commandPool, GPU->device, GPU->graphicsQueueIndex);

	//allocate a command buffer
	SMGraphics_Pool_CommandBuffer comBuffer;
	SMGraphics_Pool_CommandPool_CreateCommandBuffer(&comBuffer, &commandPool, GPU->device, true);

	//initalizes asset manager
	Smok::Renderers::AssetManager assetManager;
	assetManager.Init(engine.vulkanAllocator, &engine.GPU);

	//registers the graphics shader
	Smok::Graphics::Pipeline::GraphicsShader* defaultGShader = assetManager.RegisterGraphicsShader("DefaultGShader",
		std::string(std::string("res/Shaders/DefaultGShader") + SMOK_GRAPHICS_GRAPHICS_SHADER_DECL_FILE_EXTENSION).c_str());

	//writes decl file
	Smok::Graphics::Pipeline::GraphicsShader_WriteDeclFile("res/Shaders",
		"DefaultGShader",
		"res/Shaders/Compiled/TextureMesh.vert.spv",
		"res/Shaders/Compiled/TextureMesh.frag.spv");

	//registers the graphics pipeline
	Smok::Graphics::Pipeline::GraphicsPipeline* defaultGPipeline = assetManager.RegisterGraphicsPipeline("DefaultGPipeline",
		"",
		defaultGShader->assetID);

	//registers the texture
	Smok::Texture::Texture* defaultTexture = assetManager.RegisterTexture("BlankTexture", "res/Textures/BirdsPlayingPoker.texdecl");

	//registers the sampler
	Smok::Graphics::Util::Image::Sampler2D* defaultSampler = assetManager.RegisterSampler2D("BlankSampler", "res/Textures/Default.sap2Ddecl");

	//registers the static mesh
	Smok::Renderers::StaticMesh* cube = assetManager.RegisterStaticMesh("Cube",
		std::string(std::string("res/Meshes/") + "Cube" + SMOK_MESH_MESH_DATA_DECL_EXTENTION).c_str());

	//registers the static mesh
	Smok::Renderers::StaticMesh* table = assetManager.RegisterStaticMesh("Table",
		std::string(std::string("res/Meshes/") + "Table" + SMOK_MESH_MESH_DATA_DECL_EXTENTION).c_str());

	//registers the static mesh
	//assetManager.RegisterStaticMesh("Kirby",
	//	std::string(std::string("res/Meshes/") + "Kirby" + SMOK_MESH_MESH_DATA_DECL_EXTENTION).c_str());

	//inits the GPU based mesh renderer
	Smok::Renderers::GPUBased::MeshRenderer::GPUMeshRenderer GPUMeshRenderer;
	GPUMeshRenderer.Init(GPU, engine.vulkanAllocator, swapchain, &commandPool,
		&assetManager,
		defaultTexture->assetID,
		defaultSampler->assetID);

	//defines the widget renderer
	Smok::TyGUI::Widget::WidgetRendererSettings widgetRendererSettings;
	widgetRendererSettings.isDockingEnabled = true; widgetRendererSettings.isSeprateViewportsEnabled = false;
	Smok::TyGUI::Widget::WidgetRenderer widgetRenderer;
	widgetRenderer.Init(&engine.GContext, GPU, window, swapchain, &commandPool, widgetRendererSettings);

	//widgets
	SmokTool::ACT::GUI::AssetBarWidget assetBarWidget;
	assetBarWidget.assetManager = &assetManager;

	SmokTool::ACT::GUI::EntityListWidget entityListWidget;
	entityListWidget.assetManager = &assetManager;
	
	SmokTool::ACT::GUI::EntityInspectorWidget entityInspectorWidget;
	entityInspectorWidget.assetManager = &assetManager;//.project = &projectData;

	//camera
	BTD::Math::Camera camera;
	camera.transform.position = { 0.0f, 4.0f, 25.0f };
	camera.projectionType = BTD::Math::CameraProjectionType::Perspective;
	camera.FOV = 75.f;
	camera.isBoundToDisplaySize = true; //updates with the screen size
	camera.nearClipping = 0.1f;
	camera.farClipping = 200.0f;

	camera.UpdateMatrixData({ (float)swapchain->extents.width, (float)swapchain->extents.height }); //updates the camera data
	 
	//defines a scene
	entityListWidget.sceneRenderData.renderObjects.reserve(2);
	entityListWidget.sceneRenderData.batchObjects.reserve(10);

	TableToolkit::ECS::Scene::RenderObject* obj = entityListWidget.sceneRenderData.CreateEntity("Unit");

	obj->transform.position = { 0.0f, 2.0f, 0.0f };
	obj->transform.scale = { 1.0f, 2.0f, 1.0f };

	obj->mesh.SetStaticMesh(cube->assetID, assetManager.GetNameByID(cube->assetID));
	
	obj->mesh.SetGraphicsShader(defaultGShader->assetID, assetManager.GetNameByID(defaultGShader->assetID));
	obj->mesh.SetGraphicsPipeline(defaultGPipeline->assetID, assetManager.GetNameByID(defaultGPipeline->assetID));

	obj->mesh.SetTexture(defaultTexture->assetID, assetManager.GetNameByID(defaultTexture->assetID));
	obj->mesh.SetSampler(defaultSampler->assetID, assetManager.GetNameByID(defaultSampler->assetID));

	obj = entityListWidget.sceneRenderData.CreateEntity("Unit 2");

	obj->transform.position = { 5.0f, 2.0f, 0.0f };
	obj->transform.scale = { 1.0f, 2.0f, 1.0f };

	obj->mesh.SetStaticMesh(table->assetID, assetManager.GetNameByID(table->assetID));

	obj->mesh.SetGraphicsShader(defaultGShader->assetID, assetManager.GetNameByID(defaultGShader->assetID));
	obj->mesh.SetGraphicsPipeline(defaultGPipeline->assetID, assetManager.GetNameByID(defaultGPipeline->assetID));

	obj->mesh.SetTexture(defaultTexture->assetID, assetManager.GetNameByID(defaultTexture->assetID));
	obj->mesh.SetSampler(defaultSampler->assetID, assetManager.GetNameByID(defaultSampler->assetID));

	obj = entityListWidget.sceneRenderData.CreateEntity("Floor");

	obj->transform.position = { 0.0f, -10.0f, 0.0f };
	obj->transform.scale = { 10.0f, 1.0f, 10.0f };

	obj->mesh.SetStaticMesh(table->assetID, assetManager.GetNameByID(table->assetID));

	obj->mesh.SetGraphicsShader(defaultGShader->assetID, assetManager.GetNameByID(defaultGShader->assetID));
	obj->mesh.SetGraphicsPipeline(defaultGPipeline->assetID, assetManager.GetNameByID(defaultGPipeline->assetID));

	obj->mesh.SetTexture(defaultTexture->assetID, assetManager.GetNameByID(defaultTexture->assetID));
	obj->mesh.SetSampler(defaultSampler->assetID, assetManager.GetNameByID(defaultSampler->assetID));

	entityListWidget.sceneRenderData.sceneIsDirty = true;

	//defines a render pass
	SMGraphics_RenderCommands_RenderPass renderPass;
	SMGraphics_RenderCommands_RenderPass_Create(&renderPass, &BTD_Math_Color_Blue(), "Main Mesh Pass");

	//---game loop
	//float rotation = 0.0f;
	while (window->isRunning)
	{
		//--poll input
		glfwPollEvents();
		//BTD::Input::PollInput();

		//calculate fixed and delta time
		BTD_IO_Time_CalFixedDeltaTime(&engine.time);
		BTD_IO_Time_CalDeltaTime(&engine.time);

		//--update

		//updates camera
		if (camera.isDirty)
		{
			Smok::Renderers::GPUBased::MeshRenderer::CameraBuffer camBuffer;
			camBuffer.P[0] = camera.P;
			camBuffer.V[0] = camera.V;
			camBuffer.PV[0] = camera.P * camera.V;
		
			GPUMeshRenderer.UpdateCamera(&camBuffer);
			
			camera.isDirty = false;
		}

		//--render

		//starts a new frame
		Smok::Renderers::Frame frame;

		//if frame failed or display was resized, we don't bother rendering
		if (window->isResized || !Smok::Renderers::NextFrame(&renderManager, GPU, swapchain, frame))
		{
			//remake the swapchain
			SMWindow_Desktop_Swapchain_RemakeSwapchain(swapchain, GPU->device, GPU->gpu, GPU->graphicsQueueIndex, GPU->presentQueueIndex,
				window->window, window->surface, engine.vulkanAllocator);
		
			//remake the graphics pipeline
			assetManager.RemakeGraphicsPipelines(swapchain->renderpass);

			//resizes any cameras bound to the screen
			camera.UpdateMatrixData({ (float)swapchain->extents.width, (float)swapchain->extents.height });
		
			window->isResized = false;
			continue;
		}

		//updates the scene
		if (entityListWidget.sceneRenderData.sceneIsDirty)
		{
			//purges old objects
			GPUMeshRenderer.PurgeAllObjects(frame.frameIndex);
			entityListWidget.sceneRenderData.batchObjects.clear();

			//adds new objects
			for (uint32 i = 0; i < entityListWidget.sceneRenderData.renderObjects.size(); ++i)
			{
				GPUMeshRenderer.AddObject(&entityListWidget.sceneRenderData.renderObjects[i].transform,
					entityListWidget.sceneRenderData.renderObjects[i].mesh.staticMeshAssetID,
					entityListWidget.sceneRenderData.renderObjects[i].mesh.graphicsShaderAssetID,
					entityListWidget.sceneRenderData.renderObjects[i].mesh.graphicsPipelineAssetID,
					entityListWidget.sceneRenderData.renderObjects[i].mesh.textureAssetID,
					entityListWidget.sceneRenderData.renderObjects[i].mesh.samplerAssetID,
					entityListWidget.sceneRenderData.batchObjects);
			}

			//calculates the frame
			GPUMeshRenderer.CalculateCommandData(entityListWidget.sceneRenderData.batchObjects,
				entityListWidget.sceneRenderData.renderBatches, entityListWidget.sceneRenderData.objectBufferObjects);
		
			entityListWidget.sceneRenderData.sceneIsDirty = false;
		}

		SMGraphics_Pool_CommandBuffer_StartRecording(&comBuffer, false, false, false); //starts recording

		//starts a mesh render pass
		SMGraphics_RenderCommands_RenderPass_StartRenderPass(&renderPass, comBuffer.comBuffer,
			frame.framebuffer, swapchain->renderpass, frame.frameSize, { 0, 0 });

		//renders the active scene
		GPUMeshRenderer.Render(comBuffer.comBuffer, frame,
			entityListWidget.sceneRenderData.renderBatches, entityListWidget.sceneRenderData.objectBufferObjects);

		//ends the render pass
		SMGraphics_RenderCommands_RenderPass_EndRenderPass(&renderPass, comBuffer.comBuffer);

		//renders the widgets
		widgetRenderer.StartFrame(comBuffer.comBuffer, frame.framebuffer, frame.frameSize);

		//draws the widgets
		assetBarWidget.Draw(engine.window.window); //stores the assets to select
		entityListWidget.Draw(); //lists out and entities and lets new ones be made
		entityInspectorWidget.Draw(entityListWidget.selectedEntity, entityListWidget.sceneRenderData.sceneIsDirty); //shows the settings for modifying the entity

		widgetRenderer.SubmitFrame(comBuffer.comBuffer);

		SMGraphics_Pool_CommandBuffer_StopRecording(&comBuffer); //stops recording

		//submits the frame
		Smok::Renderers::SubmitFrame(&renderManager, GPU, &swapchain->swapchain, frame, &comBuffer.comBuffer, 1);
	}
	vkDeviceWaitIdle(engine.GPU.device);

	//---clean up

	//cleans up the widget renderer
	widgetRenderer.Shutdown();

	//cleans up mesh renderer
	GPUMeshRenderer.Shutdown();

	assetManager.Destroy();

	SMGraphics_Pool_CommandPool_DestroyCommandBuffer(&comBuffer, &commandPool, GPU->device);
	SMGraphics_Pool_CommandPool_DestroyCommandPool(&commandPool, GPU->device);

	Smok::Renderers::ShutdownRenderManager(&renderManager, GPU);

	//shutsdown the engine
	ShutdownEngine(&engine);

	getchar();
	return 0;
}
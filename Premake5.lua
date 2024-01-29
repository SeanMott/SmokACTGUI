workspace "SmokTools"
architecture "x64"
    startproject "SmokACTGUI"

configurations
{
    "Debug",
    "Release",
    "Dist"
}

outputdir = "%{cfg.buildcfg}-%{cfg.system}-%{cfg.architecture}"

include "C:\\GameDev\\SmokLibraries\\Engine\\SmokRenderers"
include "C:\\GameDev\\SmokLibraries\\Engine\\SmokWindow"
include "C:\\GameDev\\SmokLibraries\\Engine\\SmokECS"
include "C:\\GameDev\\SmokLibraries\\Engine\\SmokMesh"
include "C:\\GameDev\\SmokLibraries\\Engine\\SmokTexture"
include "C:\\GameDev\\SmokLibraries\\Engine\\SmokTyGUI"
include "C:\\GameDev\\SmokLibraries\\Tools\\SmokAssetConvertionTool"

project "SmokACTGUI"
kind "ConsoleApp"
language "C++"
            
targetdir ("bin/" .. outputdir .. "/%{prj.name}")
objdir ("bin-obj/" .. outputdir .. "/%{prj.name}")
    
    
files 
{
    "src/**.h",
    "src/**.c",
    "src/**.hpp",
    "src/**.cpp",
}

includedirs
{
    "src",

    "C:\\GameDev\\Libraries\\yaml-cpp\\include",
    "C:\\GameDev\\Libraries\\glm",
    "C:\\GameDev\\Libraries\\glfw\\include",
    "C:\\GameDev\\Libraries\\STB_Image",
    
    "C:\\VulkanSDK\\1.3.239.0\\Include",
    "C:\\GameDev\\Libraries\\VulkanMemoryAllocator\\include",

    "C:\\GameDev\\BTDSTD/includes",
    "C:\\GameDev\\BTDSTD_C/includes",

    "C:/GameDev/Libraries/imgui",

    "C:\\GameDev\\Libraries/Assimp",
    
    "C:\\GameDev\\SmokLibraries\\Engine\\SmokGraphics\\includes",
    "C:\\GameDev\\SmokLibraries\\Engine\\SmokWindow\\includes",
    "C:\\GameDev\\SmokLibraries\\Engine\\SmokECS\\includes",
    "C:\\GameDev\\SmokLibraries\\Engine\\SmokRenderers\\includes",
    "C:\\GameDev\\SmokLibraries\\Engine\\SmokMesh\\includes",
   "C:\\GameDev\\SmokLibraries\\Engine\\SmokTexture\\includes",
    "C:\\GameDev\\SmokLibraries\\Engine\\SmokTyGUI\\includes",
    "C:\\GameDev\\SmokLibraries\\Tools\\SmokAssetConvertionTool\\includes"  
}

links
{
    "SmokWindow",
    "SmokRenderers",
    "SmokMesh",
    "SmokECS",
    "SmokTexture",
    "SmokTyGUI",
    "SmokACT"
}
    
defines
{
    "GLFW_STATIC",
    "GLFW_INCLUDE_NONE",
}

flags
{
    --"LinkTimeOptimization",
    "NoRuntimeChecks",
    "MultiProcessorCompile"
}

--platforms
filter "system:windows"
cppdialect "C++17"
staticruntime "On"
systemversion "latest"

defines
{
    "Window_Build",
    "Desktop_Build"
}

--configs
filter "configurations:Debug"
defines "SMOK_DEBUG"
symbols "On"

links
{

}

filter "configurations:Release"
defines "SMOK_RELEASE"
optimize "On"

flags
{
    -- "LinkTimeOptimization"
}

filter "configurations:Dist"
defines "SMOK_DIST"
optimize "On"

defines
{
    "NDEBUG"
}

flags
{
    -- "LinkTimeOptimization"
}

links
{
   
}
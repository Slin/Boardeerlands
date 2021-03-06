cmake_minimum_required(VERSION 2.8.4)
include(ExternalProject)

project(Boardeerlands)

set(CMAKE_XCODE_GENERATE_TOP_LEVEL_PROJECT_ONLY ON)

option(RN_BUILD_BULLET_MODULE "Build the Bullet physics module" OFF)
option(RN_BUILD_PHYSX_MODULE "Build the PhysX physics module" ON)
option(RN_BUILD_ODE_MODULE "Build the ODE physics module" OFF)
option(RN_BUILD_SPLASH_MODULE "Build the Splash physics module" OFF)
option(RN_BUILD_ASSIMP_MODULE "Build the Assimp module" OFF)
option(RN_BUILD_STEAMAUDIO_MODULE "Build the Steam Audio module" OFF)
option(RN_BUILD_OCULUSAUDIO_MODULE "Build the Oculus Audio module" OFF)
option(RN_BUILD_ENET_MODULE "Build the ENet module" OFF)
option(RN_BUILD_SAMPLES "Enable building the sample projects" OFF)
option(RN_BUILD_RECAST_MODULE "Build the Recast pathfinding module" OFF)

set(RN_BUILD_CONFIGURATION "independent" CACHE STRING "The build configuration.")
set_property(CACHE RN_BUILD_CONFIGURATION PROPERTY STRINGS independent oculus steam)

option(RN_BUILD_OGG_MODULE "Build the ogg audio file module" ON)
option(RN_BUILD_OPENAL_MODULE "Build the OpenAL module" ON)
option(RN_BUILD_UI_MODULE "Build the UI module" OFF)
if(WIN32)
	set(CMAKE_SYSTEM_VERSION "10.0.10240.0")

	if(MSVC)
		option(RN_BUILD_VULKAN_MODULE "Build the Vulkan renderer module" OFF)
		option(RN_BUILD_D3D12_MODULE "Build the D3D12 renderer module" ON)
		option(RN_BUILD_OCULUS_MODULE "Build the Oculus module" ON)
		option(RN_BUILD_OPENVR_MODULE "Build the OpenVR module" ON)
	else()
		option(RN_BUILD_D3D12_MODULE "Build the D3D12 renderer module" OFF)
		option(RN_BUILD_VULKAN_MODULE "Build the Vulkan renderer module" ON)
		option(RN_BUILD_OPENVR_MODULE "Build the OpenVR module" ON)
	endif()
elseif(ANDROID)
	option(RN_BUILD_OCULUSMOBILE_MODULE "Build the Oculus GearVR module" ON)
	option(RN_BUILD_VULKAN_MODULE "Build the Vulkan renderer module" ON)
else()
	if(APPLE)
		option(RN_BUILD_VULKAN_MODULE "Build the Vulkan renderer module" OFF)
		option(RN_BUILD_OCULUS_MODULE "Build the Oculus module" OFF)
		option(RN_BUILD_OPENVR_MODULE "Build the OpenVR module" ON)
	else()
		option(RN_BUILD_VULKAN_MODULE "Build the Vulkan renderer module" ON)
		option(RN_BUILD_OCULUS_MODULE "Build the Oculus module" OFF)
		option(RN_BUILD_OPENVR_MODULE "Build the OpenVR module" ON)
	endif()
endif()


set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${PROJECT_BINARY_DIR}/Build)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${PROJECT_BINARY_DIR}/Build)
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${PROJECT_BINARY_DIR}/Build)

set(CMAKE_OSX_DEPLOYMENT_TARGET 10.12)

set(CMAKE_BUILD_WITH_INSTALL_RPATH TRUE)
if(APPLE)
	set(CMAKE_INSTALL_RPATH @executable_path;@executable_path/../Resources/Resources/Modules/RaynePhysX;@executable_path/../Resources/Resources/Modules/RayneOpenAL;@executable_path/../Resources/Resources/Modules/RayneMetal;@executable_path/../Resources/Resources/Modules/RayneOpenVR;@executable_path/../Resources/Resources/Modules/RayneVR)
elseif(UNIX)
	set(CMAKE_INSTALL_RPATH \$ORIGIN;\$ORIGIN/Resources/Modules/RaynePhysX;\$ORIGIN/Resources/Modules/RayneOpenAL;\$ORIGIN/Resources/Modules/RayneVulkan;\$ORIGIN/Resources/Modules/RayneOpenVR;\$ORIGIN/Resources/Modules/RayneVR)
endif()

add_subdirectory("${CMAKE_CURRENT_SOURCE_DIR}/../../Rayne" "${CMAKE_CURRENT_BINARY_DIR}/Rayne")

include(${CMAKE_CURRENT_SOURCE_DIR}/../../Rayne/CMake/Rayne.cmake)

set(SOURCES
	Sources/main.cpp
	Sources/BDApplication.cpp
	Sources/BDCameraManager.cpp
	Sources/BDPlayer.cpp
	Sources/BDBall.cpp
	Sources/BDBox.cpp
	Sources/BDBoxTrigger.cpp
	Sources/BDWater.cpp
	Sources/BDBridge.cpp
	Sources/BDChain.cpp
	Sources/BDBucket.cpp
	Sources/BDBucketTrigger.cpp
	Sources/BDExitDoor.cpp
	Sources/BDPlayerTrigger.cpp
	Sources/BDWorld.cpp)

set(HEADERS
	Sources/BDApplication.h
	Sources/BDCameraManager.h
	Sources/BDPlayer.h
	Sources/BDBall.h
	Sources/BDBox.h
	Sources/BDBoxTrigger.h
	Sources/BDWater.h
	Sources/BDBridge.h
	Sources/BDChain.h
	Sources/BDBucket.h
	Sources/BDBucketTrigger.h
	Sources/BDExitDoor.h
	Sources/BDPlayerTrigger.h
	Sources/BDWorld.h)

set(RESOURCES
	manifest.json
	Resources)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++14")

if(ANDROID)
    add_library(Boardeerlands SHARED "${SOURCES}" "${HEADERS}")
else()
    add_executable(Boardeerlands "${SOURCES}" "${HEADERS}")
endif()

target_include_directories(Boardeerlands PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/Sources)

set(ADDITIONAL_RESOURCE_COPY_PARAMS "")
rayne_copy_resources(Boardeerlands "${RESOURCES}" "${ADDITIONAL_RESOURCE_COPY_PARAMS}")

rayne_link_with(Boardeerlands)
rayne_use_modules(Boardeerlands RaynePhysX)
rayne_use_modules(Boardeerlands RayneVR)
rayne_use_modules(Boardeerlands RayneOpenAL)
rayne_use_modules(Boardeerlands RayneOgg)

if(ANDROID)
    rayne_use_modules(Boardeerlands RayneVulkan)
    rayne_use_modules(Boardeerlands RayneOculusMobile)

	if(${RN_BUILD_CONFIGURATION} STREQUAL "oculus")
        set(BD_COMPILE_DEFINITIONS ${BD_COMPILE_DEFINITIONS} -DBUILD_FOR_OCULUS)
    endif()
elseif(UNIX AND NOT APPLE)
	rayne_use_modules(Boardeerlands RayneVulkan)
	rayne_use_modules(Boardeerlands RayneOpenVR)

	if(${RN_BUILD_CONFIGURATION} STREQUAL "steam")
	    set(BD_COMPILE_DEFINITIONS ${BD_COMPILE_DEFINITIONS} -DBUILD_FOR_STEAM)
	endif()
else()
	if(NOT APPLE)
			if(${RN_BUILD_VULKAN_MODULE})
				rayne_use_modules(Boardeerlands RayneVulkan)
			endif()

			if(WIN32 AND ${RN_BUILD_D3D12_MODULE})
				rayne_use_modules(Boardeerlands RayneD3D12)
				rayne_use_modules(Boardeerlands RayneOculus)
			endif()

			add_custom_command(TARGET Boardeerlands POST_BUILD COMMAND ${CMAKE_COMMAND} -E copy "$<TARGET_FILE_DIR:RayneOpenAL>/OpenAL32.dll" "$<TARGET_FILE_DIR:Boardeerlands>/OpenAL32.dll")
	else()
		if(${RN_BUILD_METAL_MODULE})
	    	rayne_use_modules(Boardeerlands RayneMetal)
	    endif()
	endif()

	if(${RN_BUILD_CONFIGURATION} STREQUAL "steam")
	    rayne_use_modules(Boardeerlands RayneOpenVR)
	    set(BD_COMPILE_DEFINITIONS ${BD_COMPILE_DEFINITIONS} -DBUILD_FOR_STEAM)
	elseif(${RN_BUILD_CONFIGURATION} STREQUAL "oculus")
	    set(BD_COMPILE_DEFINITIONS ${BD_COMPILE_DEFINITIONS} -DBUILD_FOR_OCULUS)
	else()
		rayne_use_modules(Boardeerlands RayneOpenVR)

	    if(NOT APPLE)
		    add_custom_command(TARGET Boardeerlands POST_BUILD COMMAND ${CMAKE_COMMAND} -E copy ${CMAKE_CURRENT_SOURCE_DIR}/Other/RunPancake-windows.bat $<TARGET_FILE_DIR:Boardeerlands>/RunPancake-windows.bat)
		    add_custom_command(TARGET Boardeerlands POST_BUILD COMMAND ${CMAKE_COMMAND} -E copy ${CMAKE_CURRENT_SOURCE_DIR}/Other/.vrmanifest $<TARGET_FILE_DIR:Boardeerlands>/.vrmanifest)
		else()
			add_custom_command(TARGET Boardeerlands POST_BUILD COMMAND ${CMAKE_COMMAND} -E copy ${CMAKE_CURRENT_SOURCE_DIR}/Other/RunWithPreview-macOS.sh $<TARGET_BUNDLE_CONTENT_DIR:Boardeerlands>/../../RunWithPreview-macOS.sh)
		    add_custom_command(TARGET Boardeerlands POST_BUILD COMMAND ${CMAKE_COMMAND} -E copy ${CMAKE_CURRENT_SOURCE_DIR}/Other/RunPancake-macOS.sh $<TARGET_BUNDLE_CONTENT_DIR:Boardeerlands>/../../RunPancake-macOS.sh)
		    add_custom_command(TARGET Boardeerlands POST_BUILD COMMAND ${CMAKE_COMMAND} -E copy ${CMAKE_CURRENT_SOURCE_DIR}/Other/.vrmanifest $<TARGET_BUNDLE_CONTENT_DIR:Boardeerlands>/Resources/.vrmanifest)
		endif()
	endif()
endif()

target_compile_definitions(Boardeerlands PRIVATE ${BD_COMPILE_DEFINITIONS})

if(WIN32)
	add_custom_command(TARGET Boardeerlands PRE_BUILD COMMAND ${CMAKE_CURRENT_SOURCE_DIR}/Assets/Shaders/CompileShaders.bat)
else()
	add_custom_command(TARGET Boardeerlands PRE_BUILD COMMAND ${CMAKE_CURRENT_SOURCE_DIR}/Assets/Shaders/CompileShaders.sh)
endif()

if(APPLE)
	set_target_properties(Boardeerlands PROPERTIES MACOSX_BUNDLE TRUE)
	set_target_properties(Boardeerlands PROPERTIES MACOSX_BUNDLE_INFO_PLIST ${CMAKE_CURRENT_SOURCE_DIR}/Other/Info.plist.in)
endif()

if(MSVC)
    set_property(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR} PROPERTY VS_STARTUP_PROJECT Boardeerlands)
endif()

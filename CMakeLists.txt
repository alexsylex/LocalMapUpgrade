cmake_minimum_required(VERSION 3.21)

########################################################################################################################
## Define project
########################################################################################################################

project(
	LocalMapUpgrade
	VERSION 1.1.0
	LANGUAGES CXX
)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_INTERPROCEDURAL_OPTIMIZATION ON)
set(CMAKE_INTERPROCEDURAL_OPTIMIZATION_DEBUG OFF)

configure_file(
	${CMAKE_CURRENT_SOURCE_DIR}/cmake/version.rc.in
	${CMAKE_CURRENT_BINARY_DIR}/version.rc
	@ONLY
)

file(GLOB_RECURSE headers "include/*.h")
file(GLOB_RECURSE sources "source/*.cpp")

source_group(
	TREE
		${CMAKE_CURRENT_SOURCE_DIR}
	FILES
		${headers}
		${sources}
) 

function(make_includable input_file output_file)
    file(READ ${input_file} content)
    set(content "R\"${delim}(\n${content})${delim}\"")
    file(WRITE ${output_file} "${content}")
endfunction(make_includable)

make_includable(shaders/ISLocalMap.hlsl include/ISLocalMap.hlsl.h)


########################################################################################################################
## Configure target DLL
########################################################################################################################

find_package(CommonLibSSE CONFIG REQUIRED)
find_package(spdlog REQUIRED CONFIG)
find_package(directxtk CONFIG REQUIRED)

add_commonlibsse_plugin(${PROJECT_NAME} SOURCES ${headers} ${sources})
add_library("${PROJECT_NAME}::${PROJECT_NAME}" ALIAS "${PROJECT_NAME}")

target_include_directories(
	${PROJECT_NAME}
	PRIVATE
		${CMAKE_CURRENT_SOURCE_DIR}/include
)

target_link_libraries( 
	${PROJECT_NAME}
	PRIVATE
		CommonLibSSE::CommonLibSSE
		spdlog::spdlog_header_only
		Microsoft::DirectXTK
)

if (MSVC)
	target_link_options(
		${PROJECT_NAME}
		PRIVATE
			"$<$<CONFIG:DEBUG>:/INCREMENTAL;/OPT:NOREF;/OPT:NOICF>"
			"$<$<CONFIG:RELEASE>:/INCREMENTAL:NO;/OPT:REF;/OPT:ICF>"
	)
endif()

target_precompile_headers(
	${PROJECT_NAME}
	PRIVATE
		include/PCH.h
)

target_sources(
	${PROJECT_NAME}
	INTERFACE
	"$<BUILD_INTERFACE:CommonLibSSE.natvis>"
)

########################################################################################################################
## Pre-link clean-up
########################################################################################################################

file(GLOB PRE_LINK_CLEAN_DIRS "${VCPKG_INSTALLED_DIR}/*")
list(REMOVE_ITEM PRE_LINK_CLEAN_DIRS "${VCPKG_INSTALLED_DIR}/${VCPKG_TARGET_TRIPLET}")

foreach(PRE_LINK_CLEAN_DIR ${PRE_LINK_CLEAN_DIRS})
	add_custom_command(TARGET ${PROJECT_NAME} PRE_LINK
		COMMAND ${CMAKE_COMMAND} -E rm -rf "${PRE_LINK_CLEAN_DIR}" 
	)
endforeach()

########################################################################################################################
## Automatic plugin deployment
########################################################################################################################

set(SKYRIM_SE_DIR "C:/Program Files (x86)/Steam/steamapps/common/Skyrim Special Edition")
set(SKYRIM_VR_DIR "C:/Program Files (x86)/Steam/steamapps/common/Skyrim VR")

if ("$ENV{RUNTIME_DISABLE_FLAGS}" STREQUAL "")
	set(COPY_SE YES)
	set(COPY_VR YES)
elseif ("$ENV{RUNTIME_DISABLE_FLAGS}" STREQUAL "-UENABLE_SKYRIM_VR")
	set(COPY_SE YES)
	set(COPY_VR NO)
elseif ("$ENV{RUNTIME_DISABLE_FLAGS}" STREQUAL "-UENABLE_SKYRIM_SE -UENABLE_SKYRIM_AE")
	set(COPY_SE NO)
	set(COPY_VR YES)
endif()

if (COPY_SE)
	add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
		COMMAND ${CMAKE_COMMAND} -E copy $<TARGET_FILE:${PROJECT_NAME}>
		"${SKYRIM_SE_DIR}/Data/SKSE/Plugins/$<TARGET_FILE_NAME:${PROJECT_NAME}>"
	)
endif()
if (COPY_VR)
	add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
		COMMAND ${CMAKE_COMMAND} -E copy $<TARGET_FILE:${PROJECT_NAME}>
		"${SKYRIM_VR_DIR}/Data/SKSE/Plugins/$<TARGET_FILE_NAME:${PROJECT_NAME}>"
	)
endif()

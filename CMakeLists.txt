# CMake entry point
cmake_minimum_required (VERSION 2.6)
project (p7)

find_package(OpenGL REQUIRED)

# Compile external dependencies 
add_subdirectory (external)

# On Visual 2005 and above, this module can set the debug working directory
list(APPEND CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/external/rpavlik-cmake-modules-1c73e35")
include(CreateLaunchers)
include(MSVCMultipleProcessCompile) # /MP

if(INCLUDE_DISTRIB)
	add_subdirectory(distrib)
endif(INCLUDE_DISTRIB)

include_directories(
	external/AntTweakBar-1.15/include/
	external/glfw-2.7.6/include/
	external/glm-0.9.4.0/
	external/glew-1.9.0/include/
  .
)

#link_directories(${CMAKE_SOURCE_DIR})

set(ALL_LIBS
	${OPENGL_LIBRARY}
	GLFW_276
  GLEW_190
  SOIL
)

add_definitions(
	-DTW_STATIC
	-DTW_NO_LIB_PRAGMA
	-DTW_NO_DIRECT3D
	-DGLEW_STATIC
	-D_CRT_SECURE_NO_WARNINGS
)


add_executable(p7
	main.cpp
	implementacje.cpp
	common/shader.cpp
	common/shader.hpp
)

target_link_libraries(p7
	${ALL_LIBS}
)
# Xcode and Visual working directories
set_target_properties(p7 PROPERTIES XCODE_ATTRIBUTE_CONFIGURATION_BUILD_DIR "${CMAKE_CURRENT_SOURCE_DIR}/")

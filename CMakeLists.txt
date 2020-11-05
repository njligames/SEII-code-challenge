cmake_minimum_required(VERSION 3.10.2 FATAL_ERROR)
project(ISHNE_Viewer VERSION 2.0.0 LANGUAGES CXX C)

# Sets the install directories defined by GNU
include(GNUInstallDirs)
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/${CMAKE_INSTALL_LIBDIR})
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/${CMAKE_INSTALL_LIBDIR})
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/${CMAKE_INSTALL_BINDIR})

# Allows usage of configure time content
include(FetchContent)

FetchContent_Declare(glfw3
		URL https://github.com/glfw/glfw/archive/3.3.2.tar.gz
		URL_MD5 865e54ff0a100e9041a40429db98be0b
		)
FetchContent_GetProperties(glfw3)
if(NOT glfw3_POPULATED)
	set("GLFW_BUILD_EXAMPLES" OFF)
	set("GLFW_BUILD_TESTS" OFF)
	set("GLFW_BUILD_DOCS" OFF)
	set("GLFW_INSTALL" OFF)
	FetchContent_Populate(glfw3)
	add_subdirectory(${glfw3_SOURCE_DIR} ${glfw3_BINARY_DIR})
endif()

add_executable(ISHNE_Viewer
		third_party/glad/glad/glad.h
		third_party/glad/glad/glad.c
		third_party/glad/KHR/khrplatform.h
		src/Main.cpp
        src/Ishne.h
        src/Ishne.cpp
		)
target_compile_features(ISHNE_Viewer PUBLIC c_std_11 cxx_std_17)
target_link_libraries(ISHNE_Viewer
		PUBLIC
		$<$<CXX_COMPILER_ID:GNU>:stdc++fs>
		glfw
		)
target_include_directories(ISHNE_Viewer PUBLIC
		$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>
		"third_party/glad" "third_party/msdf"
		)

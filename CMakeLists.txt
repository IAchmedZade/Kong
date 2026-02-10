cmake_minimum_required(VERSION 3.25.0)

project(kong)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

if (MSVC)
	add_compile_options(/utf-8)
endif()

add_subdirectory(external/SFML)
add_subdirectory(source)

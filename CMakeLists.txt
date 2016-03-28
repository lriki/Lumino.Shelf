cmake_minimum_required (VERSION 2.8.7)
project (LuminoShelf)

set(LN_LuminoShelf_Sources
	src/Common.h
	src/Main.cpp
	src/Manager.cpp
	src/Manager.h
)

include_directories(
	"${PROJECT_SOURCE_DIR}/external/Lumino.Core/include"
)

add_subdirectory(external/Lumino.Core)
link_libraries(LuminoCore_Static)

add_executable(${PROJECT_NAME} ${LN_LuminoShelf_Sources})

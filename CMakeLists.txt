cmake_minimum_required (VERSION 2.8.7)
project (LuminoShelf)

set(LN_LuminoShelf_Sources
	src/CategoryItem.cpp
	src/CategoryItem.h
	src/CategoryManager.cpp
	src/CategoryManager.h
	src/Common.h
	src/Main.cpp
	src/Manager.cpp
	src/Manager.h
	src/Page.cpp
	src/Page.h
	src/PageToc.cpp
	src/PageToc.h
)

include_directories(
	"${PROJECT_SOURCE_DIR}/external/Lumino.Core/include"
)

if (LN_MSVC_LINK_MULTI_THREAD_STATIC_RUNTIME)
	foreach (flag CMAKE_C_FLAGS CMAKE_C_FLAGS_DEBUG CMAKE_C_FLAGS_RELEASE CMAKE_CXX_FLAGS CMAKE_CXX_FLAGS_DEBUG CMAKE_CXX_FLAGS_RELEASE)
		if (${flag} MATCHES "/MD")
			string(REGEX REPLACE "/MD" "/MT" ${flag} "${${flag}}")
		endif()
		if (${flag} MATCHES "/MDd")
			string(REGEX REPLACE "/MDd" "/MTd" ${flag} "${${flag}}")
		endif()
	endforeach()
endif()

add_definitions(-DUNICODE)
add_definitions(-D_UNICODE)
	
add_subdirectory(external/Lumino.Core)
link_libraries(LuminoCore_Static)

add_executable(${PROJECT_NAME} ${LN_LuminoShelf_Sources})

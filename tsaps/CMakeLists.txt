project(Tsaps)

add_library(Tsaps 
"src/core/graphics_io_base.h" 
"src/${TARG}/graphics_io.h" 
"src/${TARG}/graphics_io.cpp" "src/core/vertex_array.cpp")
#"src/core/user_io_base.h"
#"src/${TARG}/user_io.h"
#"src/${TARG}/user_io.cpp")

#windows
if (TARG STREQUAL "windows")
	add_subdirectory("C:/git/lib/glfw" "C:/git/lib/glfw/out")
	add_subdirectory("C:/git/lib/glm" "C:/git/lib/glm/out")
	add_subdirectory("src/windows/Glad")
	target_link_libraries(Tsaps PUBLIC glfw glm Glad)
endif()

target_include_directories(Tsaps PUBLIC "src/${TARG}" "src/core")


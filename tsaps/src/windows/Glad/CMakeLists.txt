project(Glad)

add_library(Glad 
	"C:/git/lib/glad/include/glad/glad.h"
	"C:/git/lib/glad/src/glad.c"
)
target_include_directories(Glad PUBLIC "C:/git/lib/glad/include")
#target_sources(Tsaps PUBLIC "C:/git/lib/glad/src")
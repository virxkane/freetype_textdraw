
# Find includes in corresponding build directories
set(CMAKE_INCLUDE_CURRENT_DIR ON)

set(SRC_LIST
	fc-lang-data.c
)

add_library(fc-lang-data STATIC ${SRC_LIST})

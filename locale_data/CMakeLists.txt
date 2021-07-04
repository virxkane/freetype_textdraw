
# Find includes in corresponding build directories
set(CMAKE_INCLUDE_CURRENT_DIR ON)

set(SRC_LIST
	iso-639-2_data.c
	iso-639-3_data.c
	iso-3166-1_data.c
	iso-15924_data.c
)

add_library(locale_data STATIC ${SRC_LIST})

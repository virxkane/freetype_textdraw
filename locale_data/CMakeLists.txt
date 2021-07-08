
# Find includes in corresponding build directories
set(CMAKE_INCLUDE_CURRENT_DIR ON)

# Instruct CMake to run moc automatically when needed.
set(CMAKE_AUTOMOC ON)

set(SRC_LIST
	iso-639-2_data.c
	iso-639-3_data.c
	iso-3166-1_data.c
	iso-15924_data.c

	clocaledata.cpp
)

add_library(locale_data STATIC ${SRC_LIST})
target_link_libraries(locale_data Qt5::Core)

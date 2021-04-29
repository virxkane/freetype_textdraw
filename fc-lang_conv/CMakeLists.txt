
# Find includes in corresponding build directories
set(CMAKE_INCLUDE_CURRENT_DIR ON)

set(EXE_NAME fc-lang_conv)

set(SRC_LIST
	main.c
)

if(WIN32)
	set(SRC_LIST ${SRC_LIST}
		getline_win32.c
	)
endif(WIN32)

set(LDADD_LIBS)
if(WIN32)
	# -mconsole -> console application (with terminal screen)
	set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -mconsole")
endif(WIN32)

add_executable(${EXE_NAME} WIN32 ${SRC_LIST})
target_link_libraries(${EXE_NAME} ${LDADD_LIBS})

add_library(fc-lang-cat STATIC fc-lang-cat.c)

configure_file(update.sh.cmake ${CMAKE_CURRENT_BINARY_DIR}/update.sh @ONLY)

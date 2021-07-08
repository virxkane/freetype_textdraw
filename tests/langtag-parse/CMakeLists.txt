
# Find includes in corresponding build directories
set(CMAKE_INCLUDE_CURRENT_DIR ON)

include_directories(${CMAKE_SOURCE_DIR}/locale_data)

set(EXE_NAME langtag-parse)

# Instruct CMake to run moc automatically when needed.
set(CMAKE_AUTOMOC ON)

set(SRC_LIST
	main.cpp
)

set(LDADD_LIBS)
if(WIN32)
		# -mconsole -> console application (with terminal screen)
		set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -mconsole")
		#set(LDADD_LIBS ${LDADD_LIBS} -lpsapi)
endif(WIN32)

add_executable(${EXE_NAME} WIN32 ${SRC_LIST})
target_link_libraries(${EXE_NAME} Qt5::Core ${LDADD_LIBS} locale_data)

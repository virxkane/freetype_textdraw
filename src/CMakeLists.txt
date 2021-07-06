
# Find includes in corresponding build directories
set(CMAKE_INCLUDE_CURRENT_DIR ON)

set(EXE_NAME freetype-textdraw)

# Instruct CMake to run moc automatically when needed.
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTOUIC ON)

include_directories(${CMAKE_SOURCE_DIR}/fc-lang_conv)

set(SRC_LIST
	main.cpp
	mainwnd.cpp
	mainwnd.ui
	lowlevel_textrender.cpp
	lowlevel_textrender_private.cpp
	colorbox.cpp
	colorbox_private.cpp
)

set(LDADD_LIBS)
if(WIN32)
		# -mwindows -> window application (without terminal screen)
		set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -mwindows")
		#set(LDADD_LIBS ${LDADD_LIBS} -lpsapi)
endif(WIN32)

add_executable(${EXE_NAME} WIN32 ${SRC_LIST} ${QRC_GEN_FILES})
target_link_libraries(${EXE_NAME} Qt5::Core Qt5::Gui Qt5::Widgets ${FREETYPE_LIBRARIES} ${HARFBUZZ_LIBRARIES} ${LDADD_LIBS} fc-lang-data)
install(TARGETS ${EXE_NAME} RUNTIME DESTINATION bin)

configure_file(valgrind_check.sh.cmake ${CMAKE_CURRENT_BINARY_DIR}/valgrind_check.sh)

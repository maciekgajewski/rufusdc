project(kgui)

cmake_minimum_required(VERSION 2.6)
find_package(KDE4 REQUIRED)

set(
	SOURCES
	
	main.cpp
	mainwindow.cpp
	hubwidget.cpp
	connectdialog.cpp
	userinfo.cpp
	filelistwidget.cpp
	utils.cpp
	tabcontent.cpp
	tabwidget.cpp
	transferwidget.cpp
	clientthread.cpp
	transferinfo.cpp
)

kde4_add_ui_files(
	SOURCES
	
	mainwindow.ui
	hubwidget.ui
	connectdialog.ui
	transferwidget.ui
)

include_directories( ${KDE4_INCLUDES} ${RUFUSDC_SOURCE_DIR}  ${RUFUSDC_BINARY_DIR}/kgui )

add_definitions( -DQT_NO_KEYWORDS )
add_definitions( -DBUILDING_DCPP ) # required by DCPP source, ot only for building (is used in headers)
#set( CMAKE_CXX_FLAGS -fexceptions ) # ugly and non-portable. I liked the qmake's way of doing this better
#set( CMAKE_CXX_FLAGS -O0 )

kde4_add_executable(kgui ${SOURCES})
set( CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${KDE4_ENABLE_EXCEPTIONS}" )
set( CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -O0" )


target_link_libraries(
	kgui
	
	dcpp
	${KDE4_KDEUI_LIBS}
)



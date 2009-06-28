project(kgui)

cmake_minimum_required(VERSION 2.6)
find_package(KDE4 REQUIRED)

set(
	SOURCES
	
	main.cpp
	mainwindow.cpp
	hubwidget.cpp
	connectdialog.cpp
	client.cpp
	clientthread.cpp
	hub.cpp
	usermodel.cpp
	userinfo.cpp
	clientthreadhub.cpp
	filelistwidget.cpp
	utils.cpp
	tabcontent.cpp
	tabwidget.cpp
)

kde4_add_ui_files(
	SOURCES
	
	mainwindow.ui
	hubwidget.ui
	connectdialog.ui
)

include_directories( ${KDE4_INCLUDES} ${RUFUSDC_SOURCE_DIR} )
link_directories( ${RUFUSDC_BINARY_DIR}/rufusdc )

add_definitions( -DQT_NO_KEYWORDS )
set( CMAKE_CXX_FLAGS -fexceptions ) # ugly and non-portable. I liked the qmake's way of doing this better
set( CMAKE_CXX_FLAGS -O0 )

kde4_add_executable(kgui ${SOURCES})


target_link_libraries(
	kgui
	
	rufusdc
	${KDE4_KDEUI_LIBS}
	boost_filesystem-mt
	boost_thread-mt
	boost_regex-mt
	boost_signals-mt
	)



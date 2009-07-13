project(rufusdc)

cmake_minimum_required(VERSION 2.6)
set( CMAKE_BUILD_TYPE Debug )

set(
	SOURCES
	
	client.cpp
	hub.cpp
	operation.cpp
	adcmessage.cpp
	userinfo.cpp
	connection.cpp
	listener.cpp
	directconnection.cpp
	connectionrequest.cpp
	filelist.cpp
	download.cpp
	filedownload.cpp
	filelistdownload.cpp
	error.cpp
	downloadmap.cpp
	directconnectionmanager.cpp
	hubmanager.cpp
	downloadmanager.cpp
)


add_library( rufusdc ${SOURCES} )

# libraries to link
target_link_libraries(
	rufusdc
	
	boost_filesystem-mt
	boost_thread-mt
	boost_regex-mt
	boost_signals-mt
	
	bz2
)



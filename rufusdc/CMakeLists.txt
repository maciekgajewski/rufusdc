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
	activedownload.cpp
	downloadrequest.cpp
	filelist.cpp
)


add_library( rufusdc ${SOURCES} )

# libraries to link
target_link_libraries(
	rufusdc
	
	boost_system
	boost_thread
	boost_regex
	boost_signals
	
	bz2
)



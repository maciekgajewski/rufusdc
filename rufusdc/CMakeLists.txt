project(rufusdc)

cmake_minimum_required(VERSION 2.6)

set(
	SOURCES
	
	client.cpp
	hub.cpp
	operation.cpp
	adcmessage.cpp
	userinfo.cpp
)


add_library( rufusdc ${SOURCES} )

# libraries to link
target_link_libraries(
	rufusdc
	
	boost_system
	boost_thread
	boost_regex
	boost_signals
)



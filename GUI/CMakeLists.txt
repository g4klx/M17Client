cmake_minimum_required(VERSION 3.24 FATAL_ERROR)

project(M17GUI LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)

set(THREADS_PREFER_PTHREAD_FLAG ON)
find_package(Threads REQUIRED)

find_package(wxWidgets REQUIRED core adv)
include(${wxWidgets_USE_FILE})

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -g -O2 -Wall")

file(GLOB SRC
	App.cpp
	ChannelsEvent.cpp
	CallsignsEvent.cpp
	Conf.cpp
	DestinationsEvent.cpp
	ErrorEvent.cpp
	Frame.cpp
	GPSCompass.cpp
	GPSDialog.cpp
	GPSEvent.cpp
	Logger.cpp
	ReceiveData.cpp
	ReceiveEvent.cpp
	RSSIEvent.cpp
	TextEvent.cpp
	Thread.cpp
	TransmitEvent.cpp
	UDPReaderWriter.cpp
	Utils.cpp
)

add_executable(${PROJECT_NAME} ${SRC})
target_link_libraries(${PROJECT_NAME} Threads::Threads ${wxWidgets_LIBRARIES})

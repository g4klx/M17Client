cmake_minimum_required(VERSION 3.24 FATAL_ERROR)

project(M17Client LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)

option(AUDIO "audio API")
if(NOT AUDIO)
    if(${CMAKE_SYSTEM_NAME} STREQUAL "Linux")
	set(AUDIO alsa)
    else()
	set(AUDIO pulseaudio)
    endif()
endif()   

option(USE_HAMLIB "use HamLib" OFF)
option(USE_GPSD "use GPSD" OFF)
option(USE_GPIO "use GPIO for PTT" OFF)

set(THREADS_PREFER_PTHREAD_FLAG ON)
find_package(Threads REQUIRED)
find_package(PkgConfig REQUIRED)
find_package(Git REQUIRED)

pkg_check_modules(LIBSAMPLERATE REQUIRED samplerate)
include_directories(${LIBSAMPLERATE_INCLUDE_DIRS})
link_directories(${LIBSAMPLERATE_LIBRARY_DIRS})

set(gitversion_h ${CMAKE_SOURCE_DIR}/GitVersion.h)
execute_process(COMMAND ${GIT_EXECUTABLE} rev-parse HEAD OUTPUT_VARIABLE gitversion OUTPUT_STRIP_TRAILING_WHITESPACE)
set(gitversion "const char *gitversion = \"${gitversion}\"\;\n")
file(WRITE ${gitversion_h} ${gitversion})
message(STATUS "${gitversion_h} generated")

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -g -O3 -Wall")

if(${AUDIO} STREQUAL "alsa")
    set(AUDIO_SRC SoundALSA.cpp)
    pkg_check_modules(AUDIO_API REQUIRED alsa)
else()
    set(AUDIO_SRC SoundPulse.cpp)
    pkg_check_modules(AUDIO_API REQUIRED libpulse-simple)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -DUSE_PULSEAUDIO")
endif()
include_directories(${AUDIO_API_INCLUDE_DIRS})
link_directories(${AUDIO_API_LIBRARY_DIRS})

if(USE_HAMLIB)
    pkg_check_modules(HAMLIB REQUIRED hamlib)
    include_directories(${HAMLIB_INCLUDE_DIRS})
    link_directories(${HAMLIB_LIBRARY_DIRS})
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -DUSE_HAMLIB")
endif()

if(USE_GPSD)
    pkg_check_modules(GPSD REQUIRED libgps)
    include_directories(${GPSD_INCLUDE_DIRS})
    link_directories(${GPSD_LIBRARY_DIRS})
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -DUSE_GPSD")
endif()

if(USE_GPIO)
    pkg_check_modules(GPIO REQUIRED libgpiod)
    include_directories(${GPIO_INCLUDE_DIRS})
    link_directories(${GPIO_LIBRARY_DIRS})
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -DUSE_GPIO")
endif()

file(GLOB SRC
	codec2/codebooks.cpp
	codec2/codec2.cpp
	codec2/kiss_fft.cpp
	codec2/lpc.cpp
	codec2/nlp.cpp
	codec2/pack.cpp
	codec2/qbase.cpp
	codec2/quantise.cpp
	CodePlug.cpp
	Conf.cpp
	Golay24128.cpp
	GPIO.cpp
	GPSD.cpp
	HamLib.cpp
	Log.cpp
	M17Client.cpp
	M17Convolution.cpp
	M17CRC.cpp
	M17LSF.cpp
	M17RX.cpp
	M17TX.cpp
	M17Utils.cpp
	Modem.cpp
	ModemPort.cpp
	RSSIInterpolator.cpp
	StopWatch.cpp
	Thread.cpp
	Timer.cpp
	UARTController.cpp
	UDPSocket.cpp
	Utils.cpp
	${AUDIO_SRC}
)

add_executable(${PROJECT_NAME} ${SRC})
target_link_libraries(${PROJECT_NAME} Threads::Threads ${LIBSAMPLERATE_LIBRARIES} ${AUDIO_API_LIBRARIES} ${HAMLIB_LIBRARIES} ${GPSD_LIBRARIES} ${GPIO_LIBRARIES})

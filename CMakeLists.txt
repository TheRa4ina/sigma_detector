#Top-level
cmake_minimum_required(VERSION 3.19.3)
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED On)
set(CMAKE_CXX_EXTENSIONS Off)

set(THREADS_PREFER_PTHREAD_FLAG ON)

project(sigma_detector VERSION 0.1)

if(POLICY CMP0167) # or else its angry for no reason
  cmake_policy(SET CMP0167 NEW)
endif()
if (WIN32)# because boost lib needs it
  add_definitions(-D_WIN32_WINNT=0x0A00) # Assuming we are compiling on win 10
endif()# other targets are here https://learn.microsoft.com/en-us/cpp/porting/modifying-winver-and-win32-winnt?view=msvc-170

if(MSVC) # treat all files as utf8 encoded
    add_compile_options(/utf-8)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /utf-8")
endif()

find_package(Threads REQUIRED)
find_package(OpenGL REQUIRED)
find_package(glfw3 3 REQUIRED)
find_package(Boost 1.70 REQUIRED COMPONENTS system)

add_subdirectory(3rd_party)
enable_testing()
include(GoogleTest)

add_subdirectory(src)
cmake_minimum_required (VERSION 3.13)
project (qtscreenshot)

set(CMAKE_CXX_STANDARD 14)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

set (Boost_USE_STATIC_LIBS ON)
find_package (Boost REQUIRED COMPONENTS
  thread date_time system program_options chrono)
find_package (Threads)
find_package(Qt5
  REQUIRED COMPONENTS
  Core Gui Network Widgets Multimedia MultimediaWidgets)

add_subdirectory (nseit-cpp-base)
add_subdirectory (src)

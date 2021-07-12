cmake_minimum_required(VERSION 3.0.0)
project(WindowsUtils)
set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} /MT")
set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} /MTd")

add_subdirectory(Audioswitch)
add_subdirectory(Iconfigure)

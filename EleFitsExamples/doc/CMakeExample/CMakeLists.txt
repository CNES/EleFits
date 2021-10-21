CMAKE_MINIMUM_REQUIRED(VERSION 2.8.5)
project(EleFitsCMakeExample)

find_package(EleFits REQUIRED)

add_executable(HelloFits HelloFits.cpp)
target_link_libraries(HelloFits EleFits)

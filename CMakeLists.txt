CMAKE_MINIMUM_REQUIRED(VERSION 2.8.5)

#===============================================================================
# Load macros and functions for Elements-based projects
#===============================================================================

find_package(ElementsProject)

set(USE_SPHINX OFF
    CACHE STRING "Sphinx API documentation" 
    FORCE)

#===============================================================================
# Declare project name and version
# Example with dependency:
#                         elements_project(MyProject 1.0 USE Element 3.9)
#===============================================================================

elements_project(EleFits 5.1 USE Elements 6.0.1
        DESCRIPTION "A modern C++ API on top of CFITSIO")

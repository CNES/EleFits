CMAKE_MINIMUM_REQUIRED(VERSION 2.8.12)

#===============================================================================
# Load macros and functions for Elements-based projects
#===============================================================================

find_package(ElementsProject)

set(USE_SPHINX OFF
    CACHE STRING "Disable Sphinx documentation" 
    FORCE)

#===============================================================================
# Declare project name and version
# Example with dependency:
#                         elements_project(MyProject 1.0 USE Element 3.9)
#===============================================================================

elements_project(EleFits 6.0 USE Linx 1.0
        DESCRIPTION "A modern C++ API on top of CFITSIO")

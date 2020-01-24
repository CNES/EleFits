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

elements_project(EL_FitsIO 1.1 USE Elements 5.8
        DESCRIPTION "A clean, object-oriented, template API on top of CFitsIO")

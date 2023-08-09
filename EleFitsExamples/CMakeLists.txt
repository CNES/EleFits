CMAKE_MINIMUM_REQUIRED(VERSION 2.8.5)

#===============================================================================
# Load elements_subdir macro here
# Examples:
#   For declaring a project module:
#         elements_subdir(ElementsExamples)
#===============================================================================
elements_subdir(EleFitsExamples)

#===============================================================================
# Load elements_depends_on_subdirs macro here
#   For creating a dependency onto an other accessible module
#         elements_depends_on_subdirs(ElementsKernel)
#===============================================================================
elements_depends_on_subdirs(ElementsKernel)
elements_depends_on_subdirs(EleFits EleFitsUtils)

#===============================================================================
# Add the find_package macro (a pure CMake command) here to locate the
# libraries.
# Examples:
#          find_package(CppUnit)
#===============================================================================

#===============================================================================
# Declare the library dependencies here
# Example:
#         elements_add_library(ElementsExamples src/Lib/*.cpp
#                     LINK_LIBRARIES Boost ElementsKernel
#                     INCLUDE_DIRS Boost ElementsKernel
#                     PUBLIC_HEADERS ElementsExamples)
#===============================================================================
elements_add_library(EleFitsExamples src/lib/*.cpp
                     INCLUDE_DIRS ElementsKernel EleFits EleFitsUtils
                     LINK_LIBRARIES ElementsKernel EleFits EleFitsUtils
                     PUBLIC_HEADERS EleFitsExamples)

#===============================================================================
# Declare the executables here
# Example:
# elements_add_executable(ElementsProgramExample src/Program/ProgramExample.cpp
#                        LINK_LIBRARIES Boost ElementsExamples
#                        INCLUDE_DIRS Boost ElementsExamples)
#===============================================================================
elements_add_executable(EleCfitsioExample src/program/EleCfitsioExample.cpp
                     LINK_LIBRARIES EleFitsExamples)
elements_add_executable(EleFitsBintableExample src/program/EleFitsBintableExample.cpp
                     LINK_LIBRARIES EleFitsExamples)
elements_add_executable(EleFitsExample src/program/EleFitsExample.cpp
                     LINK_LIBRARIES EleFitsExamples)
elements_add_executable(EleFitsGenerate2DMassFiles src/program/EleFitsGenerate2DMassFiles.cpp
                     LINK_LIBRARIES EleFitsExamples)
elements_add_executable(EleFitsGenerateAstroObj src/program/EleFitsGenerateAstroObj.cpp
                     LINK_LIBRARIES EleFitsExamples)
elements_add_executable(EleFitsGenerateLife src/program/EleFitsGenerateLife.cpp
                     LINK_LIBRARIES EleFitsExamples)
elements_add_executable(EleFitsTutorial src/program/EleFitsTutorial.cpp
                     LINK_LIBRARIES EleFitsExamples)

#===============================================================================
# Declare the Boost tests here
# Example:
# elements_add_unit_test(BoostClassExample tests/src/Boost/ClassExample_test.cpp
#                       EXECUTABLE BoostClassExample_test
#                       INCLUDE_DIRS ElementsExamples
#                       LINK_LIBRARIES ElementsExamples TYPE Boost)
#===============================================================================
elements_add_unit_test(GameOfLife tests/src/GameOfLife_test.cpp 
                     EXECUTABLE EleFitsExamples_GameOfLife_test
                     LINK_LIBRARIES EleFitsExamples
                     TYPE Boost)

#===============================================================================
# Use the following macro for python modules, scripts and aux files:
#  elements_install_python_modules()
#  elements_install_scripts()
#===============================================================================

#===============================================================================
# Declare the Python programs here
# Examples :
# elements_add_python_program(PythonProgramExample
#                             ElementsExamples.PythonProgramExample)
#===============================================================================

#===============================================================================
# Add the elements_install_conf_files macro
# Examples:
#          elements_install_conf_files()
#===============================================================================
elements_install_conf_files()
elements_install_aux_files()

CMAKE_MINIMUM_REQUIRED(VERSION 2.8.5)

#===============================================================================
# Load elements_subdir macro here
# Examples:
#   For declaring a project module:
#         elements_subdir(ElementsExamples)
#===============================================================================
elements_subdir(EL_FitsIO_Examples)

#===============================================================================
# Load elements_depends_on_subdirs macro here
#   For creating a dependency onto an other accessible module
#         elements_depends_on_subdirs(ElementsKernel)
#===============================================================================
elements_depends_on_subdirs(ElementsKernel)
elements_depends_on_subdirs(EL_FitsFile)

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

#===============================================================================
# Declare the executables here
# Example:
# elements_add_executable(ElementsProgramExample src/Program/ProgramExample.cpp
#                        LINK_LIBRARIES Boost ElementsExamples
#                        INCLUDE_DIRS Boost ElementsExamples)
#===============================================================================
elements_add_executable(EL_CfitsioWrapper_Example src/program/EL_CfitsioWrapper_Example.cpp
                     LINK_LIBRARIES ElementsKernel EL_FitsFile)
elements_add_executable(EL_FitsFile_Example src/program/EL_FitsFile_Example.cpp
                     LINK_LIBRARIES ElementsKernel EL_FitsFile)
elements_add_executable(EL_Cfitsio_Example src/program/EL_Cfitsio_Example.cpp
                     LINK_LIBRARIES ElementsKernel EL_FitsFile)
elements_add_executable(EL_FitsIO_GenerateAstroObj src/program/EL_FitsIO_GenerateAstroObj.cpp
                     LINK_LIBRARIES ElementsKernel EL_FitsFile)
elements_add_executable(EL_FitsIO_Tutorial src/program/EL_FitsIO_Tutorial.cpp
                     LINK_LIBRARIES ElementsKernel EL_FitsFile)
elements_add_executable(EL_FitsIO_Generate2DMassFiles src/program/EL_FitsIO_Generate2DMassFiles.cpp
                     LINK_LIBRARIES ElementsKernel EL_FitsFile)
elements_add_executable(EL_FitsIO_PrintSupportedTypes src/program/EL_FitsIO_PrintSupportedTypes.cpp
                     LINK_LIBRARIES ElementsKernel EL_FitsFile)

#===============================================================================
# Declare the Boost tests here
# Example:
# elements_add_unit_test(BoostClassExample tests/src/Boost/ClassExample_test.cpp
#                       EXECUTABLE BoostClassExample_test
#                       INCLUDE_DIRS ElementsExamples
#                       LINK_LIBRARIES ElementsExamples TYPE Boost)
#===============================================================================

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

CMAKE_MINIMUM_REQUIRED(VERSION 2.8.5)

#===============================================================================
# Load elements_subdir macro here
# Examples:
#   For declaring a project module:
#         elements_subdir(ElementsExamples)
#===============================================================================
elements_subdir(EL_FitsIO_Validation)

#===============================================================================
# Load elements_depends_on_subdirs macro here
#   For creating a dependency onto an other accessible module
#         elements_depends_on_subdirs(ElementsKernel)
#===============================================================================
elements_depends_on_subdirs(ElementsKernel)
elements_depends_on_subdirs(EL_FitsFile)
elements_depends_on_subdirs(EL_FitsIO_Examples)
elements_depends_on_subdirs(EL_CfitsioWrapper)

#===============================================================================
# Add the find_package macro (a pure CMake command) here to locate the
# libraries.
# Examples:
#          find_package(CppUnit)
#===============================================================================
find_package(Cfitsio REQUIRED)

#===============================================================================
# Declare the library dependencies here
# Example:
#         elements_add_library(ElementsExamples src/Lib/*.cpp
#                     LINK_LIBRARIES Boost ElementsKernel
#                     INCLUDE_DIRS Boost ElementsKernel
#                     PUBLIC_HEADERS ElementsExamples)
#===============================================================================
elements_add_library(EL_FitsIO_Validation src/lib/*.cpp
                     INCLUDE_DIRS ElementsKernel
                     LINK_LIBRARIES ElementsKernel
                     PUBLIC_HEADERS EL_FitsIO_Validation)

#===============================================================================
# Declare the executables here
# Example:
# elements_add_executable(ElementsProgramExample src/Program/ProgramExample.cpp
#                        LINK_LIBRARIES Boost ElementsExamples
#                        INCLUDE_DIRS Boost ElementsExamples)
#===============================================================================
elements_add_executable(EL_Cfitsio_WritePerf src/program/EL_Cfitsio_WritePerf.cpp
                     LINK_LIBRARIES ElementsKernel Cfitsio EL_CfitsioWrapper)
elements_add_executable(EL_FitsIO_WritePerf src/program/EL_FitsIO_WritePerf.cpp
                     LINK_LIBRARIES ElementsKernel EL_FitsFile)
elements_add_executable(EL_FitsIO_BufferedReadPerf src/program/EL_FitsIO_BufferedReadPerf.cpp
                     LINK_LIBRARIES ElementsKernel EL_FitsFile)

#===============================================================================
# Declare the Boost tests here
# Example:
# elements_add_unit_test(BoostClassExample tests/src/Boost/ClassExample_test.cpp
#                       EXECUTABLE BoostClassExample_test
#                       INCLUDE_DIRS ElementsExamples
#                       LINK_LIBRARIES ElementsExamples TYPE Boost)
#===============================================================================
elements_add_unit_test(Chronometer tests/src/Chronometer_test.cpp 
                     EXECUTABLE EL_FitsIO_Validation_Chronometer_test
                     LINK_LIBRARIES EL_FitsIO_Validation
                     TYPE Boost)
elements_add_unit_test(Benchmark tests/src/Benchmark_test.cpp 
                     EXECUTABLE EL_FitsIO_Validation_Benchmark_test
                     LINK_LIBRARIES EL_FitsIO_Validation
                     TYPE Boost)
elements_add_unit_test(CfitsioBenchmark tests/src/CfitsioBenchmark_test.cpp 
                     EXECUTABLE EL_FitsIO_Validation_CfitsioBenchmark_test
                     LINK_LIBRARIES EL_FitsIO_Validation
                     TYPE Boost)
elements_add_unit_test(ElBenchmark tests/src/ElBenchmark_test.cpp 
                     EXECUTABLE EL_FitsIO_Validation_ElBenchmark_test
                     LINK_LIBRARIES EL_FitsIO_Validation
                     TYPE Boost)
elements_add_unit_test(CsvAppender tests/src/CsvAppender_test.cpp 
                     EXECUTABLE EL_FitsIO_Validation_CsvAppender_test
                     LINK_LIBRARIES EL_FitsIO_Validation
                     TYPE Boost)
elements_add_test(CheckPrograms COMMAND EL_FitsIO_CheckPrograms)

#===============================================================================
# Use the following macro for python modules, scripts and aux files:
#  elements_install_python_modules()
#  elements_install_scripts()
#===============================================================================
elements_install_scripts()

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

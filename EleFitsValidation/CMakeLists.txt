CMAKE_MINIMUM_REQUIRED(VERSION 2.8.5)

#===============================================================================
# Load elements_subdir macro here
# Examples:
#   For declaring a project module:
#         elements_subdir(ElementsExamples)
#===============================================================================
elements_subdir(EleFitsValidation)

#===============================================================================
# Load elements_depends_on_subdirs macro here
#   For creating a dependency onto an other accessible module
#         elements_depends_on_subdirs(ElementsKernel)
#===============================================================================
elements_depends_on_subdirs(ElementsKernel)
elements_depends_on_subdirs(EleFits)

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
elements_add_library(EleFitsValidation src/lib/*.cpp
                     INCLUDE_DIRS ElementsKernel EleFits
                     LINK_LIBRARIES ElementsKernel EleFits
                     PUBLIC_HEADERS EleFitsValidation)

#===============================================================================
# Declare the executables here
# Example:
# elements_add_executable(ElementsProgramExample src/Program/ProgramExample.cpp
#                        LINK_LIBRARIES Boost ElementsExamples
#                        INCLUDE_DIRS Boost ElementsExamples)
#===============================================================================
elements_add_executable(EleFitsRunBenchmark src/program/EleFitsRunBenchmark.cpp
                     LINK_LIBRARIES EleFitsValidation)
elements_add_executable(EleFitsRunLoopingBenchmark src/program/EleFitsRunLoopingBenchmark.cpp
                     INCLUDE_DIRS ElementsKernel EleFitsValidation
                     LINK_LIBRARIES ElementsKernel EleFitsValidation)

#===============================================================================
# Declare the Boost tests here
# Example:
# elements_add_unit_test(BoostClassExample tests/src/Boost/ClassExample_test.cpp
#                       EXECUTABLE BoostClassExample_test
#                       INCLUDE_DIRS ElementsExamples
#                       LINK_LIBRARIES ElementsExamples TYPE Boost)
#===============================================================================
elements_add_unit_test(Chronometer tests/src/Chronometer_test.cpp 
                     EXECUTABLE EleFitsValidation_Chronometer_test
                     LINK_LIBRARIES EleFitsValidation
                     TYPE Boost)
elements_add_unit_test(CsvAppender tests/src/CsvAppender_test.cpp 
                     EXECUTABLE EleFitsValidation_CsvAppender_test
                     LINK_LIBRARIES EleFitsValidation
                     TYPE Boost)
elements_add_unit_test(Benchmark tests/src/Benchmark_test.cpp 
                     EXECUTABLE EleFitsValidation_Benchmark_test
                     LINK_LIBRARIES EleFitsValidation
                     TYPE Boost)
elements_add_unit_test(CfitsioBenchmark tests/src/CfitsioBenchmark_test.cpp 
                     EXECUTABLE EleFitsValidation_CfitsioBenchmark_test
                     LINK_LIBRARIES EleFitsValidation
                     TYPE Boost)
elements_add_unit_test(ElBenchmark tests/src/EleFitsBenchmark_test.cpp 
                     EXECUTABLE EleFitsValidation_EleFitsBenchmark_test
                     LINK_LIBRARIES EleFitsValidation
                     TYPE Boost)
elements_add_unit_test(LoopingBenchmark tests/src/LoopingBenchmark_test.cpp 
                     EXECUTABLE EleFitsValidation_LoopingBenchmark_test
                     LINK_LIBRARIES EleFitsValidation
                     TYPE Boost)
elements_add_test(CheckPrograms COMMAND EleFitsCheckPrograms)

#===============================================================================
# Use the following macro for python modules, scripts and aux files:
#  elements_install_python_modules()
#  elements_install_scripts()
#===============================================================================
elements_install_python_modules()
elements_install_scripts()

#===============================================================================
# Declare the Python programs here
# Examples :
# elements_add_python_program(PythonProgramExample
#                             ElementsExamples.PythonProgramExample)
#===============================================================================
elements_add_python_program(EleFitsRunBatchBenchmark EleFitsValidation.EleFitsRunBatchBenchmark)

#===============================================================================
# Add the elements_install_conf_files macro
# Examples:
#          elements_install_conf_files()
#===============================================================================
elements_install_conf_files()

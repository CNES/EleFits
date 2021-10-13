CMAKE_MINIMUM_REQUIRED(VERSION 2.8.5)

#===============================================================================
# Load elements_subdir macro here
# Examples:
#   For declaring a project module:
#         elements_subdir(ElementsExamples)
#===============================================================================
elements_subdir(EL_FitsData)

#===============================================================================
# Load elements_depends_on_subdirs macro here
#   For creating a dependency onto an other accessible module
#         elements_depends_on_subdirs(ElementsKernel)
#===============================================================================
elements_depends_on_subdirs(ElementsKernel)

#===============================================================================
# Add the find_package macro (a pure CMake command) here to locate the
# libraries.
# Examples:
#          find_package(CppUnit)
#===============================================================================
find_package(Boost)

#===============================================================================
# Declare the library dependencies here
# Example:
#         elements_add_library(ElementsExamples src/Lib/*.cpp
#                     LINK_LIBRARIES Boost ElementsKernel
#                     INCLUDE_DIRS Boost ElementsKernel
#                     PUBLIC_HEADERS ElementsExamples)
#===============================================================================
elements_add_library(EL_FitsData src/lib/*.cpp
                     INCLUDE_DIRS Boost ElementsKernel
                     LINK_LIBRARIES Boost ElementsKernel
                     PUBLIC_HEADERS EL_FitsData)

#===============================================================================
# Declare the executables here
# Example:
# elements_add_executable(ElementsProgramExample src/Program/ProgramExample.cpp
#                        LINK_LIBRARIES Boost ElementsExamples
#                        INCLUDE_DIRS Boost ElementsExamples)
#===============================================================================

#===============================================================================
# Declare the Boost tests here
# Example:
# elements_add_unit_test(BoostClassExample tests/src/Boost/ClassExample_test.cpp
#                       EXECUTABLE BoostClassExample_test
#                       INCLUDE_DIRS ElementsExamples
#                       LINK_LIBRARIES ElementsExamples TYPE Boost)
#===============================================================================
elements_add_unit_test(Column tests/src/Column_test.cpp 
                     EXECUTABLE EL_FitsData_Column_test
                     LINK_LIBRARIES EL_FitsData
                     TYPE Boost)
elements_add_unit_test(Raster tests/src/Raster_test.cpp 
                     EXECUTABLE EL_FitsData_Raster_test
                     LINK_LIBRARIES EL_FitsData
                     TYPE Boost)
elements_add_unit_test(Record tests/src/Record_test.cpp 
                     EXECUTABLE EL_FitsData_Record_test
                     LINK_LIBRARIES EL_FitsData
                     TYPE Boost)
elements_add_unit_test(RecordVec tests/src/RecordVec_test.cpp 
                     EXECUTABLE EL_FitsData_RecordVec_test
                     LINK_LIBRARIES EL_FitsData
                     TYPE Boost)
elements_add_unit_test(TestColumn tests/src/TestColumn_test.cpp 
                     EXECUTABLE EL_FitsData_TestColumn_test
                     LINK_LIBRARIES EL_FitsData
                     TYPE Boost)
elements_add_unit_test(TestRaster tests/src/TestRaster_test.cpp 
                     EXECUTABLE EL_FitsData_TestRaster_test
                     LINK_LIBRARIES EL_FitsData
                     TYPE Boost)
elements_add_unit_test(TestRecord tests/src/TestRecord_test.cpp 
                     EXECUTABLE EL_FitsData_TestRecord_test
                     LINK_LIBRARIES EL_FitsData
                     TYPE Boost)
elements_add_unit_test(FitsError tests/src/FitsError_test.cpp 
                     EXECUTABLE EL_FitsData_FitsError_test
                     LINK_LIBRARIES EL_FitsData
                     TYPE Boost)
elements_add_unit_test(DataUtils tests/src/DataUtils_test.cpp 
                     EXECUTABLE EL_FitsData_DataUtils_test
                     LINK_LIBRARIES EL_FitsData
                     TYPE Boost)
elements_add_unit_test(KeywordCategory tests/src/KeywordCategory_test.cpp 
                     EXECUTABLE EL_FitsData_KeywordCategory_test
                     LINK_LIBRARIES EL_FitsData
                     TYPE Boost)
elements_add_unit_test(HduCategory tests/src/HduCategory_test.cpp 
                     EXECUTABLE EL_FitsData_HduCategory_test
                     LINK_LIBRARIES EL_FitsData
                     TYPE Boost)
elements_add_unit_test(Subraster tests/src/Subraster_test.cpp 
                     EXECUTABLE EL_FitsData_Subraster_test
                     LINK_LIBRARIES EL_FitsData
                     TYPE Boost)
elements_add_unit_test(Position tests/src/Position_test.cpp 
                     EXECUTABLE EL_FitsData_Position_test
                     LINK_LIBRARIES EL_FitsData
                     TYPE Boost)
elements_add_unit_test(Region tests/src/Region_test.cpp 
                     EXECUTABLE EL_FitsData_Region_test
                     LINK_LIBRARIES EL_FitsData
                     TYPE Boost)
elements_add_unit_test(PositionIterator tests/src/PositionIterator_test.cpp 
                     EXECUTABLE EL_FitsData_PositionIterator_test
                     LINK_LIBRARIES EL_FitsData
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

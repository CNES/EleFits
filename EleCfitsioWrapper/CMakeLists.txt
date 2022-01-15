CMAKE_MINIMUM_REQUIRED(VERSION 2.8.5)

#===============================================================================
# Load elements_subdir macro here
# Examples:
#   For declaring a project module:
#         elements_subdir(ElementsExamples)
#===============================================================================
elements_subdir(EleCfitsioWrapper)

#===============================================================================
# Load elements_depends_on_subdirs macro here
#   For creating a dependency onto an other accessible module
#         elements_depends_on_subdirs(ElementsKernel)
#===============================================================================
elements_depends_on_subdirs(ElementsKernel)
elements_depends_on_subdirs(EleFitsData)

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
elements_add_library(EleCfitsioWrapper src/lib/*.cpp
                     INCLUDE_DIRS ElementsKernel EleFitsData Cfitsio
                     LINK_LIBRARIES ElementsKernel EleFitsData Cfitsio
                     PUBLIC_HEADERS EleCfitsioWrapper)

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
elements_add_unit_test(BintableWrapper tests/src/BintableWrapper_test.cpp 
                     EXECUTABLE EleCfitsioWrapper_BintableWrapper_test
                     LINK_LIBRARIES EleCfitsioWrapper
                     TYPE Boost)
elements_add_unit_test(CfitsioFixture tests/src/CfitsioFixture_test.cpp 
                     EXECUTABLE EleCfitsioWrapper_CfitsioFixture_test
                     LINK_LIBRARIES EleCfitsioWrapper
                     TYPE Boost)
elements_add_unit_test(CfitsioUtils tests/src/CfitsioUtils_test.cpp 
                     EXECUTABLE EleCfitsioWrapper_CfitsioUtils_test
                     LINK_LIBRARIES EleCfitsioWrapper
                     TYPE Boost)
elements_add_unit_test(CfitsioWrapper tests/src/CfitsioWrapper_test.cpp 
                     EXECUTABLE EleCfitsioWrapper_CfitsioWrapper_test
                     LINK_LIBRARIES EleCfitsioWrapper
                     TYPE Boost)
elements_add_unit_test(ErrorWrapper tests/src/ErrorWrapper_test.cpp 
                     EXECUTABLE EleCfitsioWrapper_ErrorWrapper_test
                     LINK_LIBRARIES EleCfitsioWrapper
                     TYPE Boost)
elements_add_unit_test(FileWrapper tests/src/FileWrapper_test.cpp 
                     EXECUTABLE EleCfitsioWrapper_FileWrapper_test
                     LINK_LIBRARIES EleCfitsioWrapper
                     TYPE Boost)
elements_add_unit_test(HduWrapper tests/src/HduWrapper_test.cpp 
                     EXECUTABLE EleCfitsioWrapper_HduWrapper_test
                     LINK_LIBRARIES EleCfitsioWrapper
                     TYPE Boost)
elements_add_unit_test(HeaderWrapper tests/src/HeaderWrapper_test.cpp 
                     EXECUTABLE EleCfitsioWrapper_HeaderWrapper_test
                     LINK_LIBRARIES EleCfitsioWrapper
                     TYPE Boost)
elements_add_unit_test(ImageWrapper tests/src/ImageWrapper_test.cpp 
                     EXECUTABLE EleCfitsioWrapper_ImageWrapper_test
                     LINK_LIBRARIES EleCfitsioWrapper
                     TYPE Boost)
elements_add_unit_test(TypeWrapper tests/src/TypeWrapper_test.cpp 
                     EXECUTABLE EleCfitsioWrapper_TypeWrapper_test
                     LINK_LIBRARIES EleCfitsioWrapper
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

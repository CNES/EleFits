CMAKE_MINIMUM_REQUIRED(VERSION 2.8.5)

#===============================================================================
# Load elements_subdir macro here
# Examples:
#   For declaring a project module:
#         elements_subdir(ElementsExamples)
#===============================================================================
elements_subdir(EleFits)

#===============================================================================
# Load elements_depends_on_subdirs macro here
#   For creating a dependency onto an other accessible module
#         elements_depends_on_subdirs(ElementsKernel)
#===============================================================================
elements_depends_on_subdirs(ElementsKernel)
elements_depends_on_subdirs(EleCfitsioWrapper)
elements_depends_on_subdirs(EleFitsUtils)

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
elements_add_library(EleFits src/lib/*.cpp
                     INCLUDE_DIRS ElementsKernel
                     LINK_LIBRARIES ElementsKernel EleCfitsioWrapper EleFitsUtils
                     PUBLIC_HEADERS EleFits)

#===============================================================================
# Declare the executables here
# Example:
# elements_add_executable(ElementsProgramExample src/Program/ProgramExample.cpp
#                        LINK_LIBRARIES Boost ElementsExamples
#                        INCLUDE_DIRS Boost ElementsExamples)
#===============================================================================
elements_add_executable(EleFitsPrintSupportedTypes src/program/EleFitsPrintSupportedTypes.cpp
                     LINK_LIBRARIES ElementsKernel EleFits)
elements_add_executable(EleFitsReadStructure src/program/EleFitsReadStructure.cpp
                     LINK_LIBRARIES ElementsKernel EleFits)

#===============================================================================
# Declare the Boost tests here
# Example:
# elements_add_unit_test(BoostClassExample tests/src/Boost/ClassExample_test.cpp
#                       EXECUTABLE BoostClassExample_test
#                       INCLUDE_DIRS ElementsExamples
#                       LINK_LIBRARIES ElementsExamples TYPE Boost)
#===============================================================================
elements_add_unit_test(BintableColumns tests/src/BintableColumns_test.cpp 
                     EXECUTABLE EleFits_BintableColumns_test
                     LINK_LIBRARIES EleFits
                     TYPE Boost)
elements_add_unit_test(BintableHdu tests/src/BintableHdu_test.cpp 
                     EXECUTABLE EleFits_BintableHdu_test
                     LINK_LIBRARIES EleFits
                     TYPE Boost)
elements_add_unit_test(ColumnKey tests/src/ColumnKey_test.cpp 
                     EXECUTABLE EleFits_ColumnKey_test
                     LINK_LIBRARIES EleFits
                     TYPE Boost)
elements_add_unit_test(FileMemRegions tests/src/FileMemRegions_test.cpp 
                     EXECUTABLE EleFits_FileMemRegions_test
                     LINK_LIBRARIES EleFits
                     TYPE Boost)
elements_add_unit_test(FileMemSegments tests/src/FileMemSegments_test.cpp 
                     EXECUTABLE EleFits_FileMemSegments_test
                     LINK_LIBRARIES EleFits
                     TYPE Boost)
elements_add_unit_test(FitsFile tests/src/FitsFile_test.cpp 
                     EXECUTABLE EleFits_FitsFile_test
                     LINK_LIBRARIES EleFits
                     TYPE Boost)
elements_add_unit_test(Hdu tests/src/Hdu_test.cpp 
                     EXECUTABLE EleFits_Hdu_test
                     LINK_LIBRARIES EleFits
                     TYPE Boost)
elements_add_unit_test(HduIterator tests/src/HduIterator_test.cpp 
                     EXECUTABLE EleFits_HduIterator_test
                     LINK_LIBRARIES EleFits
                     TYPE Boost)
elements_add_unit_test(Header tests/src/Header_test.cpp 
                     EXECUTABLE EleFits_Header_test
                     LINK_LIBRARIES EleFits
                     TYPE Boost)
elements_add_unit_test(ImageHdu tests/src/ImageHdu_test.cpp 
                     EXECUTABLE EleFits_ImageHdu_test
                     LINK_LIBRARIES EleFits
                     TYPE Boost)
elements_add_unit_test(ImageRaster tests/src/ImageRaster_test.cpp 
                     EXECUTABLE EleFits_ImageRaster_test
                     LINK_LIBRARIES EleFits
                     TYPE Boost)
elements_add_unit_test(MefFile tests/src/MefFile_test.cpp 
                     EXECUTABLE EleFits_MefFile_test
                     LINK_LIBRARIES EleFits
                     TYPE Boost)
elements_add_unit_test(SifFile tests/src/SifFile_test.cpp 
                     EXECUTABLE EleFits_SifFile_test
                     LINK_LIBRARIES EleFits
                     TYPE Boost)
elements_add_unit_test(TestBintable tests/src/TestBintable_test.cpp 
                     EXECUTABLE EleFits_TestBintable_test
                     LINK_LIBRARIES EleFits
                     TYPE Boost)

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
elements_install_aux_files()

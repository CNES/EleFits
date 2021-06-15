CMAKE_MINIMUM_REQUIRED(VERSION 2.8.5)

#===============================================================================
# Load elements_subdir macro here
# Examples:
#   For declaring a project module:
#         elements_subdir(ElementsExamples)
#===============================================================================
elements_subdir(EL_FitsFile)

#===============================================================================
# Load elements_depends_on_subdirs macro here
#   For creating a dependency onto an other accessible module
#         elements_depends_on_subdirs(ElementsKernel)
#===============================================================================
elements_depends_on_subdirs(ElementsKernel)
elements_depends_on_subdirs(EL_CfitsioWrapper)
elements_depends_on_subdirs(EL_FitsUtils)

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
elements_add_library(EL_FitsFile src/lib/*.cpp
                     INCLUDE_DIRS ElementsKernel
                     LINK_LIBRARIES ElementsKernel EL_CfitsioWrapper EL_FitsUtils
                     PUBLIC_HEADERS EL_FitsFile)

#===============================================================================
# Declare the executables here
# Example:
# elements_add_executable(ElementsProgramExample src/Program/ProgramExample.cpp
#                        LINK_LIBRARIES Boost ElementsExamples
#                        INCLUDE_DIRS Boost ElementsExamples)
#===============================================================================
elements_add_executable(EL_FitsIO_PrintSupportedTypes src/program/EL_FitsIO_PrintSupportedTypes.cpp
                     LINK_LIBRARIES ElementsKernel EL_FitsFile)
elements_add_executable(EL_FitsIO_ReadStructure src/program/EL_FitsIO_ReadStructure.cpp
                     LINK_LIBRARIES ElementsKernel EL_FitsFile)

#===============================================================================
# Declare the Boost tests here
# Example:
# elements_add_unit_test(BoostClassExample tests/src/Boost/ClassExample_test.cpp
#                       EXECUTABLE BoostClassExample_test
#                       INCLUDE_DIRS ElementsExamples
#                       LINK_LIBRARIES ElementsExamples TYPE Boost)
#===============================================================================
elements_add_unit_test(FitsFile tests/src/FitsFile_test.cpp 
                     EXECUTABLE EL_FitsFile_FitsFile_test
                     LINK_LIBRARIES EL_FitsFile
                     TYPE Boost)
elements_add_unit_test(MefFile tests/src/MefFile_test.cpp 
                     EXECUTABLE EL_FitsFile_MefFile_test
                     LINK_LIBRARIES EL_FitsFile
                     TYPE Boost)
elements_add_unit_test(SifFile tests/src/SifFile_test.cpp 
                     EXECUTABLE EL_FitsFile_SifFile_test
                     LINK_LIBRARIES EL_FitsFile
                     TYPE Boost)
elements_add_unit_test(RecordHdu tests/src/RecordHdu_test.cpp 
                     EXECUTABLE EL_FitsFile_RecordHdu_test
                     LINK_LIBRARIES EL_FitsFile
                     TYPE Boost)
elements_add_unit_test(ImageHdu tests/src/ImageHdu_test.cpp 
                     EXECUTABLE EL_FitsFile_ImageHdu_test
                     LINK_LIBRARIES EL_FitsFile
                     TYPE Boost)
elements_add_unit_test(BintableHdu tests/src/BintableHdu_test.cpp 
                     EXECUTABLE EL_FitsFile_BintableHdu_test
                     LINK_LIBRARIES EL_FitsFile
                     TYPE Boost)
elements_add_unit_test(HduIterator tests/src/HduIterator_test.cpp 
                     EXECUTABLE EL_FitsFile_HduIterator_test
                     LINK_LIBRARIES EL_FitsFile
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

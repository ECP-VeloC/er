CMAKE_MINIMUM_REQUIRED(VERSION 3.14)
PROJECT(ER VERSION 0.5.0)

# Mac rpath Policy
IF(POLICY CMP0042)
          CMAKE_POLICY(SET CMP0042 NEW)
ENDIF(POLICY CMP0042)
SET(CMAKE_MACOSX_RPATH ON)

# Linux rpath settings
SET(CMAKE_SKIP_BUILD_RPATH FALSE)
SET(CMAKE_BUILD_WITH_INSTALL_RPATH FALSE)
SET(CMAKE_INSTALL_RPATH_USE_LINK_PATH TRUE)

SET(CMAKE_POSITION_INDEPENDENT_CODE ON)

#SET(CMAKE_C_FLAGS "-Wall")

# Configuration Options
OPTION(BUILD_SHARED_LIBS "Whether to build shared libraries" ON)
MESSAGE(STATUS "BUILD_SHARED_LIBS: ${BUILD_SHARED_LIBS}")

if (BUILD_SHARED_LIBS)
   OPTION(ER_LINK_STATIC "Default to static linking? (Needed for Cray)" OFF)
else()
   OPTION(ER_LINK_STATIC "Default to static linking? (Needed for Cray)" ON)
endif()
MESSAGE(STATUS "ER_LINK_STATIC: ${ER_LINK_STATIC}")

OPTION(ENABLE_TESTS "Whether to build tests" ON)
MESSAGE(STATUS "ENABLE_TESTS: ${ENABLE_TESTS}")

# Find Packages & Files

LIST(APPEND CMAKE_MODULE_PATH "${PROJECT_SOURCE_DIR}/cmake")

## REDSET
FIND_PACKAGE(redset REQUIRED)
LIST(APPEND ER_EXTERNAL_LIBS redset::redset)
LIST(APPEND ER_EXTERNAL_STATIC_LIBS redset::redset-static)

## SHUFFILE
FIND_PACKAGE(shuffile REQUIRED)
LIST(APPEND ER_EXTERNAL_LIBS shuffile::shuffile)
LIST(APPEND ER_EXTERNAL_STATIC_LIBS shuffile::shuffile-static)

## KVTREE
FIND_PACKAGE(kvtree REQUIRED)
LIST(APPEND ER_EXTERNAL_LIBS kvtree::kvtree)
LIST(APPEND ER_EXTERNAL_STATIC_LIBS kvtree::kvtree-static)

## RANKSTR
## only needed for the test
FIND_PACKAGE(rankstr REQUIRED)
LIST(APPEND ER_EXTERNAL_LIBS rankstr::rankstr)
LIST(APPEND ER_EXTERNAL_STATIC_LIBS rankstr::rankstr-static)

## MPI
INCLUDE(SetupMPI)
IF(MPI_C_FOUND)
    LIST(APPEND ER_EXTERNAL_LIBS MPI::MPI_C)
    LIST(APPEND ER_EXTERNAL_STATIC_LIBS MPI::MPI_C)
ELSE(MPI_C_FOUND)
    MESSAGE(FATAL_ERROR
        "Could not find MPI! "
        "Either add an MPI compiler to your path "
        "or force CMake to build using the correct compiler (`export CC=mpicc`).")
ENDIF(MPI_C_FOUND)

## HEADERS
INCLUDE(CheckIncludeFile)
INCLUDE(GNUInstallDirs)

# Subdirectories
ADD_SUBDIRECTORY(src)

IF(ENABLE_TESTS)
   INCLUDE(CTest)
   INCLUDE(ER_ADD_TEST)
   ADD_SUBDIRECTORY(test)
ENDIF(ENABLE_TESTS)

# Generate config.h with all our build #defs
CONFIGURE_FILE(${PROJECT_SOURCE_DIR}/cmake/config.h.in ${PROJECT_BINARY_DIR}/config.h)
SET_SOURCE_FILES_PROPERTIES(${PROJECT_BINARY_DIR}/config.h PROPERTIES GENERATED TRUE)

# Special Install Files
# INSTALL(FILES README.md DESTINATION ${CMAKE_INSTALL_DATADIR}/kvtree)

# CMake configs

# Install the exports file
INSTALL(EXPORT erTargets FILE erTargets.cmake NAMESPACE er:: DESTINATION share/er/cmake)

# Export so we can use in-tree
EXPORT(EXPORT erTargets NAMESPACE er:: FILE erTargets.cmake)

# Setup the package config
INCLUDE(CMakePackageConfigHelpers)
CONFIGURE_PACKAGE_CONFIG_FILE(cmake/erConfig.cmake.in ${CMAKE_CURRENT_BINARY_DIR}/erConfig.cmake INSTALL_DESTINATION share/er/cmake)
WRITE_BASIC_PACKAGE_VERSION_FILE(${CMAKE_CURRENT_BINARY_DIR}/erConfigVersion.cmake COMPATIBILITY SameMajorVersion)

# Install package config
INSTALL(FILES ${CMAKE_CURRENT_BINARY_DIR}/erConfig.cmake ${CMAKE_CURRENT_BINARY_DIR}/erConfigVersion.cmake DESTINATION share/er/cmake)

# Package
SET(CPACK_PACKAGE_DESCRIPTION_SUMMARY "Encode/Rebuild library for a set of files")
SET(CPACK_PACKAGE_VENDOR "Lawrence Livermore National Laboratory")
# SET(CPACK_PACKAGE_DESCRIPTION_FILE "${PROJECT_SOURCE_DIR}/README.md")
# SET(CPACK_RESOURCE_FILE_LICENSE "${PROJECT_SOURCE_DIR}/LICENSE.TXT")

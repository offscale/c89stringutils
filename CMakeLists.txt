cmake_minimum_required(VERSION 3.11)
project(c89stringutils VERSION 0.0.2 LANGUAGES C)

set(CMAKE_C_VISIBILITY_PRESET hidden)
set(CMAKE_VISIBILITY_INLINES_HIDDEN YES)
set(CMAKE_C_STANDARD 90)

add_library("${PROJECT_NAME}_compiler_flags" INTERFACE)
if (NOT DEFINED MSVC_VERSION
        OR MSVC_VERSION STRGREATER "1900" # 2015
        OR NOT (CMAKE_C_COMPILER_ID STREQUAL "OpenWatcom"))
    target_compile_features("${PROJECT_NAME}_compiler_flags" INTERFACE "c_std_${CMAKE_C_STANDARD}")
elseif ((MSVC_VERSION STRLESS_EQUAL "1900"
         OR CMAKE_C_COMPILER_ID STREQUAL "OpenWatcom"
         OR CYGWIN)
        AND NOT EXISTS "${CMAKE_BINARY_DIR}/c89stringutils_export.h")
    file(COPY "${CMAKE_SOURCE_DIR}/c89stringutils/c89stringutils_export_pregen.h"
            DESTINATION "${CMAKE_BINARY_DIR}")
    file(RENAME "${CMAKE_BINARY_DIR}/c89stringutils_export_pregen.h"
            "${CMAKE_BINARY_DIR}/c89stringutils_export.h")
endif ()
if (CMAKE_VERSION VERSION_GREATER_EQUAL "3.15")
    set(gcc_like "$<COMPILE_LANG_AND_ID:C,CXX,ARMClang,AppleClang,Clang,GNU,LCC>")
    set(msvc "$<COMPILE_LANG_AND_ID:C,CXX,MSVC>")
    target_compile_options(
            "${PROJECT_NAME}_compiler_flags"
            INTERFACE
            "$<$<AND:${gcc_like},$<CONFIG:Debug>>:$<BUILD_INTERFACE:-Wshadow;-Wformat=2;-Wall;-Wno-missing-braces;-Wno-long-long;-pedantic;-fprofile-arcs;-ftest-coverage>>"
            "$<${msvc}:$<BUILD_INTERFACE:-W3;-WX;-Zi;-permissive->>"
    )
    target_link_options(
            "${PROJECT_NAME}_compiler_flags"
            INTERFACE
            "$<$<AND:${gcc_like},$<CONFIG:Debug>>:$<BUILD_INTERFACE:--coverage>>"
    )
endif (CMAKE_VERSION VERSION_GREATER_EQUAL "3.15")
# Set the build directories
if (CMAKE_SYSTEM_NAME STREQUAL "Windows"
        OR CMAKE_SYSTEM_NAME STREQUAL "CYGWIN"
        OR CMAKE_SYSTEM_NAME MATCHES "MINGW.*")
    set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY "${PROJECT_BINARY_DIR}")
    set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${PROJECT_BINARY_DIR}")
    set(CMAKE_LIBRARY_OUTPUT_DIRECTORY "${PROJECT_BINARY_DIR}")
else ()
    set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY "${PROJECT_BINARY_DIR}/lib")
    set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${PROJECT_BINARY_DIR}/bin")
    set(CMAKE_LIBRARY_OUTPUT_DIRECTORY "${PROJECT_BINARY_DIR}/lib")
endif ()

# configure a header file to pass the version number only
configure_file(
        "${CMAKE_CURRENT_SOURCE_DIR}/cmake/config.h.in"
        "${PROJECT_NAME}Config.h"
)

add_subdirectory("${PROJECT_NAME}")

include(CTest)
option(BUILD_TESTING "Build the tests" OFF)
option(C89STRINGUTILS_BUILD_TESTING "Build the tests" OFF)
if (BUILD_TESTING AND C89STRINGUTILS_BUILD_TESTING)
    add_subdirectory("${PROJECT_NAME}/tests")
endif (BUILD_TESTING AND C89STRINGUTILS_BUILD_TESTING)

###########
# Install #
###########

include(GNUInstallDirs)

install(
        FILES "${PROJECT_BINARY_DIR}/${PROJECT_NAME}Config.h"
        DESTINATION "${CMAKE_INSTALL_INCLUDEDIR}"
)
include(InstallRequiredSystemLibraries)
set(CPACK_BUNDLE_NAME "${PROJECT_NAME}")
set(CPACK_PACKAGE_VENDOR "Samuel Marks")
set(CPACK_PACKAGE_DESCRIPTION "string functions from newer standards / common non-standards for C89")
set(CPACK_PACKAGE_DESCRIPTION_SUMMARY "${CPACK_PACKAGE_DESCRIPTION}")
if (APPLE)
    set(CPACK_BUNDLE_PLIST "${CMAKE_CURRENT_SOURCE_DIR}/cmake/Info.plist")
    set(CPACK_BUNDLE_ICON "${CMAKE_CURRENT_SOURCE_DIR}/cmake/Info.plist")
    set(CPACK_PACKAGE_ICON "${CMAKE_CURRENT_SOURCE_DIR}/cmake/CustomVolumeIcon.icns")
endif (APPLE)
set(CPACK_RESOURCE_FILE_LICENSE "${CMAKE_CURRENT_SOURCE_DIR}/cmake/LICENSE.txt")
set(CPACK_PACKAGE_VERSION_MAJOR "${${PROJECT_NAME}_VERSION_MAJOR}")
set(CPACK_PACKAGE_VERSION_MINOR "${${PROJECT_NAME}_VERSION_MINOR}")
set(CPACK_RESOURCE_FILE_README "${CMAKE_CURRENT_SOURCE_DIR}/cmake/README.txt")
set(CPACK_RESOURCE_FILE_WELCOME "${CMAKE_CURRENT_SOURCE_DIR}/cmake/Welcome.txt")
set(CPACK_PACKAGE_CONTACT "https://github.com/offscale/${PROJECT_NAME}")

include(CPack)
include(CMakePackageConfigHelpers)

# generate the config file that is includes the exports
configure_package_config_file(
        "${CMAKE_CURRENT_SOURCE_DIR}/cmake/Config.cmake.in"
        "${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_NAME}Config.cmake"
        INSTALL_DESTINATION "${CMAKE_INSTALL_DATADIR}/${PROJECT_NAME}"
        NO_SET_AND_CHECK_MACRO
        NO_CHECK_REQUIRED_COMPONENTS_MACRO
)

# generate the version file for the config file
write_basic_package_version_file(
        "${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_NAME}ConfigVersion.cmake"
        VERSION "${${PROJECT_NAME}_VERSION_MAJOR}.${${PROJECT_NAME}_VERSION_MINOR}"
        COMPATIBILITY AnyNewerVersion
)
install(FILES "${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_NAME}Config.cmake"
        DESTINATION "${CMAKE_INSTALL_DATADIR}/${PROJECT_NAME}")

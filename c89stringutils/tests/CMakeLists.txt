get_filename_component(EXEC_NAME "${CMAKE_CURRENT_SOURCE_DIR}" NAME)
set(EXEC_NAME "${PROJECT_NAME}_${EXEC_NAME}")

#########################
# Dependencies download #
#########################

set(DOWNLOAD_DIR "${PROJECT_BINARY_DIR}/test_downloads")
file(MAKE_DIRECTORY "${DOWNLOAD_DIR}")

if (NOT CMAKE_SYSTEM_NAME STREQUAL "SunOS")
    file(DOWNLOAD "https://raw.githubusercontent.com/SamuelMarks/greatest/master/greatest.h"
            "${DOWNLOAD_DIR}/greatest.h")
endif (NOT CMAKE_SYSTEM_NAME STREQUAL "SunOS")

#########
# Tests #
#########

set(Header_Files "test_string_extras.h")
source_group("Header Files" FILES "${Header_Files}")

set(Source_Files "test.c")
source_group("Source Files" FILES "${Source_Files}")

add_executable("${EXEC_NAME}" "${Header_Files}" "${Source_Files}")

target_link_libraries("${EXEC_NAME}" PRIVATE "${PROJECT_NAME}")
if (NOT CMAKE_C_COMPILER_ID STREQUAL "OpenWatcom")
    target_link_libraries("${EXEC_NAME}" PRIVATE "${PROJECT_NAME}_compiler_flags")
endif (NOT CMAKE_C_COMPILER_ID STREQUAL "OpenWatcom")
target_include_directories(
        "${EXEC_NAME}"
        PRIVATE
        "$<BUILD_INTERFACE:${PROJECT_BINARY_DIR}/src>"
        "$<BUILD_INTERFACE:${DOWNLOAD_DIR}>"
)

add_test(NAME "${EXEC_NAME}" COMMAND "${EXEC_NAME}")

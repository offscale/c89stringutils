get_filename_component(EXEC_NAME "${CMAKE_CURRENT_SOURCE_DIR}" NAME)
string(REPLACE " " "_" EXEC_NAME "${EXEC_NAME}")

#########################
# Dependencies download #
#########################

set(DOWNLOAD_DIR "${PROJECT_BINARY_DIR}/test_downloads")
file(MAKE_DIRECTORY "${DOWNLOAD_DIR}")

set(GREATEST_SHA256 "1292593d95c35eeccc89ffa1c91d6fe53b49f81cbf2c2b7758842b7f3186fcc2")
if (NOT CMAKE_SYSTEM_NAME STREQUAL "SunOS")
    file(DOWNLOAD "https://raw.githubusercontent.com/silentbicycle/greatest/fbbf981/greatest.h"
            "${DOWNLOAD_DIR}/greatest.h")
    file(SHA256 "${DOWNLOAD_DIR}/greatest.h" GREATEST_SHA256_FOUND)
    if (NOT GREATEST_SHA256_FOUND STREQUAL GREATEST_SHA256)
        message(FATAL_ERROR "Hashes don't match on \"${DOWNLOAD_DIR}/greatest.h\" download")
    endif (NOT GREATEST_SHA256_FOUND STREQUAL GREATEST_SHA256)
endif (NOT CMAKE_SYSTEM_NAME STREQUAL "SunOS")

#########
# Tests #
#########

set(Header_Files "test_string_extras.h")
source_group("Header Files" FILES "${Header_Files}")

set(Source_Files "test.c")
source_group("Source Files" FILES "${Source_Files}")

add_executable("${EXEC_NAME}" "${Header_Files}" "${Source_Files}")

target_link_libraries("${EXEC_NAME}" PRIVATE "c89stringutils_compiler_flags" "c89stringutils")

target_include_directories(
        "${EXEC_NAME}"
        PRIVATE
        "$<BUILD_INTERFACE:${PROJECT_BINARY_DIR}/src>"
        "$<BUILD_INTERFACE:${DOWNLOAD_DIR}>"
)

add_test(NAME "${EXEC_NAME}" COMMAND "${EXEC_NAME}")

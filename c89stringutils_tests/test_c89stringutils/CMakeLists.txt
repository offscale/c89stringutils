get_filename_component(EXEC_NAME "${CMAKE_CURRENT_SOURCE_DIR}" NAME)
string(REPLACE " " "_" EXEC_NAME "${EXEC_NAME}")

#########################
# Dependencies download #
#########################

set(DOWNLOAD_DIR "${PROJECT_BINARY_DIR}/test_downloads")
file(MAKE_DIRECTORY "${DOWNLOAD_DIR}")
file(DOWNLOAD "https://raw.githubusercontent.com/silentbicycle/greatest/11a6af1/greatest.h"
        "${DOWNLOAD_DIR}/greatest.h"
        EXPECTED_HASH "SHA256=1292593d95c35eeccc89ffa1c91d6fe53b49f81cbf2c2b7758842b7f3186fcc2")

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

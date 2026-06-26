get_filename_component(EXEC_NAME "${CMAKE_CURRENT_SOURCE_DIR}" NAME)
set(EXEC_NAME "${PROJECT_NAME}_${EXEC_NAME}")

#########################
# Dependencies download #
#########################

set(DOWNLOAD_DIR "${PROJECT_BINARY_DIR}/test_downloads")
file(MAKE_DIRECTORY "${DOWNLOAD_DIR}")

if (NOT CMAKE_SYSTEM_NAME STREQUAL "SunOS")
    include(FetchContent)
    FetchContent_Declare(
            greatest
            URL "https://raw.githubusercontent.com/SamuelMarks/greatest/master/greatest.h"
            DOWNLOAD_NO_EXTRACT TRUE
    )
    FetchContent_MakeAvailable(greatest)
    file(COPY "${greatest_SOURCE_DIR}/greatest.h" DESTINATION "${DOWNLOAD_DIR}")
endif (NOT CMAKE_SYSTEM_NAME STREQUAL "SunOS")

#########
# Tests #
#########

set(Header_Files "test_string_extras.h" "test_safecrt.h")
source_group("Header Files" FILES "${Header_Files}")

set(Source_Files "test.c" "${CMAKE_CURRENT_SOURCE_DIR}/../c89stringutils_string_extras.c" "${CMAKE_CURRENT_SOURCE_DIR}/../c89stringutils_safecrt.c")
source_group("Source Files" FILES "${Source_Files}")

add_executable("${EXEC_NAME}" "${Header_Files}" "${Source_Files}")

target_compile_definitions("${EXEC_NAME}" PRIVATE C89STRINGUTILS_TEST_MOCKS C89STRINGUTILS_STATIC_DEFINE DEBUG)

if (NOT CMAKE_C_COMPILER_ID STREQUAL "OpenWatcom")
    target_link_libraries("${EXEC_NAME}" PRIVATE "${PROJECT_NAME}_compiler_flags")
endif (NOT CMAKE_C_COMPILER_ID STREQUAL "OpenWatcom")
target_include_directories(
        "${EXEC_NAME}"
        PRIVATE
        "${CMAKE_CURRENT_SOURCE_DIR}/.."
        "${CMAKE_BINARY_DIR}"
        "$<BUILD_INTERFACE:${PROJECT_BINARY_DIR}/src>"
        "$<BUILD_INTERFACE:${DOWNLOAD_DIR}>"
)

add_test(NAME "${EXEC_NAME}" COMMAND "${EXEC_NAME}")

if (C89STRINGUTILS_BUILD_AMALGAMATION)
    add_executable("test_amalg" "${CMAKE_CURRENT_SOURCE_DIR}/test_amalg.c")
    target_include_directories("test_amalg" PRIVATE "${PROJECT_BINARY_DIR}")
    add_test(NAME "test_amalg" COMMAND "test_amalg")
endif()

get_filename_component(EXEC_NAME "${CMAKE_CURRENT_SOURCE_DIR}" NAME)
set(EXEC_NAME "${PROJECT_NAME}_${EXEC_NAME}")

#########################
# Dependencies download #
#########################

set(DOWNLOAD_DIR "${PROJECT_BINARY_DIR}/test_downloads")
file(MAKE_DIRECTORY "${DOWNLOAD_DIR}")

if (NOT CMAKE_SYSTEM_NAME STREQUAL "SunOS")
    include(FetchContent)
    if(EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/../../../greatest")
        message(STATUS "Fetching greatest from local directory")
        FetchContent_Declare(
                greatest
                SOURCE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/../../../greatest"
        )
        FetchContent_MakeAvailable(greatest)
        if (MSVC)
            file(READ "${greatest_SOURCE_DIR}/greatest.h" _greatest_content)
            string(REPLACE "while (0)" "while (0,0)" _greatest_content "${_greatest_content}")
            file(WRITE "${greatest_SOURCE_DIR}/greatest.h" "${_greatest_content}")
        endif()
        file(COPY "${greatest_SOURCE_DIR}/greatest.h" DESTINATION "${DOWNLOAD_DIR}")
    else()
        FetchContent_Declare(
                greatest
                URL "https://raw.githubusercontent.com/SamuelMarks/greatest/master/greatest.h"
                DOWNLOAD_NO_EXTRACT TRUE
        )
        FetchContent_MakeAvailable(greatest)
        if (MSVC)
            file(READ "${greatest_SOURCE_DIR}/greatest.h" _greatest_content)
            string(REPLACE "while (0)" "while (0,0)" _greatest_content "${_greatest_content}")
            file(WRITE "${greatest_SOURCE_DIR}/greatest.h" "${_greatest_content}")
        endif()
        file(COPY "${greatest_SOURCE_DIR}/greatest.h" DESTINATION "${DOWNLOAD_DIR}")
    endif()
endif (NOT CMAKE_SYSTEM_NAME STREQUAL "SunOS")

#########
# Tests #
#########

set(Header_Files "test_string_extras.h" "test_safecrt.h")
source_group("Header Files" FILES "${Header_Files}")

set(Source_Files "test.c" "${CMAKE_CURRENT_SOURCE_DIR}/../c89stringutils_string_extras.c" "${CMAKE_CURRENT_SOURCE_DIR}/../c89stringutils_safecrt.c")
source_group("Source Files" FILES "${Source_Files}")

add_executable("${EXEC_NAME}" "${Header_Files}" "${Source_Files}")

target_compile_definitions("${EXEC_NAME}" PRIVATE C89STRINGUTILS_TEST_MOCKS C89STRINGUTILS_STATIC_DEFINE DEBUG _CRT_SECURE_NO_WARNINGS)

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

if (${PROJECT_PREFIX}_BUILD_AMALGAMATION)
    add_executable("test_amalg" "${CMAKE_CURRENT_SOURCE_DIR}/test_amalg.c")
    target_include_directories("test_amalg" PRIVATE "${PROJECT_BINARY_DIR}")
    add_test(NAME "test_amalg" COMMAND "test_amalg")
endif()

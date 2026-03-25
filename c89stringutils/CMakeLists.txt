get_filename_component(LIBRARY_NAME "${CMAKE_CURRENT_SOURCE_DIR}" NAME)

set(Header_Files "c89stringutils_string_extras.h")
source_group("Header Files" FILES "${Header_Files}")

set(Source_Files "c89stringutils_string_extras.c")
source_group("Source Files" FILES "${Source_Files}")

add_library("${LIBRARY_NAME}" "${Header_Files}" "${Source_Files}")

include(GNUInstallDirs)
target_include_directories(
        "${LIBRARY_NAME}"
        PUBLIC
        "$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>"
        "$<BUILD_INTERFACE:${CMAKE_BINARY_DIR}>"
        "$<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>"
)

set_target_properties(
        "${LIBRARY_NAME}"
        PROPERTIES
        LINKER_LANGUAGE
        C
)

include(GenerateExportHeader)
set(_export_file "${CMAKE_BINARY_DIR}/${LIBRARY_NAME}_export.h")
generate_export_header("${LIBRARY_NAME}" EXPORT_FILE_NAME "${_export_file}")

#################
# Install rules #
#################

include(GNUInstallDirs)

# setup the version numbering
set_property(TARGET "${LIBRARY_NAME}" PROPERTY VERSION "${${PROJECT_NAME}_VERSION}")
set_property(TARGET "${LIBRARY_NAME}" PROPERTY SOVERSION "${${PROJECT_NAME}_VERSION_MAJOR}")

set(installable_libs # "${EXEC_NAME}"
        "${LIBRARY_NAME}")
if (TARGET "${DEPENDANT_LIBRARY}")
    list(APPEND installable_libs "${DEPENDANT_LIBRARY}")
endif ()
if (CMAKE_VERSION VERSION_GREATER_EQUAL "3.19")
    list(APPEND installable_libs "${PROJECT_NAME}_compiler_flags")
endif ()
install(TARGETS ${installable_libs}
        EXPORT "${LIBRARY_NAME}Targets"
        ARCHIVE DESTINATION "${CMAKE_INSTALL_LIBDIR}"
        LIBRARY DESTINATION "${CMAKE_INSTALL_LIBDIR}"
        RUNTIME DESTINATION "${CMAKE_INSTALL_BINDIR}")
install(EXPORT "${LIBRARY_NAME}Targets" DESTINATION "${CMAKE_INSTALL_DATADIR}/${LIBRARY_NAME}")

install(FILES ${Header_Files} "${_export_file}"
        DESTINATION "${CMAKE_INSTALL_INCLUDEDIR}")

option(C89STRINGUTILS_BUILD_AMALGAMATION "Generate STB-style single-file header amalgamation" OFF)
if (C89STRINGUTILS_BUILD_AMALGAMATION)
    set(amalg_file "${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_NAME}_amalgamation.h")
    set_property(DIRECTORY APPEND PROPERTY CMAKE_CONFIGURE_DEPENDS
        "c89stringutils_export_pregen.h"
        "c89stringutils_string_extras.h"
        "c89stringutils_string_extras.c")

    file(READ "${CMAKE_CURRENT_SOURCE_DIR}/c89stringutils_export_pregen.h" amalg_export)
    file(READ "${CMAKE_CURRENT_SOURCE_DIR}/c89stringutils_string_extras.h" amalg_header)
    file(READ "${CMAKE_CURRENT_SOURCE_DIR}/c89stringutils_string_extras.c" amalg_source)

    string(REPLACE "#include \"c89stringutils_export.h\"" "" amalg_header "${amalg_header}")
    string(REPLACE "#include \"c89stringutils_string_extras.h\"" "" amalg_source "${amalg_source}")

    set(amalgamation "/* c89stringutils Amalgamation STB-style */\n\n")
    string(APPEND amalgamation "#ifndef C89STRINGUTILS_AMALGAMATION_H\n#define C89STRINGUTILS_AMALGAMATION_H\n\n")
    string(APPEND amalgamation "${amalg_export}\n\n")
    string(APPEND amalgamation "${amalg_header}\n\n")
    string(APPEND amalgamation "#ifdef C89STRINGUTILS_IMPLEMENTATION\n\n")
    string(APPEND amalgamation "${amalg_source}\n\n")
    string(APPEND amalgamation "#endif /* C89STRINGUTILS_IMPLEMENTATION */\n\n")
    string(APPEND amalgamation "#endif /* C89STRINGUTILS_AMALGAMATION_H */\n")

    file(WRITE "${amalg_file}" "${amalgamation}")

    install(FILES "${amalg_file}"
            DESTINATION "${CMAKE_INSTALL_INCLUDEDIR}")
endif()


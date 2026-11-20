get_filename_component(LIBRARY_NAME "${CMAKE_CURRENT_SOURCE_DIR}" NAME)

set(Header_Files "c89stringutils_string_extras.h" "c89stringutils_log.h" "c89stringutils_safecrt.h")
source_group("Header Files" FILES "${Header_Files}")

set(Source_Files "c89stringutils_string_extras.c" "c89stringutils_safecrt.c")
source_group("Source Files" FILES "${Source_Files}")

add_library("${LIBRARY_NAME}" "${Header_Files}" "${Source_Files}")
if (NOT TARGET "${PROJECT_NAME}::${LIBRARY_NAME}")
    add_library("${PROJECT_NAME}::${LIBRARY_NAME}" ALIAS "${LIBRARY_NAME}")
endif ()

target_link_libraries("${LIBRARY_NAME}" PRIVATE "${PROJECT_NAME}_compiler_flags")

include(GNUInstallDirs)
target_include_directories(
        "${LIBRARY_NAME}"
        PUBLIC
        "$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}>"
        "$<BUILD_INTERFACE:${PROJECT_BINARY_DIR}>"
        "$<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>"
)

set_target_properties(
        "${LIBRARY_NAME}"
        PROPERTIES
        LINKER_LANGUAGE
        C
)

include(GenerateExportHeader)
set(_export_file "${PROJECT_BINARY_DIR}/${LIBRARY_NAME}_export.h")
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
if (DEFINED DEPENDANT_LIBRARY)
    if (TARGET "${DEPENDANT_LIBRARY}")
        list(APPEND installable_libs "${DEPENDANT_LIBRARY}")
    endif ()
endif ()
if (CMAKE_VERSION VERSION_GREATER_EQUAL "3.10")
    list(APPEND installable_libs "${PROJECT_NAME}_compiler_flags")
endif ()
install(TARGETS ${installable_libs}
        EXPORT "${LIBRARY_NAME}Targets"
        ARCHIVE DESTINATION "${CMAKE_INSTALL_LIBDIR}"
        LIBRARY DESTINATION "${CMAKE_INSTALL_LIBDIR}"
        RUNTIME DESTINATION "${CMAKE_INSTALL_BINDIR}")
install(EXPORT "${LIBRARY_NAME}Targets"
        NAMESPACE "${PROJECT_NAME}::"
        DESTINATION "${CMAKE_INSTALL_DATADIR}/${LIBRARY_NAME}")

install(FILES ${Header_Files} "${_export_file}"
        DESTINATION "${CMAKE_INSTALL_INCLUDEDIR}")

option(C89STRINGUTILS_BUILD_AMALGAMATION "Generate STB-style single-file header amalgamation" OFF)
if (C89STRINGUTILS_BUILD_AMALGAMATION)
    set(amalg_file "${PROJECT_BINARY_DIR}/${PROJECT_NAME}_amalgamation.h")
    set_property(DIRECTORY APPEND PROPERTY CMAKE_CONFIGURE_DEPENDS
        "c89stringutils_export_pregen.h"
        "c89stringutils_log.h"
        "c89stringutils_safecrt.h"
        "c89stringutils_string_extras.h"
        "c89stringutils_safecrt.c"
        "c89stringutils_string_extras.c")

    file(READ "${CMAKE_CURRENT_SOURCE_DIR}/c89stringutils_export_pregen.h" amalg_export)
    file(READ "${CMAKE_CURRENT_SOURCE_DIR}/c89stringutils_log.h" amalg_log)
    file(READ "${CMAKE_CURRENT_SOURCE_DIR}/c89stringutils_safecrt.h" amalg_safecrt_header)
    file(READ "${CMAKE_CURRENT_SOURCE_DIR}/c89stringutils_string_extras.h" amalg_header)
    file(READ "${CMAKE_CURRENT_SOURCE_DIR}/c89stringutils_safecrt.c" amalg_safecrt_source)
    file(READ "${CMAKE_CURRENT_SOURCE_DIR}/stb_sprintf.h" amalg_stb)
    file(READ "${CMAKE_CURRENT_SOURCE_DIR}/c89stringutils_string_extras.c" amalg_source)
    file(READ "${PROJECT_BINARY_DIR}/${PROJECT_NAME}Config.h" amalg_config)

    string(REPLACE "#include \"c89stringutils_export.h\"" "" amalg_header "${amalg_header}")
    string(REPLACE "#include \"c89stringutilsConfig.h\"" "" amalg_header "${amalg_header}")
    string(REPLACE "#include \"c89stringutils_export.h\"" "" amalg_log "${amalg_log}")
    string(REPLACE "#include \"c89stringutilsConfig.h\"" "" amalg_log "${amalg_log}")
    string(REPLACE "#include \"c89stringutils_safecrt.h\"" "" amalg_safecrt_source "${amalg_safecrt_source}")
    string(REPLACE "#include \"c89stringutils_export.h\"" "" amalg_safecrt_header "${amalg_safecrt_header}")
    string(REPLACE "#include \"c89stringutils_string_extras.h\"" "" amalg_safecrt_header "${amalg_safecrt_header}")
    string(REPLACE "#include \"c89stringutils_log.h\"" "" amalg_safecrt_source "${amalg_safecrt_source}")
    string(REPLACE "#include \"c89stringutils_string_extras.h\"" "" amalg_source "${amalg_source}")
    string(REPLACE "#include \"c89stringutils_safecrt.h\"" "" amalg_source "${amalg_source}")
    string(REPLACE "#include \"c89stringutils_log.h\"" "" amalg_source "${amalg_source}")
    string(REPLACE "#ifdef __cplusplus\nextern \"C\" {\n#endif /* __cplusplus */" "" amalg_stb "${amalg_stb}")
    string(REPLACE "#ifdef __cplusplus\n}\n#endif /* __cplusplus */" "" amalg_stb "${amalg_stb}")
    string(REPLACE "#include \"stb_sprintf.h\"" "${amalg_stb}" amalg_source "${amalg_source}")

    string(REPLACE "#ifdef __cplusplus\nextern \"C\" {\n#endif /* __cplusplus */" "" amalg_export "${amalg_export}")
    string(REPLACE "#ifdef __cplusplus\n}\n#endif /* __cplusplus */" "" amalg_export "${amalg_export}")
    string(REPLACE "#ifdef __cplusplus\nextern \"C\" {\n#endif /* __cplusplus */" "" amalg_log "${amalg_log}")
    string(REPLACE "#ifdef __cplusplus\n}\n#endif /* __cplusplus */" "" amalg_log "${amalg_log}")
    string(REPLACE "#ifdef __cplusplus\nextern \"C\" {\n#endif /* __cplusplus */" "" amalg_header "${amalg_header}")
    string(REPLACE "#ifdef __cplusplus\n}\n#endif /* __cplusplus */" "" amalg_header "${amalg_header}")
    string(REPLACE "#ifdef __cplusplus\nextern \"C\" {\n#endif /* __cplusplus */" "" amalg_safecrt_header "${amalg_safecrt_header}")
    string(REPLACE "#ifdef __cplusplus\n}\n#endif /* __cplusplus */" "" amalg_safecrt_header "${amalg_safecrt_header}")

    foreach(_amalg_var amalg_export amalg_log amalg_header amalg_safecrt_header amalg_safecrt_source amalg_source)
        string(REPLACE "/* clang-format off */\n" "" ${_amalg_var} "${${_amalg_var}}")
        string(REPLACE "/* clang-format on */\n" "" ${_amalg_var} "${${_amalg_var}}")
        string(REPLACE "/* clang-format off */" "" ${_amalg_var} "${${_amalg_var}}")
        string(REPLACE "/* clang-format on */" "" ${_amalg_var} "${${_amalg_var}}")
    endforeach()

    set(amalgamation "/* c89stringutils Amalgamation STB-style */\n\n")
    string(APPEND amalgamation "#ifndef C89STRINGUTILS_AMALGAMATION_H\n#define C89STRINGUTILS_AMALGAMATION_H\n\n")
    string(APPEND amalgamation "/* clang-format off */\n")
    string(APPEND amalgamation "#include <stdarg.h>\n#include <stddef.h>\n#include <stdio.h>\n#include <stdlib.h>\n#include <string.h>\n#include <errno.h>\n#include <ctype.h>\n#include <limits.h>\n\n")
    string(APPEND amalgamation "${amalg_export}\n\n")
    string(APPEND amalgamation "${amalg_config}\n\n")
    string(APPEND amalgamation "#if defined(C89STRINGUTILS_HAVE_WCHAR_H)\n#include <wchar.h>\n#endif\n")
    string(APPEND amalgamation "#if defined(C89STRINGUTILS_HAVE_SYS_PARAM_H)\n#include <sys/param.h>\n#endif\n")
    string(APPEND amalgamation "#if defined(C89STRINGUTILS_HAVE_STRINGS_H)\n#include <strings.h>\n#endif\n")
    string(APPEND amalgamation "/* clang-format on */\n\n")
    string(APPEND amalgamation "#ifdef __cplusplus\nextern \"C\" {\n#endif /* __cplusplus */\n\n")
    string(APPEND amalgamation "${amalg_log}\n\n")
    string(APPEND amalgamation "${amalg_header}\n\n")
    string(APPEND amalgamation "${amalg_safecrt_header}\n\n")
    string(APPEND amalgamation "#ifdef C89STRINGUTILS_IMPLEMENTATION\n\n")
    string(APPEND amalgamation "${amalg_safecrt_source}\n\n")
    string(APPEND amalgamation "${amalg_source}\n\n")
    string(APPEND amalgamation "#endif /* C89STRINGUTILS_IMPLEMENTATION */\n\n")
    string(APPEND amalgamation "#ifdef __cplusplus\n}\n#endif /* __cplusplus */\n\n")
    string(APPEND amalgamation "#endif /* C89STRINGUTILS_AMALGAMATION_H */\n")

    file(WRITE "${amalg_file}" "${amalgamation}")

    install(FILES "${amalg_file}"
            DESTINATION "${CMAKE_INSTALL_INCLUDEDIR}")
endif()

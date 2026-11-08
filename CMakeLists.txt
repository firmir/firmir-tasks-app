cmake_minimum_required(VERSION 3.20)
project(firmir-tasks CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Firmir dependency: prefer the installed libraries via pkg-config
# (firmir-gui-controls.pc pulls in firmir-gui); fall back to a sibling
# ../firmir checkout otherwise.
# FIRMIR_ROOT points at a firmir source checkout: builds against it
# directly (no installation needed). Empty -> try the installed firmir.
set(FIRMIR_ROOT "" CACHE PATH "firmir source tree to build against")
find_package(PkgConfig QUIET)
if(PkgConfig_FOUND AND NOT FIRMIR_ROOT)
    pkg_check_modules(FIRMIR_PC QUIET IMPORTED_TARGET firmir-gui-controls)
endif()
if(FIRMIR_ROOT)
    # Explicit source-tree build: drop any stale pkg-config results
    # cached by an earlier configuration of this build directory.
    set(FIRMIR_PC_FOUND FALSE)
endif()
if(FIRMIR_PC_FOUND)
    message(STATUS "firmir-tasks: using firmir via pkg-config "
                   "(${FIRMIR_PC_VERSION})")
    # The UI is written in Firml: compile it with the installed firmlc.
    pkg_get_variable(FIRMIR_PREFIX firmir-gui prefix)
    include(${FIRMIR_PREFIX}/share/firmir/cmake/FirmirFirml.cmake)
    firmir_add_fml(tasks_codegen SOURCES tasks.fml
        INCLUDE_DIRS ${CMAKE_CURRENT_SOURCE_DIR})
else()
    if(NOT FIRMIR_ROOT)
        get_filename_component(FIRMIR_ROOT "${CMAKE_CURRENT_SOURCE_DIR}/../firmir"
                               ABSOLUTE)
    endif()
    if(NOT EXISTS "${FIRMIR_ROOT}/CMakeLists.txt")
        message(FATAL_ERROR
            "firmir not found via pkg-config and no sibling checkout at "
            "${FIRMIR_ROOT} — either 'sudo make install' the firmir project "
            "or clone it next to this directory.")
    endif()
    message(STATUS "firmir-tasks: using firmir from ${FIRMIR_ROOT}")
    set(FIRMIR_BUILD_TESTS OFF CACHE BOOL "" FORCE)
    set(FIRMIR_BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)
    set(FIRMIR_BUILD_DOC_TOOL OFF CACHE BOOL "" FORCE)
    set(FIRMIR_BUILD_TOOL_FIRML ON CACHE BOOL "" FORCE)
    add_subdirectory(${FIRMIR_ROOT} ${CMAKE_BINARY_DIR}/firmir)
    include(${FIRMIR_ROOT}/tools/firml/cmake/FirmlCodegen.cmake)
    firml_add_target(tasks_codegen SOURCES tasks.fml
        INCLUDE_DIRS ${CMAKE_CURRENT_SOURCE_DIR}
                     ${FIRMIR_ROOT}/libs/gui-controls/include
                     ${FIRMIR_ROOT}/libs/gui/include)
    target_include_directories(tasks_codegen PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
endif()

add_executable(firmir-tasks main.cpp TaskApp.cpp)
target_include_directories(firmir-tasks PRIVATE ${CMAKE_CURRENT_SOURCE_DIR})
if(FIRMIR_PC_FOUND)
    target_link_libraries(firmir-tasks PRIVATE tasks_codegen PkgConfig::FIRMIR_PC)
else()
    target_link_libraries(firmir-tasks PRIVATE tasks_codegen firmir-gui-controls firmir-gui)
endif()

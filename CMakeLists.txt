cmake_minimum_required(VERSION 3.22)

project(xgd-project VERSION 1.0.1 LANGUAGES C CXX)

enable_testing()

set(XGD_THIRD_PARTY_DIR ${CMAKE_CURRENT_SOURCE_DIR}/third_party)

include(cmake/cmake-utils.cmake)
include(cmake/project-config.cmake)

if (NOT XGD_USE_QT)
    message(STATUS "XGD_USE_QT set to OFF, check if
    CMAKE_PREFIX_PATH=\"${CMAKE_PREFIX_PATH}\" AND
    CMAKE_FIND_ROOT_PATH=\"${CMAKE_FIND_ROOT_PATH}\"
    are pointed to valid qt installation")
endif ()

add_subdirectory(apps)
add_subdirectory(lib)
add_subdirectory(shadow_build)
add_subdirectory(hello_world)

if (XGD_USE_QT)
    add_subdirectory(${XGD_THIRD_PARTY_DIR}/fluentui-src/fluentui/src/Qt6)
endif ()

if (NOT PYTHON_EXECUTABLE)
    message(STATUS "PYTHON_EXECUTABLE not set, python binding will not be built")
else ()
    add_subdirectory(python/lib)
endif ()

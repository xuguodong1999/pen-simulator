project(pen_simulator)

aux_source_directory(${CMAKE_CURRENT_SOURCE_DIR} PROJECT_SOURCES)

pybind11_add_module(${PROJECT_NAME} ${PROJECT_SOURCES})
set_target_properties(${PROJECT_NAME} PROPERTIES
        CXX_STANDARD 20
        CXX_STANDARD_REQUIRED ON)
target_compile_definitions(${PROJECT_NAME} PRIVATE PYBIND11_DETAILED_ERROR_MESSAGES)

xgd_link_libraries(${PROJECT_NAME} PRIVATE AzMath AzData AzPen AzCore eigen spdlog taskflow skia)
target_include_directories(${PROJECT_NAME} PRIVATE ${NUMPY_INCLUDE_DIR})

project(az-data-cli)

xgd_add_executable(
        ${PROJECT_NAME}
        SRC_DIRS ${CMAKE_CURRENT_SOURCE_DIR}
        INCLUDE_DIRS ${CMAKE_CURRENT_SOURCE_DIR}/../../include
)

xgd_link_libraries(
        ${PROJECT_NAME}
        PRIVATE
        AzData
        AzMath
        AzJs
        spdlog
        skia
        opencv_highgui
        boost_program_options
        boost_property_tree
        taskflow
)

project(az-test-data)
xgd_add_executable(
        ${PROJECT_NAME}
        SRC_DIRS
        ${CMAKE_CURRENT_SOURCE_DIR}
        INCLUDE_DIRS
        ${CMAKE_CURRENT_SOURCE_DIR}/../../include
)
xgd_link_gtest(${PROJECT_NAME})
xgd_link_libraries(
        ${PROJECT_NAME}
        PRIVATE
        AzMedia
        AzData
        AzJs
        skia
        opencv_imgproc
        boost_locale
        boost_property_tree
        spdlog
)

if (EMSCRIPTEN)
    xgd_emcc_link_rawfs_net(${PROJECT_NAME})
endif ()

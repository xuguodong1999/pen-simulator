project(az-test-tex)
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
        AzTex
        AzMedia
        skia
        spdlog
)

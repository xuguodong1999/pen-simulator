project(AzCore)
xgd_add_library(${PROJECT_NAME}
        SRC_DIRS ${CMAKE_CURRENT_SOURCE_DIR}
        INCLUDE_DIRS ${CMAKE_CURRENT_SOURCE_DIR}/../../include)
xgd_generate_export_header(${PROJECT_NAME} ${PROJECT_NAME} ".h")
xgd_link_libraries(
        ${PROJECT_NAME}
        PUBLIC
        Eigen3::Eigen
        boost_geometry
        boost_rational
        PRIVATE
        boost_math
        spdlog
)

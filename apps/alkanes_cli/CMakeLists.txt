project(alkanes-cli)

xgd_add_executable(
        ${PROJECT_NAME}
        SRC_DIRS ${CMAKE_CURRENT_SOURCE_DIR}
        INCLUDE_DIRS ${CMAKE_CURRENT_SOURCE_DIR}/../../include
)

xgd_link_libraries(
        ${PROJECT_NAME}
        PRIVATE
        AzMath
        spdlog
        boost_program_options
)

project(AzChem)
xgd_add_library(${PROJECT_NAME}
        SRC_DIRS ${CMAKE_CURRENT_SOURCE_DIR}
        INCLUDE_DIRS ${CMAKE_CURRENT_SOURCE_DIR}/../../include)
xgd_generate_export_header(${PROJECT_NAME} ${PROJECT_NAME} ".h")
xgd_link_libraries(
        ${PROJECT_NAME}
        PUBLIC
        AzCore
        PRIVATE
        boost_graph
        spdlog
        AzSpdStream
        openbabel
        coordgen
        rdkit_GraphMol
        rdkit_FileParsers
        rdkit_DistGeomHelpers
        rdkit_ForceFieldHelpers
        rdkit_RDInchiLib
)
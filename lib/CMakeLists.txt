add_subdirectory(az_chem)
add_subdirectory(az_core)
add_subdirectory(az_data)
add_subdirectory(az_js)
add_subdirectory(az_math)
add_subdirectory(az_media)
add_subdirectory(az_pen)
add_subdirectory(az_svg)
add_subdirectory(az_tex)

function(xgd_add_init_libraries TARGET)
    xgd_add_library(${TARGET}
            SRC_FILES
            impl.cpp
            INCLUDE_DIRS
            ${CMAKE_CURRENT_SOURCE_DIR}/../../include)
    xgd_generate_export_header(${TARGET} ${TARGET} ".h")
endfunction()

add_subdirectory(az_init_openbabel)
if (XGD_USE_QT)
    add_subdirectory(az_init_qt)
endif ()
add_subdirectory(az_spdstream)

if (Qt${QT_VERSION_MAJOR}Quick3D_FOUND)
    add_subdirectory(az_openpix_gui)
endif ()
if (XGD_USE_QT)
    add_subdirectory(az_data_cli)
    add_subdirectory(az_data_gui)
endif ()
add_subdirectory(az_test_chem)
add_subdirectory(az_test_core)
add_subdirectory(az_test_data)
add_subdirectory(az_test_js)
add_subdirectory(az_test_math)
add_subdirectory(az_test_openpix)
add_subdirectory(az_test_pen)
add_subdirectory(az_test_svg)
add_subdirectory(az_test_tex)
add_subdirectory(az_test_spdstream)

### deprecated below ###

if (XGD_USE_QT)
    add_subdirectory(COCR)
    add_subdirectory(COCR_data_gen)
endif ()

add_subdirectory(copy_deps)

add_subdirectory(decrypt_qmc)

add_subdirectory(dedup_music)

add_subdirectory(png_compress)

add_subdirectory(rcsb_util)

if (XGD_USE_TORCH)
    add_subdirectory(rwkv_torch_cli)
endif ()

add_subdirectory(test_base)

if (XGD_USE_QT)
    add_subdirectory(equation_balance_solver)
    add_subdirectory(test_COCR)
endif ()

if (XGD_USE_CUDA)
    add_subdirectory(test_cuda)
endif ()

if (XGD_USE_QT)
    add_subdirectory(test_qt)
    add_subdirectory(test_qhybrid)
endif ()

if (XGD_USE_VK)
    add_subdirectory(test_vulkan)
endif ()


if (XGD_USE_QT)
    add_subdirectory(vue_qt_renderer)
endif ()

#if (XGD_USE_QT)
#    add_subdirectory(whisper_qt_gui)
#endif ()

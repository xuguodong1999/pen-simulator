# nanosvg
set(ROOT_DIR ${XGD_THIRD_PARTY_DIR}/nanosvg-src/nanosvg/src)
set(GEN_DIR ${CMAKE_CURRENT_BINARY_DIR}/generated/src/nanosvg)
# CMake needs *.c files to do something useful
configure_file(${ROOT_DIR}/nanosvg.h ${GEN_DIR}/nanosvg.c)
configure_file(${ROOT_DIR}/nanosvgrast.h ${GEN_DIR}/nanosvgrast.c)

xgd_add_library(nanosvg STATIC SRC_FILES ${GEN_DIR}/nanosvg.c INCLUDE_DIRS ${ROOT_DIR})
target_compile_definitions(nanosvg PRIVATE NANOSVG_IMPLEMENTATION)

xgd_add_library(nanosvgrast STATIC SRC_FILES ${GEN_DIR}/nanosvgrast.c INCLUDE_DIRS ${ROOT_DIR})
xgd_link_libraries(nanosvgrast PUBLIC nanosvg)
target_compile_definitions(nanosvgrast PRIVATE NANOSVGRAST_IMPLEMENTATION)

# agg
set(ROOT_DIR ${XGD_THIRD_PARTY_DIR}/agg-src/agg/agg-src)
set(INC_DIR ${ROOT_DIR}/include)
set(SRC_DIR ${ROOT_DIR}/src)
xgd_add_library(agg STATIC
        SRC_FILES
        ${SRC_DIR}/agg_curves.cpp
        ${SRC_DIR}/agg_trans_affine.cpp
        ${SRC_DIR}/agg_vcgen_dash.cpp
        ${SRC_DIR}/agg_vcgen_stroke.cpp
        INCLUDE_DIRS ${INC_DIR})
xgd_disable_weak_warnings(agg)

# quickjs
set(ROOT_DIR ${XGD_THIRD_PARTY_DIR}/quickjs-src/quickjs)
set(INC_DIR ${ROOT_DIR}/include)
set(SRC_DIR ${ROOT_DIR}/src)
xgd_add_library(quickjs STATIC
        SRC_FILES ${SRC_DIR}/cutils.c
        ${SRC_DIR}/libbf.c
        ${SRC_DIR}/libregexp.c
        ${SRC_DIR}/libunicode.c
        ${SRC_DIR}/quickjs-libc.c
        ${SRC_DIR}/quickjs.c
        PRIVATE_INCLUDE_DIRS ${INC_DIR}/quickjs
        INCLUDE_DIRS ${INC_DIR})
target_compile_definitions(quickjs PRIVATE
        "CONFIG_BIGNUM"
        "CONFIG_VERSION=\"2021-03-27\"")
if (NOT EMSCRIPTEN AND NOT WIN32)
    xgd_link_threads(quickjs LINK_TYPE PRIVATE)
endif ()
# flatbuffers
set(ROOT_DIR ${XGD_THIRD_PARTY_DIR}/flatbuffers-src/flatbuffers)
set(INC_DIR ${ROOT_DIR}/include)
set(SRC_DIR ${ROOT_DIR}/src)

xgd_add_library(flatbuffers STATIC
        SRC_FILES
        ${SRC_DIR}/idl_parser.cpp
        ${SRC_DIR}/idl_gen_text.cpp
        ${SRC_DIR}/reflection.cpp
        ${SRC_DIR}/util.cpp
        INCLUDE_DIRS ${INC_DIR})

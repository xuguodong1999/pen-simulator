# arrow
set(ROOT_DIR ${XGD_THIRD_PARTY_DIR}/arrow-src/arrow)
set(INC_DIR ${ROOT_DIR}/cpp/src)
set(SRC_DIR ${ROOT_DIR}/cpp/src)
set(GEN_DIR ${CMAKE_CURRENT_BINARY_DIR}/generated/arrow/include)

set(ARROW_VERSION_MAJOR 15)
set(ARROW_VERSION_MINOR 0)
set(ARROW_VERSION_PATCH 2)
set(ARROW_SO_VERSION 15)
set(ARROW_FULL_SO_VERSION 15.0.2)
set(ARROW_VERSION 15.0.2)

configure_file(
        ${SRC_DIR}/arrow/util/config.h.cmake
        ${GEN_DIR}/arrow/util/config.h
        @ONLY
)

configure_file(
        ${SRC_DIR}/parquet/parquet_version.h.in
        ${GEN_DIR}/parquet/parquet_version.h
        @ONLY
)

xgd_add_library(
        arrow
        SRC_DIRS
        ${SRC_DIR}/arrow
        ${SRC_DIR}/arrow/c
        ${SRC_DIR}/arrow/compute
        ${SRC_DIR}/arrow/io
        ${SRC_DIR}/arrow/tensor
        ${SRC_DIR}/arrow/util
        ${SRC_DIR}/arrow/vendored
        ${SRC_DIR}/arrow/integration # ARROW_BUILD_INTEGRATION
        ${SRC_DIR}/arrow/csv # ARROW_CSV
        ${SRC_DIR}/arrow/acero # ARROW_ACERO
        ${SRC_DIR}/arrow/dataset # ARROW_DATASET
        ${SRC_DIR}/arrow/filesystem # ARROW_FILESYSTEM
#        ${SRC_DIR}/arrow/flight # ARROW_FLIGHT
#        ${SRC_DIR}/arrow/ipc # ARROW_IPC
        ${SRC_DIR}/arrow/json # ARROW_JSON
        ${SRC_DIR}/arrow/extension # ARROW_JSON
        ${SRC_DIR}/arrow/engine # ARROW_SUBSTRAIT
        EXCLUDE_REGEXES
        "^(.*)test(.*)\\.cc"
        "^(.*)benchmark(.*)\\.cc"

        "^(.*)jemalloc(.*)\\.cc"

        "^(.*)avx(.*)\\.cc"
        "^(.*)neon(.*)\\.cc"

        "^(.*)bz2(.*)\\.cc"
        "^(.*)snappy(.*)\\.cc"
        "^(.*)lz4(.*)\\.cc"
        "^(.*)brotli(.*)\\.cc"

        "^(.*)tracing_internal(.*)\\.cc"

        "^(.*)s3(.*)\\.cc"
        "^(.*)hdfs(.*)\\.cc"
        "^(.*)gcsfs(.*)\\.cc"
        "^(.*)azure(.*)\\.cc"
        INCLUDE_DIRS ${INC_DIR} ${GEN_DIR}
)

xgd_generate_export_header(arrow "arrow" ".h")
xgd_generate_export_header(parquet "parquet" ".h")
xgd_link_libraries(
        arrow
        PRIVATE
        zlib
        zstd
        boost_multiprecision
        boost_crc
        boost_filesystem
        protobuf
        rapidjson
)

xgd_add_library(
        parquet
        SRC_DIRS
        ${SRC_DIR}/parquet
        ${SRC_DIR}/parquet/api
        ${SRC_DIR}/parquet/arrow
        ${SRC_DIR}/parquet/encryption
        EXCLUDE_REGEXES
        "^(.*)test(.*)\\.cc"
        "^(.*)benchmark(.*)\\.cc"
        "^(.*)nossl(.*)\\.cc"
        INCLUDE_DIRS ${INC_DIR} ${GEN_DIR}
)

xgd_link_libraries(
        parquet
        PRIVATE
        arrow
        thrift
        ssl
)

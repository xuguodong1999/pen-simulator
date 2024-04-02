# snappy
set(ROOT_DIR ${XGD_THIRD_PARTY_DIR}/snappy-src/snappy)
set(INC_DIR ${ROOT_DIR})
set(SRC_DIR ${ROOT_DIR})
set(GEN_DIR ${CMAKE_CURRENT_BINARY_DIR}/generated/snappy/include)


set(SNAPPY_IS_BIG_ENDIAN 1)
set(HAVE_SYS_MMAN_H 0)
set(HAVE_SYS_RESOURCE_H 0)
set(HAVE_SYS_TIME_H ${${XGD_HAVE_SYS_TIME_H}})
set(HAVE_SYS_UIO_H 0)
set(HAVE_SYS_UIO_H_01 ${HAVE_SYS_UIO_H})

set(HAVE_UNISTD_H ${XGD_HAVE_UNISTD_H})
set(HAVE_WINDOWS_H ${WIN32})
set(HAVE_LIBZ 1)
set(HAVE_LIBLZO2 0)
set(HAVE_LIBLZ4 0)
set(HAVE_VISUAL_STUDIO_ARCH_AVX ${XGD_FLAG_AVX})
set(HAVE_VISUAL_STUDIO_ARCH_AVX2 ${XGD_FLAG_AVX2})

set(HAVE_CLANG_MAVX 0)
set(HAVE_CLANG_MBMI2 0)

set(HAVE_BUILTIN_EXPECT 0)
set(HAVE_BUILTIN_CTZ 0)

set(HAVE_ATTRIBUTE_ALWAYS_INLINE 0)
set(SNAPPY_HAVE_SSSE3 ${XGD_FLAG_SSE_ALL})
set(SNAPPY_HAVE_X86_CRC32 0)
set(SNAPPY_HAVE_NEON_CRC32 0)
set(SNAPPY_HAVE_BMI2 0)
set(SNAPPY_HAVE_NEON ${XGD_FLAG_NEON})

set(HAVE_FUNC_MMAP 0)
set(HAVE_FUNC_SYSCONF 0)

configure_file(${SRC_DIR}/cmake/config.h.in ${GEN_DIR}/config.h)

set(PROJECT_VERSION_MAJOR 1)
set(PROJECT_VERSION_MINOR 1)
set(PROJECT_VERSION_PATCH 10)
configure_file(${SRC_DIR}/snappy-stubs-public.h.in ${GEN_DIR}/snappy-stubs-public.h)

xgd_add_library(
        snappy STATIC
        SRC_FILES
        ${SRC_DIR}/snappy.cc
        ${SRC_DIR}/snappy-stubs-internal.cc
        ${SRC_DIR}/snappy-sinksource.cc
        ${SRC_DIR}/snappy-c.cc
        INCLUDE_DIRS ${INC_DIR} ${GEN_DIR}
)
# xgd_link_libraries(snappy PRIVATE zlib)
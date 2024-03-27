# thrift
set(ROOT_DIR ${XGD_THIRD_PARTY_DIR}/thrift-src/thrift)
set(INC_DIR ${ROOT_DIR}/lib/cpp/src)
set(SRC_DIR ${ROOT_DIR}/lib/cpp/src/thrift)
set(GEN_DIR ${CMAKE_CURRENT_BINARY_DIR}/generated/thrift/include)

#    set(PACKAGE_VERSION "${PACKAGE_VERSION}")
#    set(PACKAGE_STRING "${PACKAGE_STRING}")
#    set(ARITHMETIC_RIGHT_SHIFT 0)
#    set(SIGNED_RIGHT_SHIFT_IS 0)
#
#    set(PACKAGE "${PACKAGE}")
#    set(PACKAGE_BUGREPORT "${PACKAGE_BUGREPORT}")
#    set(PACKAGE_NAME "${PACKAGE_NAME}")
#    set(PACKAGE_TARNAME "${PACKAGE_TARNAME}")
#    set(PACKAGE_URL "${PACKAGE_URL}")

if (WIN32)
    set(STRERROR_R_CHAR_P 0)
    set(HAVE_NETINET_IN_H 0)
    set(HAVE_SYS_SOCKET_H 0)
    set(HAVE_STRERROR_R 0)
else ()
    if (EMSCRIPTEN)
        set(STRERROR_R_CHAR_P 0)
    else ()
        set(STRERROR_R_CHAR_P 1)
    endif ()
    set(HAVE_NETINET_IN_H 1)
    set(HAVE_SYS_SOCKET_H 1)
    set(HAVE_STRERROR_R 1)
endif ()

set(AI_ADDRCONFIG 0)
set(BISON_USE_PARSER_H_EXTENSION 0)
set(HAVE_ARPA_INET_H 0)
set(HAVE_FCNTL_H 0)
set(HAVE_INTTYPES_H 0)
set(HAVE_NETDB_H 0)
set(HAVE_SIGNAL_H 0)
set(HAVE_STDINT_H 0)
set(HAVE_UNISTD_H 0)
set(HAVE_PTHREAD_H 0)
set(HAVE_SYS_IOCTL_H 0)
set(HAVE_SYS_PARAM_H 0)
set(HAVE_SYS_RESOURCE_H 0)
set(HAVE_SYS_STAT_H 0)
set(HAVE_SYS_UN_H 0)
set(HAVE_POLL_H 0)
set(HAVE_SYS_POLL_H 0)
set(HAVE_SYS_SELECT_H 0)
set(HAVE_SYS_TIME_H 0)
set(HAVE_SCHED_H 0)
set(HAVE_STRINGS_H 0)
set(HAVE_AF_UNIX_H 0)
set(HAVE_GETHOSTBYNAME 0)
set(HAVE_GETHOSTBYNAME_R 0)
set(HAVE_SCHED_GET_PRIORITY_MAX 0)
set(HAVE_SCHED_GET_PRIORITY_MIN 0)
configure_file(
        ${ROOT_DIR}/build/cmake/config.h.in
        ${GEN_DIR}/thrift/config.h
        @ONLY
)

set(THRIFT_SRC_DIRS ${SRC_DIR}
        ${SRC_DIR}/async
        ${SRC_DIR}/concurrency
        ${SRC_DIR}/processor
        ${SRC_DIR}/protocol
        ${SRC_DIR}/server
        ${SRC_DIR}/transport
)

if (WIN32)
    list(APPEND THRIFT_SRC_DIRS ${SRC_DIR}/windows)
endif ()

xgd_add_library(
        thrift
        STATIC
        SRC_DIRS
        ${THRIFT_SRC_DIRS}
        EXCLUDE_REGEXES
        "^(.*)TEv(.*)\\.cpp"
        "^(.*)TNonblocking(.*)\\.cpp"
        "^(.*)TFileTransport(.*)\\.cpp"
        "^(.*)Socket(.*)\\.cpp"
        INCLUDE_DIRS ${INC_DIR} ${GEN_DIR}
)
xgd_generate_export_header(thrift "thrift" ".hpp")

xgd_link_libraries(
        thrift
        PRIVATE
        zlib
        ssl
        boost_locale
        boost_tokenizer
        boost_algorithm
        boost_scope_exit
        PUBLIC
        boost_numeric_conversion
)

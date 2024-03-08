# cppcoro
set(ROOT_DIR ${XGD_THIRD_PARTY_DIR}/cppcoro-src/cppcoro)
set(INC_DIR ${ROOT_DIR}/include)
set(SRC_DIR ${ROOT_DIR}/lib)

set(WIN32_SRC_FILES
        win32.cpp
        io_service.cpp
        file.cpp
        readable_file.cpp
        writable_file.cpp
        read_only_file.cpp
        write_only_file.cpp
        read_write_file.cpp
        file_read_operation.cpp
        file_write_operation.cpp
        socket_helpers.cpp
        socket.cpp
        socket_accept_operation.cpp
        socket_connect_operation.cpp
        socket_disconnect_operation.cpp
        socket_send_operation.cpp
        socket_send_to_operation.cpp
        socket_recv_operation.cpp
        socket_recv_from_operation.cpp)

set(WIN32_SRC_PATHS "")
foreach (SRC_FILE ${WIN32_SRC_FILES})
    list(APPEND WIN32_SRC_PATHS ${SRC_DIR}/${SRC_FILE})
endforeach ()

xgd_add_library(cppcoro
        STATIC
        SRC_DIRS ${SRC_DIR}
        INCLUDE_DIRS ${INC_DIR}
        EXCLUDE_SRC_FILES ${WIN32_SRC_PATHS})

# TODO: make it available to WIN32
if (WIN32)
    xgd_add_library(cppcoro_win32
            STATIC
            SRC_FILES ${WIN32_SRC_PATHS}
            INCLUDE_DIRS ${INC_DIR})
    xgd_link_libraries(cppcoro_win32 PUBLIC cppcoro)
    target_link_libraries(cppcoro_win32 PRIVATE ws2_32 wsock32)
    target_compile_definitions(cppcoro_win32 PRIVATE _WIN32_WINNT=0x0601)
endif ()

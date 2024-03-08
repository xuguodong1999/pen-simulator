project(pen-simulator)

if (NOT PYTHON_EXECUTABLE)
    message(FATAL_ERROR "PYTHON_EXECUTABLE=${PYTHON_EXECUTABLE}")
endif ()

execute_process(COMMAND ${PYTHON_EXECUTABLE} -m pybind11 --cmake
        OUTPUT_VARIABLE pybind11_DIR OUTPUT_STRIP_TRAILING_WHITESPACE)
if (NOT EXISTS ${pybind11_DIR})
    message(FATAL_ERROR "pybind11 not found, pybind11_DIR=${pybind11_DIR}")
endif ()

execute_process(COMMAND ${PYTHON_EXECUTABLE} -c "import numpy;print(numpy.get_include())"
        OUTPUT_VARIABLE NUMPY_INCLUDE_DIR OUTPUT_STRIP_TRAILING_WHITESPACE)
if (NOT EXISTS ${NUMPY_INCLUDE_DIR})
    message(FATAL_ERROR "numpy not found, NUMPY_INCLUDE_DIR=${NUMPY_INCLUDE_DIR}")
endif ()

if (POLICY CMP0148)
    cmake_policy(SET CMP0148 OLD)
endif ()

find_package(pybind11 REQUIRED)

add_subdirectory(pen_simulator)
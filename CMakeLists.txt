cmake_minimum_required(VERSION 3.26)
project(DA2324_PRJ2_G163)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
# Doxygen Build
find_package(Doxygen)
if (DOXYGEN_FOUND)
    set(BUILD_DOC_DIR "${CMAKE_SOURCE_DIR}/docs/output")
    if (NOT EXISTS "${BUILD_DOC_DIR}")
        file(MAKE_DIRECTORY "${BUILD_DOC_DIR}")
    endif ()

    set(DOXYGEN_IN "${CMAKE_CURRENT_SOURCE_DIR}/docs/Doxyfile")
    set(DOXYGEN_OUT "${CMAKE_CURRENT_BINARY_DIR}/Doxyfile")
    configure_file("${DOXYGEN_IN}" "${DOXYGEN_OUT}" @ONLY)

    message("Doxygen build started")
    add_custom_target(Doxygen ALL
            COMMAND "${DOXYGEN_EXECUTABLE}" "${DOXYGEN_OUT}"
            WORKING_DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}"
            COMMENT "Generating API documentation with Doxygen"
            VERBATIM)
else (DOXYGEN_FOUND)
    message("Doxygen needs to be installed to generate the documentation.")
endif (DOXYGEN_FOUND)

# Project build
add_executable(DA2324_PRJ2_G163 main.cpp
        src/data/Graph.hpp
        lib/MutablePriorityQueue.h
        src/Utils.h src/Utils.cpp
        src/Parsum.hpp
        src/CSV.hpp
        src/data/Info.cpp src/data/Info.h
        src/data/Data.cpp src/data/Data.h
        src/Runtime.cpp src/Runtime.h

)

#!/usr/bin/env bash
# Build script for g++ (Linux / MSYS2 / Git Bash on Windows)
set -euo pipefail

CXX="${CXX:-g++}"
CXXFLAGS="-std=c++17 -Wall -Wextra -O2"

SRCS=(
    pdata.cpp
    rnp_miner.cpp
    rnp_b.cpp
    rnp_d.cpp
    rnp_pup.cpp
    dfom_rnp.cpp
    nosep_rnp.cpp
    pro_rnp.cpp
    prefixspan.cpp
    main.cpp
)

TARGET="rnp_miner_main"

echo "Compiling with ${CXX}..."
"${CXX}" ${CXXFLAGS} -o "${TARGET}" "${SRCS[@]}"

echo "Done: ${TARGET}"

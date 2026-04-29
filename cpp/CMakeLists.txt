cmake_minimum_required(VERSION 3.14)
project(RNP-Miner LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Common header-only
add_library(common INTERFACE)
target_include_directories(common INTERFACE ${CMAKE_CURRENT_SOURCE_DIR})

# Pdata library
add_library(pdata pdata.cpp pdata.h)
target_link_libraries(pdata common)

# Algorithm libraries
add_library(rnp_miner rnp_miner.cpp rnp_miner.h)
target_link_libraries(rnp_miner common pdata)

add_library(rnp_b rnp_b.cpp rnp_b.h)
target_link_libraries(rnp_b common pdata)

add_library(rnp_d rnp_d.cpp rnp_d.h)
target_link_libraries(rnp_d common pdata)

add_library(rnp_pup rnp_pup.cpp rnp_pup.h)
target_link_libraries(rnp_pup common pdata)

add_library(dfom_rnp dfom_rnp.cpp dfom_rnp.h)
target_link_libraries(dfom_rnp common pdata)

add_library(nosep_rnp nosep_rnp.cpp nosep_rnp.h)
target_link_libraries(nosep_rnp common)

add_library(pro_rnp pro_rnp.cpp pro_rnp.h)
target_link_libraries(pro_rnp common pdata)

add_library(prefixspan prefixspan.cpp prefixspan.h)
target_link_libraries(prefixspan common)

# Main executable
add_executable(rnp_miner_main main.cpp)
target_link_libraries(rnp_miner_main
    common pdata
    rnp_miner rnp_b rnp_d rnp_pup
    dfom_rnp nosep_rnp pro_rnp
    prefixspan
)

# evmone: Fast Ethereum Virtual Machine implementation
# Copyright 2023 The evmone Authors.
# SPDX-License-Identifier: Apache-2.0

add_executable(evmone-blockchaintest)
target_link_libraries(evmone-blockchaintest PRIVATE evmone::testutils evmone evmone-buildinfo evmc::loader GTest::gtest)

find_library(DTVMAPI_LIB dtvmapi PATHS ${CMAKE_SOURCE_DIR})
if(DTVMAPI_LIB)
    set(HAVE_EXTERNAL_VM ON)
    target_compile_definitions(evmone-blockchaintest PRIVATE HAVE_EXTERNAL_VM)
    target_link_libraries(evmone-blockchaintest PRIVATE ${DTVMAPI_LIB})
    message(STATUS "External VM library found: ${DTVMAPI_LIB}")
else()
    message(STATUS "External VM library not found, building without external VM support")
endif()

target_sources(
    evmone-blockchaintest PRIVATE
    blockchaintest.cpp
    blockchaintest_runner.hpp
    blockchaintest_runner.cpp
)

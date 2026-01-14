# evmone: Fast Ethereum Virtual Machine implementation
# Copyright 2022 The evmone Authors.
# SPDX-License-Identifier: Apache-2.0

add_executable(evmone-statetest)
target_link_libraries(evmone-statetest PRIVATE evmone::testutils evmone evmone-buildinfo evmc::loader GTest::gtest)

# Link external VM library if available
find_library(DTVMAPI_LIB dtvmapi PATHS ${CMAKE_SOURCE_DIR})
if(DTVMAPI_LIB)
    target_link_libraries(evmone-statetest PRIVATE ${DTVMAPI_LIB})
    target_compile_definitions(evmone-statetest PRIVATE HAVE_EXTERNAL_VM)
    message(STATUS "Found libdtvmapi.so, enabling external VM support in statetest")
else()
    message(STATUS "libdtvmapi.so not found, external VM support disabled in statetest")
endif()
target_sources(
    evmone-statetest PRIVATE
    statetest.cpp
    statetest_runner.cpp
)

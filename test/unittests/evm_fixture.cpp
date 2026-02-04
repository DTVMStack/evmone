// evmone: Fast Ethereum Virtual Machine implementation
// Copyright 2019-2020 The evmone Authors.
// SPDX-License-Identifier: Apache-2.0

#include "evm_fixture.hpp"
#include <evmone/evmone.h>
#include <evmc/loader.h>
#include <filesystem>
#include <iostream>

namespace evmone::test
{
namespace
{
evmc::VM advanced_vm{evmc_create_evmone(), {{"advanced", ""}}};
evmc::VM baseline_vm{evmc_create_evmone()};
evmc::VM external_vm;

std::vector<evmc::VM*> get_available_vms() {
    static bool initialized = false;
    static bool external_load_state = false;
    std::vector<evmc::VM*> vms;
    vms.push_back(&advanced_vm);
    vms.push_back(&baseline_vm);

    // Load external lib
    const char* external_env_options = getenv("EVMONE_EXTERNAL_OPTIONS");
    if (external_env_options != nullptr) {
        if (!initialized) {
            external_load_state = evmone::test::try_load_external(external_env_options, external_vm);
            initialized = true;
        }
    }

    if (external_load_state) {
        vms.push_back(&external_vm);
    }

    return vms;
}

const char* print_vm_name(const testing::TestParamInfo<evmc::VM*>& info) noexcept
{
    if (info.param == &advanced_vm)
        return "evmone_advanced";
    if (info.param == &baseline_vm)
        return "evmone_baseline";
    if (info.param == &external_vm)
        return "external_vm";
    return "unknown";
}
}  // namespace

INSTANTIATE_TEST_SUITE_P(
    multi_vm, evm, testing::ValuesIn(get_available_vms()), print_vm_name);

bool evm::is_advanced() noexcept
{
    return GetParam() == &advanced_vm;
}

bool evm::is_external() noexcept
{
    return GetParam() == &external_vm;
}

std::string evm::get_vm_name() const noexcept
{
    if (GetParam() == &advanced_vm)
        return "evmone_advanced";
    if (GetParam() == &baseline_vm)
        return "evmone_baseline";
    if (GetParam() == &external_vm)
        return "external_vm";
    return "unknown";
}
}  // namespace evmone::test

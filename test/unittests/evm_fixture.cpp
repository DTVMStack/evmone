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

std::unique_ptr<evmc::VM> external_vm;

bool try_load_external_vm(const std::string& path, const std::string& vm_name = "external")
{
    auto ec = evmc_loader_error_code{};
    auto vm = evmc::VM{evmc_load_and_configure(path.c_str(), &ec)};
    if (ec == EVMC_LOADER_SUCCESS) {
        external_vm = std::make_unique<evmc::VM>(std::move(vm));
        std::cout << "Successfully loaded external VM from: " << path << std::endl;
        return true;
    }
    std::cout << "Failed to load external VM from: " << path 
              << " (error: " << static_cast<int>(ec) << ")" << std::endl;
    return false;
}

void discover_and_load_external_vm() {
#ifdef HAVE_EXTERNAL_VM
    std::vector<std::string> search_paths = {
        "./libdtvmapi.so",
        "../libdtvmapi.so", 
        "/usr/local/lib/libdtvmapi.so",
        "/usr/lib/libdtvmapi.so"
    };

    for (const auto& path : search_paths) {
        if (std::filesystem::exists(path)) {
            if (try_load_external_vm(path)) {
                return;
            }
        }
    }

    std::cout << "External VM library not found in any search path" << std::endl;
#else
    std::cout << "External VM support not compiled in" << std::endl;
#endif
}

std::vector<evmc::VM*> get_available_vms() {
    static bool initialized = false;
    if (!initialized) {
        discover_and_load_external_vm();
        initialized = true;
    }

    std::vector<evmc::VM*> vms;
    vms.push_back(&advanced_vm);
    vms.push_back(&baseline_vm);

    if (external_vm) {
        vms.push_back(external_vm.get());
    }

    return vms;
}

const char* print_vm_name(const testing::TestParamInfo<evmc::VM*>& info) noexcept
{
    if (info.param == &advanced_vm)
        return "evmone_advanced";
    if (info.param == &baseline_vm)
        return "evmone_baseline";
    if (external_vm && info.param == external_vm.get())
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
    return external_vm && GetParam() == external_vm.get();
}

std::string evm::get_vm_name() const noexcept
{
    if (GetParam() == &advanced_vm)
        return "evmone_advanced";
    if (GetParam() == &baseline_vm)
        return "evmone_baseline";
    if (external_vm && GetParam() == external_vm.get())
        return "external_vm";
    return "unknown";
}
}  // namespace evmone::test

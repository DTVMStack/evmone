#!/bin/bash
cmake -S . -B build -DEVMONE_TESTING=ON
cmake --build build -j16

TEST_LIST_FILE=${1:-"./EVMOneUnitTestsRunList.txt"}
export EVMONE_EXTERNAL_OPTIONS=$2

FILTER_PARAM=""
while IFS= read -r line || [[ -n "$line" ]]; do
    line_clean=$(echo "$line" | xargs)
    if [ -n "$line_clean" ]; then
        if [ -z "$FILTER_PARAM" ]; then
            FILTER_PARAM="$line_clean"
        else
            FILTER_PARAM="$FILTER_PARAM:$line_clean"
        fi
    fi
done < "$TEST_LIST_FILE"

ASAN_LIB_PATH="${ASAN_LIB_PATH:-$(find /usr/lib -name 'libasan.so.*' 2>/dev/null | head -n1)}"
if [ -n "$ASAN_LIB_PATH" ]; then
    export LD_PRELOAD="$ASAN_LIB_PATH"
else
    echo "Warning: AddressSanitizer library not found" >&2
fi

./build/bin/evmone-unittests --gtest_filter="$FILTER_PARAM"

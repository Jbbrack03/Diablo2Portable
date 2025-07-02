#!/bin/bash

# Run all tests including MPQ integration tests with real game files

echo "Running all tests with MPQ integration enabled..."

# Set environment variables for MPQ tests
export ENABLE_MPQ_INTEGRATION_TESTS=1
export TEST_MPQ_PATH="/Users/jbbrack03/Diablo2Portable/vendor/mpq/d2data.mpq"
export D2_DATA_PATH="/Users/jbbrack03/Diablo2Portable/vendor/mpq/"

# Run the tests
./build/engine/tests/d2_unit_tests "$@"
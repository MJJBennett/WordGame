#!/bin/bash

# Get the script directory relative to the current directory
script_dir=$(dirname "$0")

# Move to that directory so we can reliably move to the correct build directory
cd $script_dir/..

# Make and move to the output directory, ignore errors
mkdir output >/dev/null 2>&1
cd output

cmake ..
# rm ../compile_commands.json
# ln -s compile_commands.json ../compile_commands.json
cp compile_commands.json ..
cd ..
echo "Fixing compile commands..."
./scripts/fix-compile-commands.py include/framework include/game src auth include/debug src/web include/application.hpp

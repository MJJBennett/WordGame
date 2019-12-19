#!/bin/bash

# Get the script directory relative to the current directory
script_dir=$(dirname "$0")

# Move to that directory so we can reliably move to the correct build directory
cd $script_dir/..

# Make and move to the output directory, ignore errors
mkdir output >/dev/null 2>&1
cd output

#export BOOST_ROOT=/home/michael/install/boost_1_71_0/boost/
#export BOOST_ROOT=/home/michael/install/boost_1_71_0/boost/
#export BOOST_ROOT=/home/michael/install/boost_1_71_0/boost/
cmake .. -DCMAKE_CXX_COMPILER=/usr/bin/g++-8 -DCMAKE_C_COMPILER=/usr/bin/gcc-8 -DBOOST_ROOT="/home/michael/install/boost_1_71_0" -DBOOST_INCLUDEDIR=/home/michael/install/boost_1_71_0 -DBoost_DEBUG=ON -DBOOST_LIBRARYDIR=/home/michael/install/boost_1_71_0/stage/lib/
# rm ../compile_commands.json
# ln -s compile_commands.json ../compile_commands.json
cp compile_commands.json ..
cd ..
echo "Fixing compile commands..."
./scripts/fix-compile-commands.py include/framework include/game src auth include/debug

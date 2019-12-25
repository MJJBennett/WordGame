#!/bin/bash

# Get the script directory relative to the current directory
script_dir=$(dirname "$0")

# Move to that directory so we can reliably move to the correct build directory
cd $script_dir/..

mkdir include/deps
mkdir resources/Roboto

# Note - this doesn't actually work for whatever reason
# The cURL might not be working properly, hard to say, or we need to follow redirects maybe
if [ ! -e "resources/Roboto/LICENSE.txt" ]; then
curl https://raw.githubusercontent.com/google/fonts/master/apache/robotomono/LICENSE.txt -o resources/Roboto/LICENSE.txt
fi
if [ ! -e "resources/Roboto/RobotoMono-Bold.ttf" ]; then
curl https://github.com/google/fonts/raw/master/apache/robotomono/RobotoMono-Bold.ttf -o resources/Roboto/RobotoMono-Bold.ttf
fi
if [ ! -e "resources/Roboto/Roboto-Black.ttf" ]; then
curl https://github.com/google/fonts/raw/master/apache/roboto/Roboto-Black.ttf -o resources/Roboto/Roboto-Black.ttf
fi

cp -a resources/Roboto/ output/

# Clone nlohmann/json into dependencies folder
git clone https://github.com/nlohmann/json.git include/deps/nlohmann_json

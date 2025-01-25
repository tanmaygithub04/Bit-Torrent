#!/bin/bash

# Compile all C++ files without OpenSSL and libcurl
g++ -std=c++20 *.cpp -o bittorrent -v

# Check if the compilation was successful
if [ $? -eq 0 ]; then
    echo "Build successful!"
    
    # Read user input for program arguments
    read args

    # Run the compiled program with the provided arguments
    ./bittorrent $args
else
    echo "Build failed!"
fi
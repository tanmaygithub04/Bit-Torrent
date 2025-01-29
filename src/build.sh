#!/bin/bash

# Path to Boost headers
BOOST_PATH="/opt/homebrew/include"

# Path to Boost libraries (if you need to link against them)
BOOST_LIB_PATH="/opt/homebrew/lib"

# Path to OpenSSL headers
OPENSSL_PATH="/opt/homebrew/opt/openssl/include"

# Path to OpenSSL libraries
OPENSSL_LIB_PATH="/opt/homebrew/opt/openssl/lib"

# Compile the C++ files, include Boost and OpenSSL headers, and link Boost and OpenSSL libraries
g++ -std=c++20 -I$BOOST_PATH -I$OPENSSL_PATH -L$BOOST_LIB_PATH -L$OPENSSL_LIB_PATH -lboost_system -lssl -lcrypto *.cpp -o bittorrent -v

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

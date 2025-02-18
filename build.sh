#!/bin/bash
BOOST_PATH="/opt/homebrew/include"
OPENSSL_PATH="/opt/homebrew/opt/openssl/include"
OPENSSL_LIB_PATH="/opt/homebrew/opt/openssl/lib"

g++ -std=c++20 \
    -I./src \
    -I./src/core \
    -I./src/utils \
    -I/opt/homebrew/include \
    -I/opt/homebrew/opt/openssl/include \
    -I/opt/homebrew/opt/boost/include \
    -L/opt/homebrew/opt/boost/lib \
    -L/opt/homebrew/opt/openssl/lib \
    -o bittorrent \
    src/main.cpp \
    src/core/*.cpp \
    src/utils/*.cpp \
    -lboost_system -lssl -lcrypto

if [ $? -eq 0 ]; then
    echo "Build successful!"

    # Read user input for program arguments
    read -p "Enter arguments: " args

    # Run the compiled program with the provided arguments
    ./bittorrent $args
else
    echo "Build failed!"
fi

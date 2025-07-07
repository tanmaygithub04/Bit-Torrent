#!/bin/bash

# BitTorrent Client - Simple Runner
# Two ways: Docker (instant) or Local Build (development)

set -e

# Colors
GREEN='\033[0;32m'
BLUE='\033[0;34m'
RED='\033[0;31m'
NC='\033[0m'

print_success() { echo -e "${GREEN}✓ $1${NC}"; }
print_info() { echo -e "${BLUE}→ $1${NC}"; }
print_error() { echo -e "${RED}✗ $1${NC}"; }

# Docker Demo - One command to run everything
docker_demo() {
    print_info "Starting BitTorrent Client Demo with Docker..."
    
    # Ensure directories exist
    mkdir -p torrents downloads
    
    # Copy sample torrent if needed
    if [ ! -f "torrents/sample.torrent" ]; then
        cp sample.torrent torrents/ 2>/dev/null || {
            print_error "sample.torrent not found in current directory"
            exit 1
        }
    fi
    
    # Build Docker image if it doesn't exist
    if ! docker image inspect bittorrent-client >/dev/null 2>&1; then
        print_info "Building Docker image..."
        docker build -t bittorrent-client .
        print_success "Docker image built"
    fi
    
    # Run the demo
    print_info "Downloading sample.torrent..."
    docker run -it --rm \
        -v "$(pwd)/torrents:/app/torrents" \
        -v "$(pwd)/downloads:/app/downloads" \
        bittorrent-client download_file /app/torrents/sample.torrent
    
    print_success "Demo completed! Check downloads/ directory"
}

# Local Build - For development
local_build() {
    print_info "Building BitTorrent Client locally..."
    
    # Check if dependencies exist
    if ! command -v g++ >/dev/null 2>&1; then
        print_error "g++ not found. Install build tools first:"
        echo "  Ubuntu/Debian: sudo apt-get install build-essential libboost-all-dev libssl-dev"
        echo "  macOS: brew install boost openssl"
        exit 1
    fi
    
    # Create build directory
    mkdir -p build
    
    # Simple build command
    print_info "Compiling..."
    if g++ -std=c++20 \
        -I./src -I./src/core -I./src/utils -I./src/lib \
        -o build/bittorrent \
        src/Main.cpp src/core/*.cpp src/utils/*.cpp \
        -lboost_system -lssl -lcrypto -lpthread \
        -O2 2>/dev/null; then
        print_success "Build successful! Binary: build/bittorrent"
        
        # Test run
        print_info "Testing with sample torrent..."
        ./build/bittorrent download_file sample.torrent
        print_success "Local build completed!"
    else
        print_error "Build failed. Try the advanced build script: ./build.sh"
        exit 1
    fi
}

# Show usage
show_usage() {
    echo "BitTorrent Client - Simple Runner"
    echo ""
    echo "Usage: $0 <command>"
    echo ""
    echo "Commands:"
    echo "  docker    Run demo with Docker (recommended - works anywhere)"
    echo "  build     Build and run locally (for development)"
    echo ""
    echo "Examples:"
    echo "  $0 docker    # 🚀 Instant demo"
    echo "  $0 build     # 🔧 Local development"
}

# Main command handling
case "$1" in
    "docker")
        docker_demo
        ;;
    "build")
        local_build
        ;;
    *)
        show_usage
        exit 1
        ;;
esac 
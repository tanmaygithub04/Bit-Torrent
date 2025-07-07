#!/bin/bash

# BitTorrent Client - Quick Start Script
# Easy commands for building, running, and testing

set -e

# Colors for output
RED='\033[0;31m'
GREEN='\033[0;32m'
BLUE='\033[0;34m'
YELLOW='\033[1;33m'
NC='\033[0m' # No Color

# Helper functions
print_header() {
    echo -e "${BLUE}===========================================${NC}"
    echo -e "${BLUE}    BitTorrent Client - Quick Start${NC}"
    echo -e "${BLUE}===========================================${NC}"
}

print_success() {
    echo -e "${GREEN}✓ $1${NC}"
}

print_error() {
    echo -e "${RED}✗ $1${NC}"
}

print_info() {
    echo -e "${YELLOW}→ $1${NC}"
}

# Build function
build() {
    print_info "Building BitTorrent client..."
    
    if [ -f "build/bittorrent" ]; then
        print_success "Already built! Use 'rebuild' to force rebuild."
        return 0
    fi
    
    mkdir -p build
    
    if g++ -std=c++20 \
        -I./src -I./src/core -I./src/utils -I./src/lib \
        -o build/bittorrent \
        src/Main.cpp src/core/*.cpp src/utils/*.cpp \
        -lboost_system -lssl -lcrypto -lpthread \
        -O2 >/dev/null 2>&1; then
        print_success "Build successful!"
    else
        print_error "Build failed. Run './build.sh' for detailed build with dependency installation."
        exit 1
    fi
}

# Force rebuild
rebuild() {
    print_info "Force rebuilding..."
    rm -f build/bittorrent
    build
}

# Run function
run() {
    if [ ! -f "build/bittorrent" ]; then
        print_info "Binary not found. Building first..."
        build
    fi
    
    if [ "$1" ]; then
        print_info "Running: $*"
        ./build/bittorrent "$@"
    else
        print_info "Showing help:"
        ./build/bittorrent --help
    fi
}

# Docker build function
docker_build() {
    print_info "Building Docker image..."
    
    if docker build -t bittorrent-client . >/dev/null 2>&1; then
        print_success "Docker image built successfully!"
    else
        print_error "Docker build failed. Make sure Docker is installed and running."
        exit 1
    fi
}

# Docker run function
docker_run() {
    print_info "Running in Docker container..."
    
    if [ "$1" ]; then
        docker run -it --rm \
            -v "$(pwd)/torrents:/app/torrents" \
            -v "$(pwd)/downloads:/app/downloads" \
            bittorrent-client "$@"
    else
        docker run -it --rm bittorrent-client --help
    fi
}

# Download function (local)
download() {
    if [ -z "$1" ]; then
        print_error "Usage: $0 download <torrent-file>"
        echo "  Example: $0 download sample.torrent"
        exit 1
    fi
    
    if [ ! -f "$1" ]; then
        print_error "Torrent file not found: $1"
        exit 1
    fi
    
    print_info "Downloading $1..."
    run download_file "$1"
}

# Docker download function
docker_download() {
    if [ -z "$1" ]; then
        print_error "Usage: $0 docker-download <torrent-file>"
        echo "  Example: $0 docker-download sample.torrent"
        echo "  Note: Put your torrent file in the 'torrents/' directory"
        exit 1
    fi
    
    if [ ! -f "torrents/$1" ]; then
        print_error "Torrent file not found: torrents/$1"
        print_info "Make sure to put your torrent file in the 'torrents/' directory"
        exit 1
    fi
    
    print_info "Downloading $1 with Docker..."
    docker_run download_file "/app/torrents/$1"
}

# Show usage
show_usage() {
    print_header
    echo
    echo "Usage: $0 <command> [options]"
    echo
    echo "Commands:"
    echo "  build                    Build the BitTorrent client"
    echo "  rebuild                  Force rebuild (clean build)"
    echo "  run [args]               Run the client with optional arguments"
    echo "  download <file>          Download a torrent file (local)"
    echo "  docker-build             Build Docker image"
    echo "  docker-run [args]        Run client in Docker container"
    echo "  docker-download <file>   Download torrent with Docker"
    echo
    echo "Examples:"
    echo "  $0 build"
    echo "  $0 download sample.torrent"
    echo "  $0 docker-download sample.torrent"
    echo "  $0 run --help"
    echo
    echo "Quick Demo (try this first!):"
    echo "  $0 docker-download sample.torrent"
    echo
    echo "For advanced building with auto-dependencies: ./build.sh"
    echo
}

# Main command handling
case "$1" in
    "build")
        build
        ;;
    "rebuild")
        rebuild
        ;;
    "run")
        shift
        run "$@"
        ;;
    "download")
        download "$2"
        ;;
    "docker-build")
        docker_build
        ;;
    "docker-run")
        shift
        docker_run "$@"
        ;;
    "docker-download")
        docker_download "$2"
        ;;
    "help" | "--help" | "-h")
        show_usage
        ;;
    "")
        show_usage
        ;;
    *)
        print_error "Unknown command: $1"
        echo
        show_usage
        exit 1
        ;;
esac 
#!/bin/bash

# BitTorrent Build Script with Automatic Dependency Management
# Supports macOS, Linux, and Windows (WSL/MSYS2)

set -e  # Exit on any error

# Colors for output
RED='\033[0;31m'
GREEN='\033[0;32m'
YELLOW='\033[1;33m'
BLUE='\033[0;34m'
NC='\033[0m' # No Color

# Function to print colored output
print_info() {
    echo -e "${BLUE}[INFO]${NC} $1"
}

print_success() {
    echo -e "${GREEN}[SUCCESS]${NC} $1"
}

print_warning() {
    echo -e "${YELLOW}[WARNING]${NC} $1"
}

print_error() {
    echo -e "${RED}[ERROR]${NC} $1"
}

# Detect OS
detect_os() {
    if [[ "$OSTYPE" == "linux-gnu"* ]]; then
        echo "linux"
    elif [[ "$OSTYPE" == "darwin"* ]]; then
        echo "macos"
    elif [[ "$OSTYPE" == "msys" ]] || [[ "$OSTYPE" == "win32" ]]; then
        echo "windows"
    else
        echo "unknown"
    fi
}

# Check if command exists
command_exists() {
    command -v "$1" >/dev/null 2>&1
}

# Install dependencies for Linux (Ubuntu/Debian)
install_linux_deps() {
    print_info "Installing dependencies for Linux..."
    
    # Update package manager
    if command_exists apt-get; then
        sudo apt-get update
        sudo apt-get install -y \
            build-essential \
            cmake \
            libboost-all-dev \
            libssl-dev \
            pkg-config
    elif command_exists yum; then
        sudo yum install -y \
            gcc-c++ \
            cmake \
            boost-devel \
            openssl-devel \
            pkgconfig
    elif command_exists pacman; then
        sudo pacman -S --noconfirm \
            base-devel \
            cmake \
            boost \
            openssl \
            pkgconfig
    else
        print_error "Unsupported Linux distribution. Please install dependencies manually:"
        print_error "- build-essential/gcc-c++/base-devel"
        print_error "- cmake"
        print_error "- boost development libraries"
        print_error "- openssl development libraries"
        exit 1
    fi
}

# Install dependencies for macOS
install_macos_deps() {
    print_info "Installing dependencies for macOS..."
    
    if ! command_exists brew; then
        print_error "Homebrew not found. Please install Homebrew first:"
        print_error "Visit: https://brew.sh/"
        exit 1
    fi
    
    # Install dependencies
    brew install boost openssl cmake
    
    # Set environment variables for OpenSSL
    export OPENSSL_ROOT_DIR=$(brew --prefix openssl)
    export OPENSSL_LIBRARIES=$OPENSSL_ROOT_DIR/lib
    export OPENSSL_INCLUDE_DIR=$OPENSSL_ROOT_DIR/include
}

# Install dependencies for Windows (WSL/MSYS2)
install_windows_deps() {
    print_info "Installing dependencies for Windows..."
    
    if command_exists pacman; then
        # MSYS2
        pacman -S --noconfirm \
            mingw-w64-x86_64-gcc \
            mingw-w64-x86_64-cmake \
            mingw-w64-x86_64-boost \
            mingw-w64-x86_64-openssl \
            mingw-w64-x86_64-pkg-config
    elif command_exists apt-get; then
        # WSL
        install_linux_deps
    else
        print_error "Unsupported Windows environment. Please use WSL or MSYS2."
        exit 1
    fi
}

# Check and install dependencies
install_dependencies() {
    local os=$(detect_os)
    
    print_info "Detected OS: $os"
    
    # Check if required tools are already installed
    if command_exists g++ && command_exists cmake; then
        print_info "Basic build tools found. Checking for Boost..."
        
        # Simple check for Boost
        if [ -f "/usr/include/boost/version.hpp" ] || [ -f "/usr/local/include/boost/version.hpp" ] || [ -f "/opt/homebrew/include/boost/version.hpp" ]; then
            print_info "Boost libraries found. Skipping dependency installation."
            return 0
        fi
    fi
    
    print_info "Installing dependencies..."
    
    case $os in
        "linux")
            install_linux_deps
            ;;
        "macos")
            install_macos_deps
            ;;
        "windows")
            install_windows_deps
            ;;
        *)
            print_error "Unsupported operating system: $os"
            exit 1
            ;;
    esac
}

# Build the project
build_project() {
    print_info "Building BitTorrent client..."
    
    # Find the correct main file
    MAIN_FILE=""
    if [ -f "src/main.cpp" ]; then
        MAIN_FILE="src/main.cpp"
    elif [ -f "src/Main.cpp" ]; then
        MAIN_FILE="src/Main.cpp"
    else
        print_error "Neither src/main.cpp nor src/Main.cpp found!"
        exit 1
    fi
    
    print_info "Using main file: $MAIN_FILE"
    
    # Create build directory if it doesn't exist
    mkdir -p build
    
    # Determine compiler flags based on OS
    local os=$(detect_os)
    INCLUDE_DIRS="-I./src -I./src/core -I./src/utils -I./src/lib"
    LIBRARY_DIRS=""
    LIBRARIES="-lboost_system -lssl -lcrypto -lpthread"
    
    case $os in
        "macos")
            if command_exists brew; then
                HOMEBREW_PREFIX=$(brew --prefix)
                INCLUDE_DIRS="$INCLUDE_DIRS -I$HOMEBREW_PREFIX/include"
                LIBRARY_DIRS="$LIBRARY_DIRS -L$HOMEBREW_PREFIX/lib"
                
                # Add OpenSSL paths
                OPENSSL_PREFIX=$(brew --prefix openssl)
                INCLUDE_DIRS="$INCLUDE_DIRS -I$OPENSSL_PREFIX/include"
                LIBRARY_DIRS="$LIBRARY_DIRS -L$OPENSSL_PREFIX/lib"
            fi
            ;;
        "linux")
            INCLUDE_DIRS="$INCLUDE_DIRS -I/usr/include -I/usr/local/include"
            LIBRARY_DIRS="$LIBRARY_DIRS -L/usr/lib -L/usr/local/lib"
            ;;
        "windows")
            if command_exists pacman; then
                # MSYS2
                INCLUDE_DIRS="$INCLUDE_DIRS -I/mingw64/include"
                LIBRARY_DIRS="$LIBRARY_DIRS -L/mingw64/lib"
            fi
            ;;
    esac
    
    # Compile
    print_info "Compiling with g++..."
g++ -std=c++20 \
        $INCLUDE_DIRS \
        $LIBRARY_DIRS \
        -o build/bittorrent \
        $MAIN_FILE \
    src/core/*.cpp \
    src/utils/*.cpp \
        $LIBRARIES \
        -O2 -DNDEBUG

if [ $? -eq 0 ]; then
        print_success "Build successful! Executable created at build/bittorrent"

        # Make executable
        chmod +x build/bittorrent
        
        return 0
    else
        print_error "Build failed!"
        return 1
    fi
}

# Run the program
run_program() {
    if [ -f "build/bittorrent" ]; then
        print_info "Running BitTorrent client..."
        echo
        read -p "Enter arguments (or press Enter for help): " args
        echo
        
        if [ -z "$args" ]; then
            args="--help"
        fi
        
        ./build/bittorrent $args
    else
        print_error "Executable not found. Build may have failed."
        exit 1
    fi
}

# Main execution
main() {
    print_info "BitTorrent Build Script"
    print_info "======================="
    
    # Check if we're in the right directory
    if [ ! -f "src/core/torrent.h" ]; then
        print_error "Please run this script from the project root directory"
        exit 1
    fi
    
    # Install dependencies
    install_dependencies
    
    # Build the project
    if build_project; then
        print_success "Build completed successfully!"
        
        # Ask if user wants to run the program
        echo
        read -p "Do you want to run the program now? (y/N): " -n 1 -r
        echo
        
        if [[ $REPLY =~ ^[Yy]$ ]]; then
            run_program
        else
            print_info "You can run the program later with: ./build/bittorrent [args]"
        fi
    else
        print_error "Build failed. Please check the errors above."
        exit 1
    fi
}

# Run main function
main "$@"

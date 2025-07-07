# GitHub Codespaces Setup

This repository is configured for GitHub Codespaces, allowing you to run the BitTorrent client directly in your browser.

## Quick Start in Codespaces

1. Click the "Open in GitHub Codespaces" badge in the main README
2. Wait for the environment to set up (auto-installs dependencies)
3. Run the demo:
   ```bash
   ./quick-start.sh docker-download sample.torrent
   ```

## What's Included

- **Pre-configured C++ environment** with all dependencies
- **Docker support** for containerized builds
- **VS Code extensions** for C++ development
- **Port forwarding** for BitTorrent protocol (6881)

## Available Commands

```bash
# Quick demo
./quick-start.sh docker-download sample.torrent

# Local build and run
./quick-start.sh build
./quick-start.sh download sample.torrent

# Advanced build with dependency management
./build.sh
```

The environment automatically installs:
- GCC/G++ compiler
- CMake
- Boost libraries
- OpenSSL
- Docker

Perfect for testing, development, and demonstrations! 
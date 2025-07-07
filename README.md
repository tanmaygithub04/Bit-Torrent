# BitTorrent Client in C++

[![Build Status](https://github.com/tanmaygithub04/Bit-Torrent/workflows/Build%20BitTorrent%20Client/badge.svg)](https://github.com/tanmaygithub04/Bit-Torrent/actions)
[![Platform](https://img.shields.io/badge/platform-linux%20%7C%20macos%20%7C%20windows-lightgrey)](https://github.com/tanmaygithub04/Bit-Torrent)
[![Docker](https://img.shields.io/badge/docker-supported-blue)](https://hub.docker.com/)

A fully functional BitTorrent client implementation in C++ that demonstrates understanding of peer-to-peer networking, protocol implementation, and system-level programming. Features a modular architecture with comprehensive error handling and cross-platform support.

## 🚀 Quick Demo

### **🌐 Try in Browser (Zero Setup)**
[![Open in GitHub Codespaces](https://github.com/codespaces/badge.svg)](https://codespaces.new/tanmaygithub04/Bit-Torrent)
[![Open in Gitpod](https://gitpod.io/button/open-in-gitpod.svg)](https://gitpod.io/#https://github.com/tanmaygithub04/Bit-Torrent)

**Click above → VS Code opens in browser → Run:**
```bash
./quick-start.sh docker-download sample.torrent
```

### **💻 Try Locally (Docker Required)**
```bash
git clone https://github.com/tanmaygithub04/Bit-Torrent.git
cd Bit-Torrent
./quick-start.sh docker-download sample.torrent
```

**Watch it download a file with real-time progress, peer connections, and piece verification!**

## 📋 Key Features

- **Complete BitTorrent Protocol Implementation**: Handshake, peer communication, piece downloading
- **Block-based Piece Management**: Downloads in 16KB blocks with SHA-1 verification
- **Multi-peer Connection Handling**: Connects to multiple peers simultaneously
- **Tracker Communication**: Full HTTP tracker protocol support
- **Bencode Parser**: Custom implementation for torrent metadata parsing
- **Cross-platform Build System**: Linux, macOS, Windows support
- **Professional Terminal UI**: Progress bars, colored output, real-time status
- **Docker Containerization**: Easy deployment and demonstration

## 🔧 Architecture

```
┌─────────────────────────────────────────────────────────────┐
│                        Main Application                     │
└─────────────────────────┬───────────────────────────────────┘
                          │
┌─────────────────────────▼───────────────────────────────────┐
│                   Download Manager                          │
│  • Orchestrates download process                           │
│  • Manages piece verification                              │
│  • Handles file assembly                                   │
└─────────────────────────┬───────────────────────────────────┘
                          │
         ┌────────────────┼────────────────┐
         │                │                │
┌────────▼────────┐  ┌────▼────────┐  ┌───▼──────────┐
│  Torrent Parser │  │   Tracker   │  │  Peer Manager│
│  • Metadata     │  │  • Get peers│  │  • Handshake │
│  • Bencode      │  │  • Announce │  │  • Messages  │
│  • Info hash    │  │  • HTTP     │  │  • Downloads │
└─────────────────┘  └─────────────┘  └──────────────┘
```

## 🚀 Getting Started

### Option 1: Browser Demo (Recommended for Recruiters)

**Zero setup required - runs in browser:**
- Click the GitHub Codespaces or Gitpod badge above
- Pre-configured environment with all dependencies
- VS Code interface in browser
- Perfect for interviews and demonstrations

### Option 2: Docker (Local Demo)

```bash
# Clone and run instantly
git clone https://github.com/tanmaygithub04/Bit-Torrent.git
cd Bit-Torrent

# Download sample file (shows full capability)
./quick-start.sh docker-download sample.torrent

# Or with your own torrent file
cp your-file.torrent torrents/
./quick-start.sh docker-download your-file.torrent
```

### Option 3: Local Build

For development and customization:

```bash
# Clone repository
git clone https://github.com/tanmaygithub04/Bit-Torrent.git
cd Bit-Torrent

# Auto-install dependencies and build
./build.sh

# Run with sample file
./build/bittorrent download_file sample.torrent

# Or use the quick-start script
./quick-start.sh download sample.torrent
```

## 📦 Dependencies

**Auto-installed by build script:**
- C++20 compiler (GCC 9+/Clang 10+)
- Boost libraries (system, beast)
- OpenSSL
- CMake

**Supported platforms:** Linux, macOS, Windows (WSL2)

## 🎯 Usage Examples

### Basic Usage
```bash
# Download a torrent file
./build/bittorrent download_file sample.torrent

# Show help and available commands
./build/bittorrent --help
```

### Docker Usage
```bash
# Build Docker image
docker build -t bittorrent-client .

# Run with volume mounts
docker run -it --rm \
  -v $(pwd)/downloads:/app/downloads \
  -v $(pwd)/torrents:/app/torrents \
  bittorrent-client download_file /app/torrents/sample.torrent
```

### Quick Start Script
```bash
# Local build and run
./quick-start.sh download sample.torrent

# Docker build and run
./quick-start.sh docker-download sample.torrent

# See all available commands
./quick-start.sh
```



### Protocol Features

- **Handshake Protocol**: 68-byte handshake with protocol identification
- **Message Framing**: 4-byte length prefix + message ID + payload
- **Piece Verification**: SHA-1 hash verification for each piece
- **Peer Selection**: Intelligent peer selection for piece downloads
- **Error Handling**: Comprehensive error handling and recovery

## 📊 Performance

- **Concurrent Downloads**: Multiple peer connections
- **Efficient Memory Usage**: Streaming piece assembly
- **Network Optimization**: Optimal block size (16KB)
- **Cross-platform**: Consistent performance across platforms

## 🔍 File Structure

```
src/
├── core/                 # Core BitTorrent implementation
│   ├── DownloadManager.* # Download orchestration
│   ├── peer.*           # Peer communication
│   ├── torrent.*        # Torrent metadata parsing
│   └── tracker.*        # Tracker communication
├── utils/               # Utility functions
│   ├── bencode.*        # Bencode encoding/decoding
│   ├── hash.*           # Cryptographic functions
│   └── terminal_ui.h    # UI utilities
└── Main.cpp            # Application entry point
```


## 🚀 Deployment

### Docker Hub
```bash
docker pull your-username/bittorrent-client
docker run -it --rm \
  -v $(pwd)/downloads:/app/downloads \
  -v $(pwd)/torrents:/app/torrents \
  your-username/bittorrent-client
```


## 💡 Technical Highlights

- **Custom Bencode Parser**: Implemented from scratch for torrent metadata
- **Network Programming**: Boost.Asio for async networking
- **Protocol Implementation**: Full BitTorrent protocol compliance
- **Memory Management**: Efficient piece storage and assembly
- **Error Handling**: Robust error handling and recovery mechanisms
- **Cross-platform**: Automated dependency management and building



## Features Implemented ✅

1. **Block-Based Piece Downloading:**  
   Downloads pieces in 16 KiB blocks by sending REQUEST messages and assembling the corresponding PIECE messages.

2. **Piece Integrity Verification:**  
   Computes the SHA-1 hash of each downloaded piece and compares it against the expected hash from the torrent metadata to ensure file integrity.

3. **File Assembly:**  
   Once all pieces are successfully downloaded and verified, the client assembles them into the final output file.

4. **Peer Connection & Handshake:**  
   Uses Boost.Asio to resolve peer addresses, establish TCP connections, and perform the BitTorrent handshake.

5. **Low-Level Message I/O:**  
   The Peer class handles sending and receiving protocol messages with proper framing (4-byte length prefix, message ID, and payload).

6. **Torrent Metadata Parsing:**  
   Extracts essential information (info_hash, piece length, total file length, concatenated SHA-1 hashes) from torrent files.

7. **Bencode Decoding:**  
   Implements bencode parsing utilities to correctly read and interpret torrent metadata.

8. **Modular Architecture:**  
   Refactored from a monolithic implementation into separate, maintainable components (TorrentMetadata, Peer, and DownloadManager) to improve clarity and scalability.



## Assumptions 📌

- This is a simple Bit torrent client without Piece Selection , or Tracker Implementation , or any Choking Algorithm so might not work for all torrent files
- The client assumes the `.torrent` file is valid and well-formed.
- Peers listed in the torrent metadata are available and active.
- The download directory (`src/downloads/`) exists before running the program.

## Future Plans

- [ ] Make it compatibility with standard multi file torrents 
- [ ] Can go in direction of video downloading/streaming or in the direction of implementing advanced algorithms 
- [ ] Multithreaded piece downloading
- [ ] DHT (Distributed Hash Table) support
- [ ] Magnet link support
- [ ] Web UI interface
- [ ] Bandwidth throttling



## Problems Faced 🛠

1. **Monolithic Code Structure**
   - Initially, all functionality was implemented in a single file, making the code hard to maintain. This necessitated refactoring into modular components.

2. **Linker and Undefined Symbols**
   - Some functions were declared but not defined or had signature mismatches, leading to linker errors.

3. **URL Encoding and Conversion Errors**
   - Improper encoding of binary data (like the info hash) resulted in conversion errors such as “stoul: no conversion.”

4. **UTF‑8 Validation Issues**
   - The tracker’s compact peer data wasn’t valid UTF‑8, which broke JSON parsing until UTF‑8 validation was disabled.

5. **File Path and Directory Problems**
   - Incorrect relative paths (e.g., using "../downloads/sample.txt" instead of the correct path) and missing directories led to file-opening errors.

6. **Piece Assembly Errors**
   - Using vector insertion to assemble piece data appended blocks rather than placing them at the correct offsets, causing hash mismatches.

7. **State Management for Peer Communication**
   - Waiting for an UNCHOKE message on every piece download—even when the peer was already unchoked—resulted in failed downloads.

8. **Segmentation Faults & Memory Issues**
   - Mismanagement of memory and null pointer dereferences caused segmentation faults during handshake and piece download.

 And Many more 😅



## Contributing ✨

If you want to contribute:

- Fork the repo
- Create a feature branch
- Make changes and submit a PR 🚀



## 📄 License

This project is licensed under the MIT License - see the LICENSE file for details.

---

<div align="center">

**[Demo](https://github.com/tanmaygithub04/Bit-Torrent)** • **[Documentation](https://github.com/tanmaygithub04/Bit-Torrent/wiki)** • **[Issues](https://github.com/tanmaygithub04/Bit-Torrent/issues)**

*Built with C++20 • Boost.Asio • OpenSSL • Docker*

</div>


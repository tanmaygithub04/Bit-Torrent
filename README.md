
# BitTorrent Client
A complete BitTorrent client implementation in C++20 with multi-peer downloading, SHA-1 verification, and cross-platform support.

## 🌐 Try in Browser (Zero Setup)

[![Open in Gitpod](https://gitpod.io/button/open-in-gitpod.svg)](https://gitpod.io/#https://github.com/tanmaygithub04/Bit-Torrent)
[![Open in GitHub Codespaces](https://github.com/codespaces/badge.svg)](https://codespaces.new/tanmaygithub04/Bit-Torrent)

*Click above → VS Code opens in browser → Run: `./run.sh docker`*

## 🚀 Quick Start

**Two ways to run:**
1. **Docker** (instant, works anywhere)
2. **Local Build** (for development)

---

## 🐳 Option 1: Docker (Recommended)

**Works on any system with Docker installed.**

### Step 1: Clone the repository
```bash
git clone https://github.com/tanmaygithub04/Bit-Torrent.git
cd Bit-Torrent
```

### Step 2: Run the demo
```bash
./run.sh docker
```

**That's it!** The script will:
- Build the Docker image automatically
- Download a sample torrent file
- Show real-time progress
- Save the file to `downloads/` directory

---

## 🔧 Option 2: Local Build

**For development and customization.**

### Step 1: Install dependencies

**Ubuntu/Debian:**
```bash
sudo apt-get update
sudo apt-get install build-essential libboost-all-dev libssl-dev cmake
```

**macOS:**
```bash
brew install boost openssl cmake
```

### Step 2: Clone and build
```bash
git clone https://github.com/tanmaygithub04/Bit-Torrent.git
cd Bit-Torrent
./run.sh build
```

The script will compile and test automatically.

---

## 📁 What You Get

```
Bit-Torrent/
├── src/
│   ├── core/
│   │   ├── DownloadManager.cpp     # Orchestrates downloads
│   │   ├── DownloadManager.h
│   │   ├── peer.cpp                # Peer communication
│   │   ├── peer.h
│   │   ├── torrent.cpp             # Torrent metadata parsing
│   │   ├── torrent.h
│   │   ├── tracker.cpp             # Tracker communication
│   │   └── tracker.h
│   ├── utils/
│   │   ├── bencode.cpp             # Bencode parsing
│   │   ├── bencode.h
│   │   ├── error.h                 # Error handling
│   │   ├── hash.cpp                # SHA-1 hashing
│   │   ├── hash.h
│   │   └── terminal_ui.h           # UI utilities
│   └── Main.cpp                    # Entry point
├── torrents/
│   └── sample.torrent              # Sample torrent file
├── downloads/                      # Downloaded files appear here
├── README.md                       # Project documentation
├── run.sh                          # Simple runner script
```


## 🏗️ Architecture
```
┌─────────────────────────────────────────────────────────────┐
│                        Main Application                     │
└─────────────────────────┬───────────────────────────────────┘
                          │
┌─────────────────────────▼───────────────────────────────────┐
│                   Download Manager                          │
│  • Orchestrates download process                            │
│  • Manages piece verification                               │
│  • Handles file assembly                                    │
└─────────────────────────┬───────────────────────────────────┘
                          │
         ┌────────────────┼────────────────┐
         │                │                │
┌────────▼────────┐  ┌────▼────────┐  ┌───-▼─────────┐
│  Torrent Parser │  │   Tracker   │  │  Peer Manager│
│  • Metadata     │  │  • Get peers│  │  • Handshake │
│  • Bencode      │  │  • Announce │  │  • Messages  │
│  • Info hash    │  │  • HTTP     │  │  • Downloads │
└─────────────────┘  └─────────────┘  └──────────────┘
```



## Features Implemented 

1. Block-Based Piece Downloading✅:
Downloads pieces in 16 KiB blocks by sending REQUEST messages and assembling the corresponding PIECE messages.

2. Piece Integrity Verification✅:
Computes the SHA-1 hash of each downloaded piece and compares it against the expected hash from the torrent metadata to ensure file integrity.

3. File Assembly✅:
Once all pieces are successfully downloaded and verified, the client assembles them into the final output file.

4. Peer Connection & Handshake✅:
Uses Boost.Asio to resolve peer addresses, establish TCP connections, and perform the BitTorrent handshake.

5. Torrent Metadata Parsing✅:
Extracts essential information (info_hash, piece length, total file length, concatenated SHA-1 hashes) from torrent files.

6. Bencode Decoding✅:
Implements bencode parsing utilities to correctly read and interpret torrent metadata.

7. Modular Architecture✅:
Refactored from a monolithic implementation into separate, maintainable components (TorrentMetadata, Peer, and DownloadManager) to improve clarity and scalability.

Assumptions 📌
This is a simple Bit torrent client without Piece Selection , or Tracker Implementation , or any Choking Algorithm so might not work for all torrent files
The client assumes the .torrent file is valid and well-formed.
Peers listed in the torrent metadata are available and active.
The download directory (src/downloads/) exists before running the program.


## Future Plans
- [ ] Make it compatibility with standard multi file torrents 
- [ ] Can go in direction of video downloading/streaming or in the direction of implementing advanced algorithms 
- [ ] Multithreaded piece downloading
- [ ] DHT (Distributed Hash Table) support
- [ ] Magnet link support
- [ ] Web UI interface
- [ ] Bandwidth throttling


## Problems Faced 🛠
- Monolithic Code Structure
Initially, all functionality was implemented in a single file, making the code hard to maintain. This necessitated refactoring into modular components.
Linker and Undefined Symbols

- Some functions were declared but not defined or had signature mismatches, leading to linker errors.
URL Encoding and Conversion Errors

- Improper encoding of binary data (like the info hash) resulted in conversion errors such as “stoul: no conversion.”
UTF‑8 Validation Issues

- The tracker’s compact peer data wasn’t valid UTF‑8, which broke JSON parsing until UTF‑8 validation was disabled.
File Path and Directory Problems

- Incorrect relative paths (e.g., using "../downloads/sample.txt" instead of the correct path) and missing directories led to file-opening errors.
Piece Assembly Errors

- Using vector insertion to assemble piece data appended blocks rather than placing them at the correct offsets, causing hash mismatches.
State Management for Peer Communication

- Waiting for an UNCHOKE message on every piece download—even when the peer was already unchoked—resulted in failed downloads.
Segmentation Faults & Memory Issues

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


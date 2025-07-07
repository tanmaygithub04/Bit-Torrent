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
├── torrents/          # Put your .torrent files here
├── downloads/         # Downloaded files appear here
├── run.sh            # Simple runner script
└── src/              # Source code
    ├── Main.cpp      # Entry point
    ├── core/         # BitTorrent protocol implementation
    └── utils/        # Utilities (bencode, hashing, etc.)
```

## 🎯 Using Your Own Torrents

1. **Add your .torrent file:**
   ```bash
   cp your-file.torrent torrents/
   ```

2. **Run with Docker:**
   ```bash
   docker run -it --rm \
     -v $(pwd)/torrents:/app/torrents \
     -v $(pwd)/downloads:/app/downloads \
     bittorrent-client download_file /app/torrents/your-file.torrent
   ```

3. **Run locally:**
   ```bash
   ./build/bittorrent download_file torrents/your-file.torrent
   ```

## 🏗️ Architecture

- **TorrentMetadata**: Parses .torrent files using custom bencode decoder
- **DownloadManager**: Orchestrates downloads, manages pieces, verifies hashes
- **Peer**: Handles BitTorrent protocol, peer connections, data transfer
- **Tracker**: Communicates with trackers to get peer lists

## ⚡ Features

- ✅ Complete BitTorrent protocol implementation
- ✅ Multi-peer concurrent downloading
- ✅ SHA-1 piece verification
- ✅ Custom bencode parser
- ✅ Cross-platform support (Linux, macOS, Windows)
- ✅ Docker containerization
- ✅ Real-time download progress

## 🛠️ Technical Details

- **Language**: C++20
- **Networking**: Boost.Asio
- **Crypto**: OpenSSL (SHA-1 hashing)
- **Build**: CMake/g++
- **Container**: Docker

## 📊 Performance

- Concurrent piece downloading from multiple peers
- 16KB block size for optimal network usage
- Efficient memory management with streaming verification
- Cross-platform networking with Boost.Asio

## 🤝 Contributing

1. Fork the repository
2. Create a feature branch
3. Make your changes
4. Test with both Docker and local build
5. Submit a pull request

## 📄 License

MIT License - see LICENSE file for details.

---

**Built with C++20 • Boost.Asio • OpenSSL • Docker**


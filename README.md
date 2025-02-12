# BitTorrent Client (BIT V2)

## Overview

This is a BitTorrent client implementation in C++. The project is a work-in-progress, aimed at understanding the BitTorrent protocol and building a functional client that can connect to peers, download files, and handle torrent metadata.

## File Structure

```
BIT V2/
│── src/
│   ├── core/               # Core functionality 
│   ├── downloads/          # Stores downloaded files
│   │   ├── sample.txt      # Test file - downloaded 
│   ├── lib/                # External libraries
│   ├── utils/              # Utility functions
│   │   ├── bencode.cpp     # Bencode parsing
│   │   ├── bencode.h       
│   │   ├── error.h         
│   │   ├── hash.cpp        # Hashing functions
│   │   ├── hash.h          
│   │   ├── main.cpp        # Entry point of the project
│── .gitignore              # Ignoring unnecessary files
│── build.sh                # Build script
│── sample.torrent          # Example torrent file
```

## How to Run

1. **Clone the repo:**
   ```sh
   git clone <https://github.com/tanmaygithub04/Bit-Torrent>
   cd BIT-V2
   ```
2. **Build the project:**
   ```sh
   ./build.sh
   ```
3. **Run the client:**
   ```sh
   ./bittorrent sample.torrent
   ```
   *(Make sure **`sample.torrent`** exists in the project root!)*

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

- This is a simple Bit torrent client without Piece Selection , Or Tracker Implementation , Or any Choking Algorithm so might not work for all torrent files
- The client assumes the `.torrent` file is valid and well-formed.
- Peers listed in the torrent metadata are available and active.
- The download directory (`src/downloads/`) exists before running the program.

## Future Plans

- Add a frontend to this 
- Make it compatible with standard torrents 
- Can go in direction of video downloading/streaming or in the direction of implementing advanced algorithms 

## Contributing ✨

If you want to contribute:

- Fork the repo
- Create a feature branch
- Make changes and submit a PR 🚀


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


## License

This project is open-source. Feel free to use and modify it as needed!


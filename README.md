# BitTorrent Client (BIT V2)

## Overview

This is a BitTorrent client implementation in C++. The project is a work-in-progress, aimed at understanding the BitTorrent protocol and building a functional client that can connect to peers, download files, and handle torrent metadata.

## File Structure

```
BIT V2/
│── src/
│   ├── core/               # Core functionality (yet to be implemented)
│   ├── downloads/          # Stores downloaded files
│   │   ├── output.dat      # Example downloaded file
│   │   ├── sample.txt      # Test file
│   ├── lib/                # External libraries (if needed)
│   ├── utils/              # Utility functions
│   │   ├── bencode.cpp     # Bencode parsing
│   │   ├── bencode.h       
│   │   ├── error.h         
│   │   ├── hash.cpp        # Hashing functions
│   │   ├── hash.h          
│   │   ├── main.cpp        # Entry point of the project
│── .gitignore              # Ignoring unnecessary files
│── bittorrent              # (Need to clarify what this file is)
│── build.sh                # Build script
│── newsetup.sh             # Setup script
│── sample.torrent          # Example torrent file
```

## How to Run

1. **Clone the repo:**
   ```sh
   git clone <repo-link>
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

-


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


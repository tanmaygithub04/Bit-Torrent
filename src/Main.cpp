#include <iostream>
#include <string>
#include <vector>
#include "core/torrent.h"
#include "core/peer.h"
#include "core/tracker.h"
//#include "core/piece_manager.h"  // Uncomment if/when you implement piece management
#include "utils/bencode.h"
#include "utils/hash.h"
#include "utils/error.h"
#include "utils/terminal_ui.h"
#include "core/DownloadManager.h"

using namespace std;
using namespace BitTorrent;

int main(int argc, char* argv[]) {
    // Handle help command or no arguments
    if (argc < 2 || (argc == 2 && (string(argv[1]) == "--help" || string(argv[1]) == "-h"))) {
        TerminalUI::printUsage(argv[0]);
        return 0;
    }
    
    // Expecting: download_file <torrent_file>
    if (argc < 3) {
        TerminalUI::printUsage(argv[0]);
        TerminalUI::logError("Missing required arguments");
        return 1;
    }
    
    // Show banner
    TerminalUI::printBanner();
    
    const string command = argv[1];
    const string torrentFile = argv[2];

    if (command != "download_file") {
        TerminalUI::logError("Unknown command: " + command);
        TerminalUI::logInfo("Use --help to see available commands");
        return 1;
    }

    try {
        // Step 1: Load torrent metadata from file
        TerminalUI::logInfo("Loading torrent metadata from: " + torrentFile);
        
        auto metadata = TorrentMetadata::fromFile(torrentFile);
        TerminalUI::logSuccess("Torrent metadata loaded successfully");
        
        // Display torrent information in a beautiful format
        TerminalUI::printTorrentInfo(
            metadata.getInfoHash(),
            metadata.getAnnounceUrl(),
            metadata.getTotalLength(),
            metadata.getPieceLength(),
            metadata.getTotalPieces()
        );
        
        // Step 2: Request peer list from tracker
        TerminalUI::logNetwork("Connecting to tracker...");
        
        auto response = Tracker::getPeers(
            metadata, 
            "00112233445566778899",  // Example peer_id (should be 20 bytes)
            6881,                   // Port number to use for the tracker request
            0,                      // Uploaded (0 for now)
            0,                      // Downloaded (0 for now)
            metadata.getTotalLength()
        );
        
        if (!response || response->peers.empty()) {
            throw NetworkError("Failed to get peers from tracker");
        }
        
        TerminalUI::logSuccess("Successfully retrieved peer list from tracker");
        
        // Convert peer info to string format for display
        vector<string> peerStrings;
        for (const auto &peer_info : response->peers) {
            peerStrings.push_back(peer_info.ip + ":" + to_string(peer_info.port));
        }
        TerminalUI::printPeerList(peerStrings);
        
        // Step 3: Create download manager and connect to peers
        TerminalUI::printSectionHeader("Download Process", TerminalUI::Symbols::DOWNLOAD);
        TerminalUI::logInfo("Initializing download manager...");
        
        DownloadManager dm(&metadata, response->peers);
        
        TerminalUI::logNetwork("Connecting to peers...");
        dm.connectToPeers();
        TerminalUI::logSuccess("Connected to available peers");
        
        // Step 4: Download all pieces with progress tracking
        string outputPath = "./downloads/";
        
        // Create output directory if it doesn't exist
        system("mkdir -p ./downloads");
        
        int totalPieces = metadata.getTotalPieces();
        
        TerminalUI::logDownload("Starting download of " + to_string(totalPieces) + " pieces");
        cout << endl;
        
        for (int pieceIndex = 0; pieceIndex < totalPieces; ++pieceIndex) {
            TerminalUI::showProgress(pieceIndex, totalPieces, "Downloading pieces");
            
            auto pieceOpt = dm.downloadPiece(pieceIndex);
            if (!pieceOpt.has_value()) {
                cout << endl;
                TerminalUI::logError("Failed to download piece " + to_string(pieceIndex));
                TerminalUI::logInfo("You may want to try again or check your network connection");
                return 1;
            }
        }
        
        // Show completion of piece downloads
        TerminalUI::showProgress(totalPieces, totalPieces, "Downloading pieces");
        
        // Step 5: Assemble the final file
        TerminalUI::logInfo("Assembling downloaded pieces into final file...");
        
        bool assembleResult = dm.assembleFile(outputPath);
        
        if (assembleResult) {
            TerminalUI::logSuccess("File assembly completed successfully");
            
            // Show final completion message
            TerminalUI::printDownloadComplete(outputPath, metadata.getTotalLength());
        } else {
            TerminalUI::logError("Failed to assemble the final file");
            return 1;
        }
        
    } catch (const TorrentError& e) {
        TerminalUI::logError("Torrent Error: " + string(e.what()));
        return 1;
    } catch (const NetworkError& e) {
        TerminalUI::logError("Network Error: " + string(e.what()));
        TerminalUI::logInfo("Please check your internet connection and try again");
        return 1;
    } catch (const std::exception& e) {
        TerminalUI::logError("Unexpected Error: " + string(e.what()));
        return 1;
    }
    
    return 0;
}

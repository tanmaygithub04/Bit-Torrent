#include <iostream>
#include <string>
#include "core/torrent.h"
#include "core/peer.h"
#include "core/tracker.h"
//#include "core/piece_manager.h"  // Uncomment if/when you implement piece management
#include "utils/bencode.h"
#include "utils/hash.h"
#include "utils/error.h"
#include "core/DownloadManager.h"

using namespace std;
using namespace BitTorrent;

int main(int argc, char* argv[]) {
    // Expecting: download_piece <torrent_file>
    if (argc < 3) {
        cerr << "Usage: " << argv[0] << " download_file <torrent_file>" << endl;
        return 1;
    }
    
    const string command = argv[1];
    const string torrentFile = argv[2];

    if (command != "download_file") {
        cerr << "Unknown command: " << command << endl;
        return 1;
    }

    try {
        // Step 1: Load torrent metadata from file
        auto metadata = TorrentMetadata::fromFile(torrentFile);
        cout << "Torrent metadata loaded" << endl;
        cout <<" InfoHash: " << metadata.getInfoHash() << endl;
        cout << " Tracker URL:" << metadata.getAnnounceUrl() << endl;
        cout << "Length: " << metadata.getTotalLength() << endl;
        cout << "Piece Length: " << metadata.getPieceLength() << endl;
        // Step 2: Request peer list from tracker

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
        cout << "Peer list obtained:" << endl;
        for (const auto &peer_info : response->peers) {
            cout << "\t" << peer_info.ip << ":" << peer_info.port << endl;
        }
        
        // Step 3: create a downlaod Manger class
        DownloadManager dm(&metadata,response->peers);
        dm.connectToPeers();
        string outputPath = "../downloads";
        cout << "total peices to be downloaded : " <<  metadata.getTotalPieces() << endl;
        for (int pieceIndex = 0; pieceIndex < metadata.getTotalPieces(); ++pieceIndex) {
        cout << "Downloading piece " << pieceIndex << "..." << endl;
        auto pieceOpt = dm.downloadPiece(pieceIndex);
        if (!pieceOpt.has_value()) {
            cerr << "Failed to download piece " << pieceIndex << endl;
            // For a simple project, you may choose to exit or retry.
            return 1;
        }
    }
    bool res =  dm.assembleFile(outputPath);
        // In connectToPeers (or immediately after), each peer should:
        //    a) Complete handshake,
        //    b) Immediately call receiveMessage() to get the BITFIELD message,
        //    c) Then call updateBitfield(message.payload) to update its internal state.
        // This ensures that later piece selection (via hasPiece()) works.
        // dm.downloadPiece(1);
       
        if(res)
            cout << "success" << endl;
    }
    catch (const TorrentError& e) {
        cerr << "TorrentError: " << e.what() << endl;
        return 1;
    }
    catch (const std::exception& e) {
        cerr << "Error: " << e.what() << endl;
        return 1;
    }
    
    return 0;
}

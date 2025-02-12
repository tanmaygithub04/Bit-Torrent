#pragma once
#include <vector>
#include <string>
#include <optional>
#include "torrent.h"      // Contains TorrentMetadata definition.
#include "peer.h"         // Contains Peer definition.
#include "tracker.h"    // Or wherever Tracker::PeerInfo is defined.
#include <iostream>

// Forward declarations
class TorrentMetadata;
class Peer;
class Tracker;

class DownloadManager {
public:
    // Constructor: Takes a pointer to TorrentMetadata and a list of PeerInfo objects.
    DownloadManager(const TorrentMetadata* metadata, const std::vector<Tracker::PeerInfo>& peersInfo);

    // Connect to peers: Create Peer objects from the PeerInfo list and attempt connections.
    void connectToPeers();


    // Select the next piece to download (e.g., sequentially).
    int selectNextPiece() const;

    // Download a given piece by selecting an appropriate peer, calling its download_piece method,
    // and verifying the piece.
    std::optional<std::vector<uint8_t>> downloadPiece(int piece_idx);

    // Once all pieces are downloaded, assemble them into the final file.
    bool assembleFile( std::string& outputPath);

    // Accessor for connected peers.
    std::vector<std::shared_ptr<Peer>> getConnectedPeers() const;

    // Immutable torrent metadata.
    const TorrentMetadata* m_metadata;

    // List of PeerInfo objects from the tracker.
    std::vector<Tracker::PeerInfo> m_peersInfo;

    // Active (connected) Peer objects.
    std::vector<std::shared_ptr<Peer>> m_peers;

    // Track which pieces have been successfully downloaded.
    std::vector<bool> m_downloadedPieces;

    // Storage for raw piece data, indexed by piece index.
    std::vector<std::vector<uint8_t>> m_pieceData;

    // Storage for expected values of piece hashes for verificariton 
    std::vector<std::string> m_pieceHashes;

    int m_totalPieces;

    int m_piece_length;

    // Helper: Mark a piece as downloaded and store its data.
    void updateDownloadedPiece(int piece_idx, const std::vector<uint8_t>& data);

    // Helper: Calculate the actual length of a given piece.
    int actualPieceLength(int piece_idx);

    // (Optional) Helper: Choose the best peer for downloading a given piece.
    std::shared_ptr<Peer> selectPeerForPiece(int piece_idx) ;
};

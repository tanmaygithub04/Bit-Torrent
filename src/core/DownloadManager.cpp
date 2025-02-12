#include "DownloadManager.h"
#include "torrent.h"      // Contains TorrentMetadata definition.
#include "peer.h"         // Contains Peer definition.
#include "../utils/hash.h"  // For computeSHA1(), etc.
#include <iostream>
#include <fstream>
#include <algorithm>
#include <cassert>
using namespace std;

DownloadManager::DownloadManager(const TorrentMetadata* metadata, const std::vector<Tracker::PeerInfo>& peersInfo){
    m_metadata = metadata;
    m_peersInfo = peersInfo;
    m_totalPieces = metadata->getTotalPieces();
    m_downloadedPieces.assign(m_totalPieces, false);
    m_pieceData.resize(m_totalPieces);
    string concatenatedHashes = metadata->getPieces();
    for(size_t i=0 ; i<concatenatedHashes.size() ; i+=20){
        m_pieceHashes.push_back(concatenatedHashes.substr(i,20));
    }
    m_piece_length = metadata->getPieceLength();
}

void DownloadManager::connectToPeers(){

    for(size_t i = 0 ; i<m_peersInfo.size() ; i++){
        // you have peerInfo object 
        auto peer = std::make_shared<Peer>(m_peersInfo[i].ip,m_peersInfo[i].port, m_metadata->getTotalLength());
        if(peer->connect(m_metadata->getInfoHash() , m_peersInfo[i].peer_id)){
            m_peers.push_back(peer);
        }
        else{
            std::cerr << "Failed to connect to peer " << m_peersInfo[i].ip << ":" << m_peersInfo[i].port << std::endl;
        }
    }
}

vector<shared_ptr<Peer>> DownloadManager::getConnectedPeers() const {
    return m_peers;
}

int DownloadManager::selectNextPiece() const {
    for(int i=0 ; i<m_totalPieces ; i++){
        if(!m_downloadedPieces[i]){
            return i;
        }
    }
    return -1;
}
 
shared_ptr<Peer> DownloadManager::selectPeerForPiece(int pieceIdx)  {
    for(auto &peer : m_peers){
        if(peer->isConnected() && peer->hasPiece(pieceIdx)){
            return peer;
        }
    }
    // for our implementation all the peers have all the pieces required 
    for(auto &peer : m_peers){
        if(peer->isConnected()){
            return peer;
        }
    }
    return NULL;
}

int DownloadManager::actualPieceLength(int piece_idx){

    int totalLength = static_cast<int>(m_metadata->getTotalLength());
        if (piece_idx== m_totalPieces - 1) {
            return totalLength - (m_totalPieces - 1) * m_piece_length;
        }
    return m_piece_length;

}

void DownloadManager::updateDownloadedPiece(int piece_idx , const std::vector<uint8_t>& data){
    // mark this as downloaded 
   if (piece_idx >= 0 && piece_idx < m_totalPieces) {
        m_pieceData[piece_idx] = data;
        m_downloadedPieces[piece_idx] = true;
    }

}

std::optional<std::vector<uint8_t>> DownloadManager::downloadPiece(int piece_idx){
    cout << "downloading piece in download Manager" << endl;
    // check pieceidx 
    if(piece_idx < 0 || piece_idx > m_totalPieces){
        std::cerr << "Invalid piece index: " << piece_idx << std::endl;
        return std::nullopt;
    }

    // you have a valid idx so now select a peer 
    auto peer = selectPeerForPiece(piece_idx);
    cout << "Peer has been selected : " << peer->m_ip << ":" << peer->m_port << endl;

    // check if you got a peer 
    if(!peer){
        std::cerr << "No available peer for piece " << piece_idx << std::endl;
        return std::nullopt;
    }
    // now call the download piece function of this peer for this peice
    int pieceLen = actualPieceLength(piece_idx);
    cout << "starting the download" << endl;
    auto pieceDownloadOpt = peer->downloadPiece(piece_idx, pieceLen);
    if (!pieceDownloadOpt.has_value()) {
        std::cerr << "Peer failed to download piece " << piece_idx << std::endl;
        return std::nullopt;
    }
    // verify sha1 hash of the piece 
    std::string hashedVal = HashUtils::computeSHA1(std::string(pieceDownloadOpt->begin(), pieceDownloadOpt->end()));
    if (hashedVal != m_pieceHashes[piece_idx]) {
        std::cerr << "Hash mismatch for piece " << piece_idx << std::endl;
        return std::nullopt;
    }
    cout << "Piece " << piece_idx << " downloaded and verified successfully." << endl;
    updateDownloadedPiece(piece_idx, pieceDownloadOpt.value());
    return pieceDownloadOpt;
}



bool DownloadManager::assembleFile( std::string& outputPath) {
    // check if all pieces have been downloaded 
    outputPath +=m_metadata->getName();
    for(int i=0 ; i<m_totalPieces ; i++){
        if(!m_downloadedPieces[i]){
            cerr << "All pieces are not avialable" << endl;
            return false;
        }
    }
    std::ofstream outfile(outputPath, std::ios::binary);
    if(!outfile){
        cerr << "error opening the path , recheck the path" << outputPath << endl;
        return false;
    }
    for(int i=0 ; i<m_totalPieces ; i++){
        outfile.write(reinterpret_cast<const char*>(m_pieceData[i].data()), m_pieceData[i].size());
    }
    outfile.close();
    std::cout << "File assembled successfully: " << outputPath << std::endl;
    return true;
}



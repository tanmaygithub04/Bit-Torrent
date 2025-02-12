#include "torrent.h"
#include "../utils/bencode.h"
#include "../utils/hash.h"
#include "../utils/error.h"
#include <fstream>
#include <iostream>

TorrentMetadata TorrentMetadata::fromFile(const std::string& path) {
    std::ifstream file(path, std::ios::binary);
    if (!file) {
        throw BitTorrent::TorrentError("Cannot open torrent file: " + path);
    }

    std::string content((std::istreambuf_iterator<char>(file)),
                        std::istreambuf_iterator<char>());

    auto [decoded, _] = BencodeUtils::decode(content);
    if (!decoded) {
        throw BitTorrent::BencodeError("Failed to decode torrent file");
    }

    TorrentMetadata metadata;
    metadata.m_data = decoded.value();
    
    // Extract and store required fields
    if (!metadata.m_data.contains("announce")) {
        throw BitTorrent::TorrentError("Missing announce URL in torrent file");
    }
    metadata.m_announce_url = metadata.m_data["announce"].get<std::string>();

    auto info = metadata.m_data["info"];
    metadata.m_piece_length = info["piece length"].get<size_t>();
    metadata.m_pieces = info["pieces"].get<std::string>();
    metadata.m_name = info["name"].get<std::string>();
    
    // Calculate total length
    if (info.contains("length")) {
        metadata.m_total_length = info["length"].get<size_t>();
    } 
    // else if (info.contains("files")) {
        // metadata.m_total_length = 0;
        // for (const auto& file : info["files"]) {
        //     metadata.m_total_length += file["length"].get<size_t>();
        // }
    // }

    // Calculate info hash
    std::string bencoded_info = BencodeUtils::encode(info);
    metadata.m_info_hash = HashUtils::computeSHA1(bencoded_info);

    return metadata;
}

void TorrentMetadata::printInfo() const {
    std::cout << "Tracker URL: " << m_announce_url << "\n"
              << "Length: " << m_total_length << "\n"
              << "Info Hash: " << HashUtils::bytesToHex(m_info_hash) << "\n"
              << "Piece Length: " << m_piece_length << "\n"
              << "Piece Count: " << m_pieces.length() / 20 << std::endl;
} 

std::string TorrentMetadata::getInfoHash() const { return m_info_hash; }
std::string TorrentMetadata::getAnnounceUrl() const { return m_announce_url; }
size_t TorrentMetadata::getTotalLength() const { return m_total_length; }
size_t TorrentMetadata::getPieceLength() const { return m_piece_length; }
std::string TorrentMetadata::getPieces() const { return m_pieces; }
int TorrentMetadata::getTotalPieces() const { return (getTotalLength() + getPieceLength() - 1) / getPieceLength();}
std::string TorrentMetadata::getName() const {return m_name;}
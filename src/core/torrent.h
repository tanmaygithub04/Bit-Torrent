#pragma once
#include <string>
#include <vector>
#include "../lib/nlohmann/json.hpp"

class TorrentMetadata {
public:
    static TorrentMetadata fromFile(const std::string& path);
    
    // Getters used in main.cpp and other files
    std::string getInfoHash() const;
    std::string getAnnounceUrl() const;
    size_t getPieceLength() const;
    size_t getTotalLength() const;
    std::string getPieces() const;  // Returns concatenated SHA1 hashes of pieces
    int getTotalPieces() const;
    void printInfo() const;  // Used by the info command
    std::string getName() const;

private:
    nlohmann::json m_data;
    std::string m_info_hash;
    std::string m_announce_url;
    size_t m_total_length;
    size_t m_piece_length;
    std::string m_pieces;
    std::string m_name;
}; 
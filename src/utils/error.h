#pragma once
#include <stdexcept>
#include <string>

namespace BitTorrent {
    class TorrentError : public std::runtime_error {
    public:
        explicit TorrentError(const std::string& message) : std::runtime_error(message) {}
    };

    class BencodeError : public TorrentError {
    public:
        explicit BencodeError(const std::string& message) : TorrentError("Bencode error: " + message) {}
    };

    class NetworkError : public TorrentError {
    public:
        explicit NetworkError(const std::string& message) : TorrentError("Network error: " + message) {}
    };
} 
#pragma once
#include <string>
#include <vector>
#include <optional>
#include <cstdint>    // for int64_t
#include "torrent.h"

class Tracker {
public:
    struct PeerInfo {
        std::string ip;
        uint16_t port;
        std::string peer_id;
    };

    struct TrackerResponse {
        std::vector<PeerInfo> peers;
        int64_t interval = 0;
        int64_t min_interval = 0;
        int64_t complete = 0;
        int64_t incomplete = 0;
    };

    static std::optional<TrackerResponse> getPeers(
        const TorrentMetadata& metadata,
        const std::string& peer_id,
        uint16_t port,
        int64_t uploaded,
        int64_t downloaded,
        int64_t left
    );

private:
    static std::string buildTrackerUrl(
        const TorrentMetadata& metadata,
        const std::string& peer_id,
        uint16_t port,
        int64_t uploaded,
        int64_t downloaded,
        int64_t left
    );
};

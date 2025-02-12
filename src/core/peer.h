#pragma once
#include <string>
#include <memory>
#include <vector>
#include <queue>
#include <bitset>
#include <boost/asio.hpp>
#include <optional>
#include <iostream>

class Peer {
public:
    struct Message {
        enum Type {
            CHOKE = 0,
            UNCHOKE = 1,
            INTERESTED = 2,
            NOT_INTERESTED = 3,
            HAVE = 4,
            BITFIELD = 5,
            REQUEST = 6,
            PIECE = 7,
            CANCEL = 8
        };
        
        Type type;
        std::vector<uint8_t> payload;
    };


    Peer(std::string ip, uint16_t port, int totalLength);
    ~Peer();

    bool connect(const std::string& info_hash, const std::string& peer_id);
    bool performHandshake(const std::string& info_hash, const std::string& peer_id);
    bool sendMessage(const Message& msg);
    std::optional<Message> receiveMessage(std::chrono::seconds timeout = std::chrono::seconds(10));
    bool isConnected() const { return m_connected; }
    bool readMessage(std::vector<uint8_t>& buffer, size_t length, boost::system::error_code& ec);
    
    // New methods for piece download

    bool requestPiece(uint32_t index, uint32_t begin, uint32_t length);
    std::optional<std::vector<uint8_t>> downloadPiece(uint32_t index, int pieceLength, int BLOCK_SIZE = 16 * 1024);
    bool hasPiece(uint32_t index) const;
    void updateBitfield(const std::vector<uint8_t>& bitfield);
    bool verifyPiece(uint32_t index);
    
    std::string m_ip;
    uint16_t m_port;
    std::unique_ptr<boost::asio::ip::tcp::socket> m_socket;
    boost::asio::io_context m_io_context;
    bool m_connected;
    std::vector<bool> m_bitfield;
    bool m_choked{true};
    std::string m_peer_id;
    bool m_interested{false};
    
    


}; 
#include "peer.h"
#include "../utils/hash.h"
#include "../utils/error.h"
#include <iostream>
#include <algorithm>
#include <array>
#include <chrono>
using namespace std;
using namespace std::chrono_literals;

Peer::Peer(std::string ip, uint16_t port, int totalLength){
        cout << "Peer constructor called" << endl;
        m_ip = ip;
        m_port = port;
        m_socket = make_unique<boost::asio::ip::tcp::socket>(m_io_context);
        m_connected = false;
        m_bitfield = vector<bool>(totalLength, true); 
        // this will not be true in real world and we will need to get its value , 
        //the tracker I am using has the peers which have all the piece so I am hard 
        //coding it 
    }

Peer::~Peer() {
    if (m_connected && m_socket->is_open()) {
        boost::system::error_code ec;
        m_socket->close(ec);
    }
}

bool Peer::connect(const std::string& info_hash, const std::string& peer_id) {
    try {
        std::cout << "Connecting to " << m_ip << ":" << m_port << std::endl;
        boost::asio::ip::tcp::resolver resolver(m_io_context);
        boost::asio::ip::tcp::resolver::results_type endpoints = 
            resolver.resolve(m_ip, std::to_string(m_port));
        
        boost::system::error_code ec;
        boost::asio::connect(*m_socket, endpoints, ec);
        
        if (ec) {
            std::cerr << "Connection failed: " << ec.message() << std::endl;
            return false;
        }
        cout << "Connection successful" <<  endl;
        m_connected = performHandshake(info_hash, peer_id);
        if(!m_connected) return false;
        auto response = receiveMessage();
        if (!response || response->type != Message::Type::BITFIELD) {
            return false;
        }
        return true;
    } catch (const std::exception& e) {
        std::cerr << "Connection failed: " << e.what() << std::endl;
        return false;
    }
}

bool Peer::performHandshake(const std::string& info_hash, const std::string& peer_id) {
    if (info_hash.size() != 20) {
        std::cerr << "Invalid info_hash length: " << info_hash.size() << ". Expected 20 bytes." << std::endl;
        return false;
    }
    try {
        static const std::string protocol = "BitTorrent protocol";
        std::array<unsigned char, 68> handshake_msg;
        
        // Construct handshake message
        handshake_msg[0] = protocol.length();
        std::copy(protocol.begin(), protocol.end(), handshake_msg.begin() + 1);
        std::fill(handshake_msg.begin() + 20, handshake_msg.begin() + 28, 0);  // Reserved bytes
        std::copy(info_hash.begin(), info_hash.end(), handshake_msg.begin() + 28);
        std::copy(peer_id.begin(), peer_id.end(), handshake_msg.begin() + 48);

        // Send handshake
        boost::system::error_code ec;
        boost::asio::write(*m_socket, boost::asio::buffer(handshake_msg), ec);
        if (ec) {
            std::cerr << "Failed to send handshake: " << ec.message() << std::endl;
            return false;
        }
        // Receive response
        std::array<unsigned char, 68> response;
        size_t bytes_read = boost::asio::read(*m_socket, boost::asio::buffer(response.data(), response.size()), ec);
        if (ec || bytes_read != response.size()) {
            std::cerr << "Handshake read failed: " << ec.message() << std::endl;
            // Close the socket explicitly on error to avoid further operations on a dead socket.
            boost::system::error_code close_ec;
            m_socket->close(close_ec);
            return false;
        }
        std::pair<bool,string> p =  HashUtils::verifyHandshakeResponse(response, info_hash);
        m_peer_id = p.second;
        return p.first;
    } catch (const std::exception& e) {
        std::cerr << "Handshake failed: " << e.what() << std::endl;
        return false;
    }
}

bool Peer::sendMessage(const Message& msg) {
    try {
        // Message format: <length prefix><message ID><payload>
        vector<uint8_t> data;
        uint32_t length = 1 + msg.payload.size(); // message ID (1 byte) + payload
        
        // Add length prefix (4 bytes) (big endian format)
        data.push_back((length >> 24) & 0xFF);
        data.push_back((length >> 16) & 0xFF);
        data.push_back((length >> 8) & 0xFF);
        data.push_back(length & 0xFF);
        // Add message ID
        data.push_back(static_cast<uint8_t>(msg.type));
        // Add payload
        data.insert(data.end(), msg.payload.begin(), msg.payload.end());
        boost::system::error_code ec;
        boost::asio::write(*m_socket, boost::asio::buffer(data), ec);
        if (ec) {
            cerr << "SendMessage error: " << ec.message() << endl;
            return false;
        }
        return true;
    } catch (const std::exception& e) {
        std::cerr << "Failed to send message: " << e.what() << std::endl;
        return false;
    }
}

std::optional<Peer::Message> Peer::receiveMessage(std::chrono::seconds timeout) {
    try {
        // Read message length (4 bytes)
        std::vector<uint8_t> length_buf(4);
        boost::system::error_code ec;
        
        if (!readMessage(length_buf, 4, ec)) {
            return std::nullopt;
        }
        
        // Convert length bytes to integer
        uint32_t length = (length_buf[0] << 24) | (length_buf[1] << 16) |
                         (length_buf[2] << 8) | length_buf[3];
        
        if (length == 0) { // Keep-alive message
            return Message{Message::Type::CHOKE, {}}; // Return empty message
        }
        
        // Read message ID and payload
        std::vector<uint8_t> message_data(length);
        if (!readMessage(message_data, length, ec)) {
            return std::nullopt;
        }
        
        Message msg;
        msg.type = static_cast<Message::Type>(message_data[0]);
        msg.payload.assign(message_data.begin() + 1, message_data.end());
        return msg;
    } catch (const std::exception& e) {
        std::cerr << "Failed to receive message: " << e.what() << std::endl;
        return std::nullopt;
    }
}

bool Peer::readMessage(std::vector<uint8_t>& buffer, size_t length, boost::system::error_code& ec) {
    size_t total_read = 0;
    while (total_read < length) {
        size_t bytes_read = m_socket->read_some(
            boost::asio::buffer(buffer.data() + total_read, length - total_read), ec);
        
        if (ec) {
            std::cerr << "Read error: " << ec.message() << std::endl;
            return false;
        }
        
        total_read += bytes_read;
    }
    return true;
}

bool Peer::hasPiece(uint32_t index) const {
    return index < m_bitfield.size() && m_bitfield[index];
}


void Peer::updateBitfield(const std::vector<uint8_t>& bitfield) {
    m_bitfield.clear();
    m_bitfield.reserve(bitfield.size() * 8);
    
    for (uint8_t byte : bitfield) {
        for (int i = 7; i >= 0; i--) {
            m_bitfield.push_back((byte >> i) & 1);
        }
    }
}


bool Peer::requestPiece(uint32_t index, uint32_t begin, uint32_t length) {

    cout << "requesting Piece " << endl;

    Message msg;
    msg.type = Message::Type::REQUEST;
    
    // Construct request payload: <index><begin><length>
    std::vector<uint8_t> payload(12);
    for (int i = 0; i < 4; i++) {
        payload[i] = (index >> (24 - i * 8)) & 0xFF;
        payload[i + 4] = (begin >> (24 - i * 8)) & 0xFF;
        payload[i + 8] = (length >> (24 - i * 8)) & 0xFF;
    }
    msg.payload = std::move(payload);
    
    return sendMessage(msg);
}

std::optional<std::vector<uint8_t>> Peer::downloadPiece(uint32_t index, int piece_length, int BLOCK_SIZE){
    
    cout << "In peer download" << endl;
    // if (!m_connected || m_choked || !hasPiece(index)) {
    //     return std::nullopt;
    // }
    // Send interested message if not already interested
    if (!m_interested) {
        Message msg{Message::Type::INTERESTED, {}};
        if (!sendMessage(msg)) {
            return std::nullopt;
        }
        m_interested = true;
    }
    // Wait for unchoke
    if (m_choked) {
        auto response = receiveMessage();
        if (!response || response->type != Message::Type::UNCHOKE) {
            return std::nullopt;
        }
        m_choked = false;
    }
    
    std::vector<uint8_t> pieceData(piece_length,0);
    cout << "starting download ..." << endl;
    // Request blocks
    for (int offset = 0; offset < piece_length; offset += BLOCK_SIZE) {
        int length = min(BLOCK_SIZE, piece_length - offset);
        cout << " sending request .." << endl; 
        if (!requestPiece(index, offset, length)) {
            return std::nullopt;
        }
        auto block_response = receiveMessage();
        cout << " response received .." << endl; 

        if (!block_response || block_response->type != Message::Type::PIECE) {
            return std::nullopt;
        }
        if (block_response->payload.size() < 8) {
            cerr << "PIECE message payload too short." << endl;
            return nullopt;
        }
        
        std::copy(block_response->payload.begin() + 8, 
          block_response->payload.end(), 
          pieceData.begin() + offset);
    }
    cout << "Piece data has been acquired";
    cout << endl;
    return pieceData;
}
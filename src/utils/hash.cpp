#include "hash.h"
#include <openssl/evp.h> // Include EVP header
#include <openssl/sha.h>
#include <sstream>
#include <iomanip>
#include <array>
#include <stdexcept> 
std::string HashUtils::computeSHA1(const std::string& input) {
    // Create a context for the hash operation
    EVP_MD_CTX* ctx = EVP_MD_CTX_new();
    if (!ctx) {
        throw std::runtime_error("Failed to create EVP_MD_CTX");
    }

    // Initialize the context for SHA1
    if (EVP_DigestInit_ex(ctx, EVP_sha1(), nullptr) != 1) {
        EVP_MD_CTX_free(ctx);
        throw std::runtime_error("Failed to initialize SHA1 context");
    }

    // Update the context with the input data
    if (EVP_DigestUpdate(ctx, input.c_str(), input.length()) != 1) {
        EVP_MD_CTX_free(ctx);
        throw std::runtime_error("Failed to update SHA1 context");
    }

    // Finalize the hash and store it in a buffer
    unsigned char hash[SHA_DIGEST_LENGTH];
    unsigned int hashLen;
    if (EVP_DigestFinal_ex(ctx, hash, &hashLen) != 1) {
        EVP_MD_CTX_free(ctx);
        throw std::runtime_error("Failed to finalize SHA1 hash");
    }

    // Clean up the context
    EVP_MD_CTX_free(ctx);

    // Return the raw binary hash as a std::string
    return std::string(reinterpret_cast<char*>(hash), SHA_DIGEST_LENGTH);
}

std::string HashUtils::hash_to_hex(const std::string& hash) {
    std::ostringstream oss;
    for (unsigned char byte : hash) {
        oss << std::hex << std::setw(2) << std::setfill('0') 
            << static_cast<int>(byte);
    }
    return oss.str();
}

std::string HashUtils::bytesToHex(const std::string& bytes) {
    std::ostringstream oss;
    for (unsigned char byte : bytes) {
        oss << std::hex << std::setw(2) << std::setfill('0') 
            << static_cast<int>(byte);
    }
    return oss.str();
}

std::array<bool, 256> HashUtils::initializeUnreservedChars() {
    std::array<bool, 256> unreserved{};
    for (size_t i = '0'; i <= '9'; ++i) unreserved[i] = true;
    for (size_t i = 'A'; i <= 'Z'; ++i) unreserved[i] = true;
    for (size_t i = 'a'; i <= 'z'; ++i) unreserved[i] = true;
    unreserved['-'] = true;
    unreserved['_'] = true;
    unreserved['.'] = true;
    unreserved['~'] = true;
    return unreserved;
}

std::string HashUtils::urlEncode(const std::string& hex_string) {
    static const auto unreserved = initializeUnreservedChars();
    std::string result;
    result.reserve(hex_string.length() * 3);
    
    for (size_t i = 0; i < hex_string.length(); i += 2) {
        std::string byte_str = hex_string.substr(i, 2);
        size_t byte_val = std::stoul(byte_str, nullptr, 16);
        if (unreserved[byte_val]) {
            result += static_cast<char>(byte_val);
        } else {
            result += "%" + byte_str;
        }
    }
    return result;
}

std::vector<std::string> HashUtils::splitPieceHashes(const std::string& pieces) {
    std::vector<std::string> hashes;
    for (size_t i = 0; i < pieces.length(); i += 20) {
        hashes.push_back(pieces.substr(i, 20));
    }
    return hashes;
} 

std::pair<bool,std::string> HashUtils::verifyHandshakeResponse(const std::array<unsigned char, 68> response, const std::string& info_hash) {
    if (response.size() < 68) {
        return {false,""};
    }

    // Verify protocol
    const std::string protocol = "BitTorrent protocol";
    if (response[0] != protocol.length()) {
        return {false,""};
    }

    std::string received_protocol(response.begin() + 1, response.begin() + 20);
    if (received_protocol != protocol) {
        return {false,""};
    }

    // Verify info hash
    std::string received_hash(response.begin() + 28, response.begin() + 48);
    if (received_hash != info_hash) {
        return {false,""};
    }
    std::string m_peer_id = std::string(reinterpret_cast<const char*>(response.data() + 48), 20);
    // Store peer_id
    return {true,m_peer_id};
}

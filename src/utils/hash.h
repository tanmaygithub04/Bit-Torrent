#pragma once
#include <string>
#include <sstream>
#include <iomanip>
#include <array>

class HashUtils {
public:
    static std::string computeSHA1(const std::string& input);
    static std::string bytesToHex(const std::string& bytes);
    static std::string urlEncode(const std::string& hex_string);
    static std::string hash_to_hex(const std::string& hash);
    static std::pair<bool, std::string> verifyHandshakeResponse(const std::array<unsigned char, 68> response, const std::string& info_hash);

    
    // Helper method to split piece hashes
    static std::vector<std::string> splitPieceHashes(const std::string& pieces);
    
private:
    static constexpr std::array<char, 16> HEX_CHARS = {
        '0', '1', '2', '3', '4', '5', '6', '7',
        '8', '9', 'a', 'b', 'c', 'd', 'e', 'f'
    };

    static std::array<bool, 256> initializeUnreservedChars();
}; 
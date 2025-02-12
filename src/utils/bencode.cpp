#include "bencode.h"
#include <stdexcept>
#include <cctype>
#include <cstdlib>

using json = nlohmann::json;

std::string BencodeUtils::encode(const json& j) { 
    if (j.is_string()) {
        size_t len = j.get<std::string>().size();
        return std::to_string(len) + ":" + j.get<std::string>();
    }
    else if (j.is_number_integer()) {
        return "i" + std::to_string(j.get<int64_t>()) + "e";
    }
    else if (j.is_array()) {
        std::string bencoded = "l";
        for (const auto& item : j) {
            bencoded += encode(item);
        }
        bencoded += "e";
        return bencoded;
    }
    else if (j.is_object()) {
        std::string bencoded = "d";
        for (const auto& item : j.items()) {
            bencoded += encode(item.key());
            bencoded += encode(item.value());
        }
        bencoded += "e";
        return bencoded;
    }
    throw std::runtime_error("Unsupported JSON type for bencoding");
}

std::pair<std::optional<json>, size_t> BencodeUtils::decode(std::string_view encoded_value) {
    return decodeValue(encoded_value);
}

std::pair<std::optional<json>, size_t> BencodeUtils::decodeValue(std::string_view encoded_value) {
    if (encoded_value.empty()) return {std::nullopt, 0};

    if (std::isdigit(encoded_value[0])) {  // String
        size_t colon_pos = encoded_value.find(':');
        if (colon_pos == std::string_view::npos) return {std::nullopt, 0};
        
        long length = std::atol(encoded_value.data());
        if (length < 0 || colon_pos + 1 + length > encoded_value.size()) {
            return {std::nullopt, 0};
        }
        
        std::string str(encoded_value.substr(colon_pos + 1, length));
        return {json(str), colon_pos + 1 + length};
    }
    else if (encoded_value[0] == 'i') {  // Integer
        size_t e_pos = encoded_value.find('e', 1);
        if (e_pos == std::string_view::npos) return {std::nullopt, 0};
        
        int64_t num = std::atoll(encoded_value.data() + 1);
        return {json(num), e_pos + 1};
    }
    else if (encoded_value[0] == 'l') {  // List
        json arr = json::array();
        size_t offset = 1;
        
        while (offset < encoded_value.size() && encoded_value[offset] != 'e') {
            auto [item, advance] = decodeValue(encoded_value.substr(offset));
            if (!item) return {std::nullopt, 0};
            
            arr.push_back(*item);
            offset += advance;
        }
        
        if (offset >= encoded_value.size()) return {std::nullopt, 0};
        return {arr, offset + 1};  // +1 to skip 'e'
    }
    else if (encoded_value[0] == 'd') {  // Dictionary
        json dict = json::object();
        size_t offset = 1;
        
        while (offset < encoded_value.size() && encoded_value[offset] != 'e') {
            // Parse key
            auto [key, key_advance] = decodeValue(encoded_value.substr(offset));
            if (!key || !key->is_string()) return {std::nullopt, 0};
            offset += key_advance;
            
            // Parse value
            auto [value, value_advance] = decodeValue(encoded_value.substr(offset));
            if (!value) return {std::nullopt, 0};
            offset += value_advance;
            
            dict[key->get<std::string>()] = *value;
        }
        
        if (offset >= encoded_value.size()) return {std::nullopt, 0};
        return {dict, offset + 1};  // +1 to skip 'e'
    }
    
    return {std::nullopt, 0};
}


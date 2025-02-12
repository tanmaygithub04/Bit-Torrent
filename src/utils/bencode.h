#pragma once
#include <string>
#include <string_view>
#include <optional>
#include "../lib/nlohmann/json.hpp"

class BencodeUtils {
public:
    static std::string encode(const nlohmann::json& j);
    static std::pair<std::optional<nlohmann::json>, size_t> decode(std::string_view encoded_value);
    
private:
    static std::pair<std::optional<nlohmann::json>, size_t> decodeValue(std::string_view encoded_value);
};
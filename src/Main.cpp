#include <iostream>
#include <string>
#include <cctype>
#include <cstdlib>
#include <stdexcept>
#include <string_view>
#include <fstream>
#include <sstream>
#include <charconv>
#include <stdexcept>
#include "lib/nlohmann/json.hpp"
#include <openssl/sha.h>
#include "lib/hash/HTTPRequest.hpp"
#include <boost/asio.hpp>
#include <unistd.h>
using boost::asio::ip::tcp;
using json = nlohmann::json;
using namespace std;


std::string compute_sha1(const std::string& input) {
    unsigned char hash[SHA_DIGEST_LENGTH]; // SHA1 produces a 20-byte hash

    SHA_CTX sha1_ctx;
    SHA1_Init(&sha1_ctx);
    SHA1_Update(&sha1_ctx, input.c_str(), input.length());
    SHA1_Final(hash, &sha1_ctx);

    return std::string(reinterpret_cast<char*>(hash), SHA_DIGEST_LENGTH);
}

string hash_to_hex_string(const string& piece_hash){
    ostringstream oss;
        for (char c : piece_hash)
        {
                unsigned char byte = static_cast<unsigned char>(c); // cast to unsigned char to avoid sign extension
                int value = static_cast<int>(byte);
                oss << std::hex << std::setw(2) << std::setfill('0') << value;
        }
        return oss.str();
}

// Helper function to encode bytes into URL-safe format
string url_encode(const std::string &hex_string)
{
        string result;
        result.reserve(hex_string.length() + hex_string.length() / 2);
        std::array<bool, 256> unreserved{};
        for (size_t i = '0'; i <= '9'; ++i)
                unreserved[i] = true;
        for (size_t i = 'A'; i <= 'Z'; ++i)
                unreserved[i] = true;
        for (size_t i = 'a'; i <= 'z'; ++i)
                unreserved[i] = true;
        unreserved['-'] = true;
        unreserved['_'] = true;
        unreserved['.'] = true;
        unreserved['~'] = true;
        for (size_t i = 0; i < hex_string.length(); i += 2)
        {
                
                std::string byte_str = hex_string.substr(i, 2);
                size_t byte_val = std::stoul(byte_str, nullptr, 16);
                if (unreserved[byte_val])
                {
                        result += static_cast<char>(byte_val);
                }
                else
                {
                        result += "%" + byte_str;
                }
        }
        return result;
}

// Function to bencode a JSON object
string bencode(const json& j) 
{ 
    if(j.is_string()){
        size_t len = j.get<string>().size();
        return to_string(len) + ":" + j.get<string>();
    }
    else if(j.is_number_integer()){
        // return in format of i52e
        string bencoded = "i" + to_string(j.get<int64_t>()) + "e";
        return bencoded;
    }
    else if(j.is_array()){
            string bencoded = "l";
            for(auto item : j){
                bencoded += bencode(item);
            }
            bencoded += "e";
            return bencoded;
    }
    else if(j.is_object()){
        string bencoded = "d";
        for(const auto& item : j.items()){
                bencoded += bencode(item.key());
                bencoded += bencode(item.value());
            }
            bencoded += "e";
            return bencoded;
    }
    throw runtime_error("unsupported JSON type object to bencode");
}

pair<optional<json>, size_t> decode_value(string_view encoded_value) {
    if (isdigit(encoded_value.front())) {
        const size_t colon_index = encoded_value.find(':');
        if (colon_index != string::npos) {
            string_view number_string = encoded_value.substr(0, colon_index);
            const int64_t number = atoll(number_string.data());
            string_view str = encoded_value.substr(colon_index + 1, number);
            return {json(str), colon_index + number + 1};
        }
    } else if (encoded_value.front() == 'i') {
        const size_t e_index = encoded_value.find('e', 1);
        if (e_index != string::npos) {
            string_view number_string = encoded_value.substr(1, e_index - 1);
            const int64_t number = atoll(number_string.data());
            return {json(number), e_index + 1};
        }
    } else if (encoded_value.front() == 'l') {
        json decoded_array = json::array();
        size_t offset = 1;
        while (true) {
            auto [decoded_value, new_offset] = decode_value(encoded_value.substr(offset));
            if (!decoded_value.has_value()) break;
            decoded_array.emplace_back(decoded_value.value());
            offset += new_offset;
        }
        if (encoded_value.substr(offset).front() == 'e') {
            return {decoded_array, offset + 1};
        }
    } else if (encoded_value.front() == 'd') {
        json decoded_dict = json::object();
        size_t offset = 1;
        while (true) {
            auto [decoded_key, key_offset] = decode_value(encoded_value.substr(offset));
            if (!decoded_key.has_value()) break;
            offset += key_offset;
            auto [decoded_value, value_offset] = decode_value(encoded_value.substr(offset));
            if (!decoded_value.has_value()) break;
            decoded_dict[decoded_key.value().get<string>()] = decoded_value.value();
            offset += value_offset;
        }
        if (encoded_value.substr(offset).front() == 'e') {
            return {decoded_dict, offset + 1};
        }
    }
    return {nullopt, 0};
}

string format_string(const string& template_str, const vector<string>& args) {
    ostringstream oss;
    size_t pos = 0;
    for (const auto& arg : args) {
        size_t placeholder = template_str.find("{}", pos);
        if (placeholder == string::npos) break;
        oss << template_str.substr(pos, placeholder - pos) << arg;
        pos = placeholder + 2;
    }
    oss << template_str.substr(pos);
    return oss.str();
}


string perform_handshake(const std::string& ip, const string& port, const std::string& info_hash, const std::string& our_peer_id) {
    boost::asio::io_context context;
    boost::asio::ip::tcp::resolver resolver(context);
    boost::asio::ip::tcp::resolver::results_type endpoints;

    try {
        endpoints = resolver.resolve(ip, port);
    } catch (const boost::system::system_error& e) {
        std::cerr << "Error during hostname resolution: " << e.what() << std::endl;
        return ""; // Exit or handle error
    }
    tcp::socket socket(context);
    boost::asio::connect(socket, endpoints);

    if(socket.is_open()){
        cout << "socket is open " << endl;
        std::string protocol = "BitTorrent protocol";
        std::array<unsigned char, 68> handshake_msg;
        cout << info_hash << " <- info hash" << endl;
        // Fill in handshake message with "protocol" and other fields (infohash, peer id)
        handshake_msg[0] = protocol.length();
        std::copy(protocol.begin(), protocol.end(), handshake_msg.begin() + 1);
        
        // Add 8 reserved bytes (can be 0 for now)
        std::fill(handshake_msg.begin() + 20, handshake_msg.begin() + 28, 0);

        copy(info_hash.begin(),info_hash.end(),handshake_msg.begin() + 28);

        copy(our_peer_id.begin(),our_peer_id.end(),handshake_msg.begin() + 48);

        boost::asio::write(socket, boost::asio::buffer(handshake_msg));
        std::array<unsigned char, 68> response;
        boost::asio::read(socket, boost::asio::buffer(response));

        // check is info hash is same or not 
        size_t start_of_info_hash = 28;
        array<char, 20> res_info_hash;
        memcpy(res_info_hash.data(), handshake_msg.data() + start_of_info_hash, 20);
        if(memcmp(res_info_hash.data(), info_hash.data() , 20) == 0){
            string peerID(response.begin() + 48 , response.end());
            return peerID;
        }
        cout << "invalid response or error" << endl;
        return "";
    }

}

int main(int argc, char* argv[]) {
    cout << unitbuf;
    cerr << unitbuf;

    if (argc < 2) {
        cerr << "Usage: " << argv[0] << " decode <encoded_value> | info <file_path>" << endl;
        return 1;
    }

    const string command = argv[1];
    if (command == "decode") {
        if (argc < 3) {
            cerr << "Usage: " << argv[0] << " decode <encoded_value>" << endl;
            return 1;
        }

        string_view encoded_value = argv[2];
        if (const auto [decoded_value, offset] = decode_value(encoded_value);
            decoded_value.has_value()) {
        } else {
            cerr << format_string("Unhandled encoded value: {}", {string(encoded_value)}) << endl;
        }
    }
    else if (command == "info") {
        
        if (argc < 3) {
            cerr << "Usage: " << argv[0] << " info <file_path>" << endl;
            return 1;
        }

        filesystem::path torrent_path = argv[2];
        ifstream fileStream(torrent_path, ios::in | ios::binary);
        if (!fileStream) {
            cerr << format_string("Failed to open file: {}", {torrent_path.string()}) << endl;
            return 1;
        }
        try{
            stringstream contentStream;
            contentStream << fileStream.rdbuf();
            const string encoded_file_content = contentStream.str();
            const auto [decoded_value, offset] = decode_value(encoded_file_content);
            const auto info_dict = decoded_value.value()["info"];
            string bencoded_info_dict = bencode(info_dict);
            string hexed_sha1_info_dict = hash_to_hex_string(compute_sha1(bencoded_info_dict));

            if (decoded_value.has_value()) {
                cout << "Tracker URL: " << decoded_value.value()["announce"].get<string>() << endl;
                cout << "Length: " << info_dict["length"].get<int64_t>() << endl;
                cout << "Info Hash: " << hexed_sha1_info_dict << endl;
                cout << "Piece Length: " << info_dict["piece length"].get<int64_t>() << endl;

                // Parse and display piece hashes
                string pieces = info_dict["pieces"].get<string>();
                cout << "Piece Hashes:" << endl;    
                for (size_t i = 0; i < pieces.length(); i += 20)
                    {
                        string piece_hash = pieces.substr(i, 20);
                        std::cout << hash_to_hex_string(piece_hash) << std::endl;
                    }
            }
        }
        catch (const std::invalid_argument &e)
        {
                std::cerr << "Error decoding bencoded info dictionary: " << e.what() << std::endl;
                return 1;
        }
    }
    else if (command == "peers") {
        std::ifstream input_file{argv[2], std::ios::binary};
                if (!input_file)
                {
                        std::cerr << "Error opening torrent file: " << argv[2] << std::endl;
                        return 1;
                }
                std::vector<char> file_data((std::istreambuf_iterator<char>(input_file)), std::istreambuf_iterator<char>());
                std::string_view file_data_view(file_data.data(), file_data.size());
                try
                {
                        
                        auto [decoded_info, _1] = decode_value(file_data_view);
                        string bencoded_string = bencode(decoded_info.value()["info"]);
                        string url = decoded_info.value()["announce"].get<std::string>();
                        string hashed_sha_info = hash_to_hex_string(compute_sha1(bencoded_string));
                        string encoded_info_hash = url_encode(hashed_sha_info);

                        std::string left = std::to_string(file_data.size()); // Convert size_t to string
                        http::Request request{url + "?info_hash=" + encoded_info_hash + "&peer_id=00112233445566778899&port=6881&uploaded=0&downloaded=0&left=" + left + "&compact=1"};

                        const auto response = request.send("GET");

                        std::string response_body{response.body.begin(), response.body.end()};
                        std::string_view response_body_view(response_body.data(), response_body.size());
                        auto [decoded_response, _2] = decode_value(response_body_view);
                        std::string peers = decoded_response.value()["peers"].get<std::string>();
                        for (size_t i = 0; i < peers.length(); i += 6)
                        {
                                std::string ip = std::to_string(static_cast<unsigned char>(peers[i])) + "." +
                                                 std::to_string(static_cast<unsigned char>(peers[i + 1])) + "." +
                                                 std::to_string(static_cast<unsigned char>(peers[i + 2])) + "." +
                                                 std::to_string(static_cast<unsigned char>(peers[i + 3]));
                                uint16_t port = (static_cast<uint16_t>(static_cast<unsigned char>(peers[i + 4]) << 8)) | static_cast<uint16_t>(static_cast<unsigned char>(peers[i + 5]));
                                std::cout << ip << ":" << port << std::endl;
                        }
                }
                catch (const std::invalid_argument &e)
                {
                        std::cerr << "Error decoding bencoded info dictionary: " << e.what() << std::endl;
                        return 1;
                }
    } 
    else if(command == "handshake"){
        if (argc < 3) {
        cerr << "Usage: " << argv[0] << " handshake <torrent_file> <peer_ip> <peer_port>" << endl;
        return 1;
    }

    string peer_ip = argv[3];
    string peer_port = argv[4];

    // Read and parse torrent file
    ifstream input_file(argv[2], ios::binary);
    if (!input_file) {
        cerr << "Error opening torrent file: " << argv[2] << endl;
        return 1;
    }

    vector<char> file_data((istreambuf_iterator<char>(input_file)), istreambuf_iterator<char>());
    string_view file_data_view(file_data.data(), file_data.size());

    try {
        auto [decoded_info, _] = decode_value(file_data_view);
        string bencoded_string = bencode(decoded_info.value()["info"]);
        string sha1_info_dict = compute_sha1(bencoded_string);
        cout << sha1_info_dict << " my info hash ";
        // Generate a random peer ID (you might want to make this consistent)
        string peer_id = "00112233445566778899";
        // Perform the handshake
        string res = perform_handshake(peer_ip, peer_port, sha1_info_dict, peer_id);
        if(res != ""){
            cout << "Handshake successful!" << endl;
            cout << "peer Id is : " << hash_to_hex_string(res) << endl;
        } else {
            cerr << "Handshake failed!" << endl;
            return 1;
        }
    } catch (const std::exception& e) {
        cerr << "Error during handshake: " << e.what() << endl;
        return 1;
    }
    }
    else {
        cerr << "Unknown command: " << command << endl;
        return 1;
    }

    return 0;
}




// handshake sample.torrent 165.232.38.164 51493
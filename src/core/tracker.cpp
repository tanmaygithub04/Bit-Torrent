#include "tracker.h"
#include "../utils/hash.h"
#include "../utils/error.h"
// Removed: #include "../lib/hash/HTTPRequest.hpp"
#include "../utils/bencode.h"
#include <iostream>
#include <sstream>
#include <optional>
#include <regex>
#include <stdexcept>

// Boost headers
#include <boost/beast/core.hpp>
#include <boost/beast/http.hpp>
#include <boost/beast/version.hpp>
#include <boost/asio/ip/tcp.hpp>
#include <boost/asio/connect.hpp>

using namespace std;

// For convenience
namespace beast = boost::beast;         // Boost.Beast
namespace http = beast::http;           // HTTP from Beast
namespace net = boost::asio;            // Boost.Asio
using tcp = net::ip::tcp;

// Helper: parse URL into protocol, host, port and target
// (This is a basic implementation that assumes the URL starts with "http://" or "https://")
static void parseUrl(const std::string& url_str,
                     std::string& protocol,
                     std::string& host,
                     std::string& port,
                     std::string& target)
{
    // This regex will match URLs of the form:
    //    http(s)://host[:port][target]
    std::regex url_regex(R"(^(https?)://([^/:]+)(?::(\d+))?(/.*)?$)", std::regex::icase);
    std::smatch url_match_result;
    if (std::regex_match(url_str, url_match_result, url_regex))
    {
        protocol = url_match_result[1].str();
        host = url_match_result[2].str();
        // If port is specified in the URL, use it; otherwise, default based on protocol.
        if (url_match_result[3].matched && !url_match_result[3].str().empty())
            port = url_match_result[3].str();
        else
            port = (protocol == "http") ? "80" : "443";
        // Target (path and query) if present; otherwise, use "/".
        if (url_match_result[4].matched && !url_match_result[4].str().empty())
            target = url_match_result[4].str();
        else
            target = "/";
    }
    else
    {
        throw std::invalid_argument("Invalid URL: " + url_str);
    }
}

std::optional<Tracker::TrackerResponse> Tracker::getPeers(
    const TorrentMetadata& metadata,
    const std::string& peer_id,
    uint16_t port,
    int64_t uploaded,
    int64_t downloaded,
    int64_t left)
{
    cout << "getting peers " << endl;
    try {
        // Build the full tracker URL with query parameters.
        std::string request_url = buildTrackerUrl(metadata, peer_id, port, uploaded, downloaded, left);

        // Parse URL components: protocol, host, port, target (path + query)
        std::string protocol, host, portStr, target;
        parseUrl(request_url, protocol, host, portStr, target);
        std::cout << "Parsed values:" << std::endl;
        std::cout << "  protocol: " << protocol << std::endl;
        std::cout << "  host    : " << host << std::endl;
        std::cout << "  port    : " << portStr << std::endl;
        std::cout << "  target  : " << target << std::endl;
        // Set up Boost.Asio I/O context and resolve the host.
        net::io_context ioc;
        tcp::resolver resolver(ioc);
        auto const results = resolver.resolve(host, portStr);

        // Set up a TCP stream using Beast.
        beast::tcp_stream stream(ioc);
        stream.connect(results);
        cout << "connected";
        // Set up the HTTP GET request.
        http::request<http::string_body> req{ http::verb::get, target, 11 };
        req.set(http::field::host, host);
        req.set(http::field::user_agent, BOOST_BEAST_VERSION_STRING);

        // Send the HTTP request.
        http::write(stream, req);

        // Buffer for reading.
        beast::flat_buffer buffer;

        // Container for the HTTP response.
        http::response<http::dynamic_body> res;
        http::read(stream, buffer, res);

        // Gracefully close the socket.
        beast::error_code ec;
        stream.socket().shutdown(tcp::socket::shutdown_both, ec);
        // Note: not critical if shutdown fails.

        // Convert the response body to a string.
        std::string response_body = beast::buffers_to_string(res.body().data());
        cout << "response body " << response_body << endl;

        // Parse bencode response.
        auto [decoded_response, _] = BencodeUtils::decode(response_body);
        if (!decoded_response) {
            throw BitTorrent::BencodeError("Failed to decode tracker response");
        }
        // cout << "response : " << decoded_response.value() << endl;
        TrackerResponse result;
        const auto& response_dict = decoded_response.value();

        // Parse interval.
        if (response_dict.contains("interval")) {
            result.interval = response_dict["interval"].get<int64_t>();
        }

        // Parse min interval if present.
        if (response_dict.contains("min interval")) {
            result.min_interval = response_dict["min interval"].get<int64_t>();
        }

        // Parse complete/incomplete if present.
        if (response_dict.contains("complete")) {
            result.complete = response_dict["complete"].get<int64_t>();
        }
        if (response_dict.contains("incomplete")) {
            result.incomplete = response_dict["incomplete"].get<int64_t>();
        }

        // Parse peers.
        if (!response_dict.contains("peers")) {
            throw BitTorrent::NetworkError("No peers in tracker response");
        }

        std::string peers_data = response_dict["peers"].get<std::string>();

        // Parse compact peer format (6 bytes per peer).
        for (size_t i = 0; i < peers_data.length(); i += 6) {
            if (i + 6 > peers_data.length())
                break;

            PeerInfo peer;

            // Parse IP (first 4 bytes).
            peer.ip = std::to_string(static_cast<uint8_t>(peers_data[i])) + "." +
                      std::to_string(static_cast<uint8_t>(peers_data[i + 1])) + "." +
                      std::to_string(static_cast<uint8_t>(peers_data[i + 2])) + "." +
                      std::to_string(static_cast<uint8_t>(peers_data[i + 3]));

            // Parse port (last 2 bytes).
            peer.port = static_cast<uint16_t>(
                (static_cast<uint8_t>(peers_data[i + 4]) << 8) |
                static_cast<uint8_t>(peers_data[i + 5])
            );

            // In compact format, peer_id is not provided.
            peer.peer_id = "";
            result.peers.push_back(peer);
        }

        return result;
    }
    catch (const std::exception& e) {
        std::cerr << "Tracker request failed: " << e.what() << std::endl;
        return std::nullopt;
    }
}

std::string Tracker::buildTrackerUrl(
    const TorrentMetadata& metadata,
    const std::string& peer_id,
    uint16_t port,
    int64_t uploaded,
    int64_t downloaded,
    int64_t left)
{
    std::stringstream url;
    url << metadata.getAnnounceUrl()
        << "?info_hash=" << HashUtils::urlEncode(HashUtils::hash_to_hex(metadata.getInfoHash()))
        << "&peer_id=" << peer_id
        << "&port=" << port
        << "&uploaded=" << uploaded
        << "&downloaded=" << downloaded
        << "&left=" << left
        << "&compact=1";

    return url.str();
}

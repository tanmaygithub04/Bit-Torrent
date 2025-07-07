#pragma once
#include <iostream>
#include <string>
#include <chrono>
#include <iomanip>
#include <sstream>

namespace TerminalUI {
    // ANSI Color Codes
    namespace Colors {
        const std::string RESET = "\033[0m";
        const std::string BOLD = "\033[1m";
        const std::string DIM = "\033[2m";
        
        // Text Colors
        const std::string BLACK = "\033[30m";
        const std::string RED = "\033[31m";
        const std::string GREEN = "\033[32m";
        const std::string YELLOW = "\033[33m";
        const std::string BLUE = "\033[34m";
        const std::string MAGENTA = "\033[35m";
        const std::string CYAN = "\033[36m";
        const std::string WHITE = "\033[37m";
        
        // Bright Colors
        const std::string BRIGHT_RED = "\033[91m";
        const std::string BRIGHT_GREEN = "\033[92m";
        const std::string BRIGHT_YELLOW = "\033[93m";
        const std::string BRIGHT_BLUE = "\033[94m";
        const std::string BRIGHT_MAGENTA = "\033[95m";
        const std::string BRIGHT_CYAN = "\033[96m";
        const std::string BRIGHT_WHITE = "\033[97m";
        
        // Background Colors
        const std::string BG_BLACK = "\033[40m";
        const std::string BG_RED = "\033[41m";
        const std::string BG_GREEN = "\033[42m";
        const std::string BG_YELLOW = "\033[43m";
        const std::string BG_BLUE = "\033[44m";
        const std::string BG_MAGENTA = "\033[45m";
        const std::string BG_CYAN = "\033[46m";
        const std::string BG_WHITE = "\033[47m";
    }
    
    namespace Symbols {
        const std::string CHECK = "✓";
        const std::string CROSS = "✗";
        const std::string ARROW = "→";
        const std::string BULLET = "•";
        const std::string DOWNLOAD = "⬇";
        const std::string UPLOAD = "⬆";
        const std::string CONNECT = "🔗";
        const std::string GEAR = "⚙";
        const std::string ROCKET = "🚀";
        const std::string TORRENT = "🌊";
        const std::string FOLDER = "📁";
        const std::string FILE = "📄";
        const std::string NETWORK = "🌐";
        const std::string CLOCK = "⏰";
        const std::string LIGHTNING = "⚡";
    }
    
    // Get current timestamp
    inline std::string getCurrentTime() {
        auto now = std::chrono::system_clock::now();
        auto time_t = std::chrono::system_clock::to_time_t(now);
        std::stringstream ss;
        ss << std::put_time(std::localtime(&time_t), "%H:%M:%S");
        return ss.str();
    }
    
    // Format file size
    inline std::string formatFileSize(long long bytes) {
        const char* suffixes[] = {"B", "KB", "MB", "GB", "TB"};
        int suffixIndex = 0;
        double size = static_cast<double>(bytes);
        
        while (size >= 1024 && suffixIndex < 4) {
            size /= 1024;
            suffixIndex++;
        }
        
        std::stringstream ss;
        if (suffixIndex == 0) {
            ss << static_cast<long long>(size) << " " << suffixes[suffixIndex];
        } else {
            ss << std::fixed << std::setprecision(1) << size << " " << suffixes[suffixIndex];
        }
        return ss.str();
    }
    
    // Print banner
    inline void printBanner() {
        std::cout << Colors::BRIGHT_CYAN << Colors::BOLD
                  << "╔══════════════════════════════════════════════════════════════╗\n"
                  << "║                    " << Symbols::TORRENT << " BitTorrent Client v2.0 " << Symbols::TORRENT << "                    ║\n"
                  << "║                   High-Performance P2P Client                ║\n"
                  << "╚══════════════════════════════════════════════════════════════╝"
                  << Colors::RESET << std::endl << std::endl;
    }
    
    // Logging functions with different levels
    inline void logInfo(const std::string& message) {
        std::cout << Colors::DIM << "[" << getCurrentTime() << "] "
                  << Colors::BRIGHT_BLUE << Colors::BOLD << "INFO " << Colors::RESET 
                  << Colors::BLUE << Symbols::BULLET << " " << message << Colors::RESET << std::endl;
    }
    
    inline void logSuccess(const std::string& message) {
        std::cout << Colors::DIM << "[" << getCurrentTime() << "] "
                  << Colors::BRIGHT_GREEN << Colors::BOLD << "SUCCESS " << Colors::RESET 
                  << Colors::GREEN << Symbols::CHECK << " " << message << Colors::RESET << std::endl;
    }
    
    inline void logWarning(const std::string& message) {
        std::cout << Colors::DIM << "[" << getCurrentTime() << "] "
                  << Colors::BRIGHT_YELLOW << Colors::BOLD << "WARNING " << Colors::RESET 
                  << Colors::YELLOW << "⚠ " << message << Colors::RESET << std::endl;
    }
    
    inline void logError(const std::string& message) {
        std::cerr << Colors::DIM << "[" << getCurrentTime() << "] "
                  << Colors::BRIGHT_RED << Colors::BOLD << "ERROR " << Colors::RESET 
                  << Colors::RED << Symbols::CROSS << " " << message << Colors::RESET << std::endl;
    }
    
    inline void logDownload(const std::string& message) {
        std::cout << Colors::DIM << "[" << getCurrentTime() << "] "
                  << Colors::BRIGHT_MAGENTA << Colors::BOLD << "DOWNLOAD " << Colors::RESET 
                  << Colors::MAGENTA << Symbols::DOWNLOAD << " " << message << Colors::RESET << std::endl;
    }
    
    inline void logNetwork(const std::string& message) {
        std::cout << Colors::DIM << "[" << getCurrentTime() << "] "
                  << Colors::BRIGHT_CYAN << Colors::BOLD << "NETWORK " << Colors::RESET 
                  << Colors::CYAN << Symbols::NETWORK << " " << message << Colors::RESET << std::endl;
    }
    
    // Progress bar
    inline void showProgress(int current, int total, const std::string& prefix = "") {
        const int barWidth = 50;
        float progress = static_cast<float>(current) / total;
        int pos = static_cast<int>(barWidth * progress);
        
        std::cout << "\r" << Colors::BRIGHT_BLUE << prefix << Colors::RESET
                  << " [" << Colors::BRIGHT_GREEN;
        
        for (int i = 0; i < barWidth; ++i) {
            if (i < pos) std::cout << "█";
            else if (i == pos) std::cout << "▓";
            else std::cout << "░";
        }
        
        std::cout << Colors::RESET << "] " 
                  << Colors::BRIGHT_WHITE << std::setw(3) << static_cast<int>(progress * 100) << "%"
                  << Colors::DIM << " (" << current << "/" << total << ")" << Colors::RESET;
        
        if (current == total) {
            std::cout << " " << Colors::BRIGHT_GREEN << Symbols::CHECK << " Complete!" << Colors::RESET << std::endl;
        } else {
            std::cout << std::flush;
        }
    }
    
    // Section headers
    inline void printSectionHeader(const std::string& title, const std::string& symbol = Symbols::GEAR) {
        std::cout << std::endl << Colors::BRIGHT_WHITE << Colors::BOLD 
                  << "┌─ " << symbol << " " << title << " " << symbol << " ─┐" << Colors::RESET << std::endl;
    }
    
    // Print torrent info in a nice format
    inline void printTorrentInfo(const std::string& infoHash, const std::string& trackerUrl, 
                                long long totalLength, int pieceLength, int totalPieces) {
        printSectionHeader("Torrent Information", Symbols::FILE);
        
        std::cout << Colors::BRIGHT_WHITE << "  Info Hash:    " << Colors::RESET 
                  << Colors::YELLOW << infoHash << Colors::RESET << std::endl;
        std::cout << Colors::BRIGHT_WHITE << "  Tracker:      " << Colors::RESET 
                  << Colors::CYAN << trackerUrl << Colors::RESET << std::endl;
        std::cout << Colors::BRIGHT_WHITE << "  Total Size:   " << Colors::RESET 
                  << Colors::BRIGHT_GREEN << formatFileSize(totalLength) << Colors::RESET << std::endl;
        std::cout << Colors::BRIGHT_WHITE << "  Piece Length: " << Colors::RESET 
                  << Colors::BRIGHT_BLUE << formatFileSize(pieceLength) << Colors::RESET << std::endl;
        std::cout << Colors::BRIGHT_WHITE << "  Total Pieces: " << Colors::RESET 
                  << Colors::BRIGHT_MAGENTA << totalPieces << Colors::RESET << std::endl;
        
        std::cout << Colors::DIM << "└─────────────────────────────────────────────────────────────┘" 
                  << Colors::RESET << std::endl;
    }
    
    // Print peer list
    inline void printPeerList(const std::vector<std::string>& peers) {
        printSectionHeader("Peer Discovery", Symbols::NETWORK);
        
        std::cout << Colors::BRIGHT_WHITE << "  Found " << Colors::BRIGHT_GREEN 
                  << peers.size() << Colors::BRIGHT_WHITE << " peers:" << Colors::RESET << std::endl;
        
        for (size_t i = 0; i < peers.size(); ++i) {
            std::cout << Colors::DIM << "  " << (i + 1) << ". " << Colors::RESET 
                      << Colors::CYAN << peers[i] << Colors::RESET << std::endl;
        }
        
        std::cout << Colors::DIM << "└─────────────────────────────────────────────────────────────┘" 
                  << Colors::RESET << std::endl;
    }
    
    // Print usage information
    inline void printUsage(const std::string& programName) {
        printBanner();
        
        std::cout << Colors::BRIGHT_WHITE << Colors::BOLD << "USAGE:" << Colors::RESET << std::endl;
        std::cout << "  " << Colors::BRIGHT_CYAN << programName << " download_file <torrent_file>" << Colors::RESET << std::endl;
        std::cout << "  " << Colors::BRIGHT_CYAN << programName << " --help" << Colors::RESET << std::endl << std::endl;
        
        std::cout << Colors::BRIGHT_WHITE << Colors::BOLD << "EXAMPLES:" << Colors::RESET << std::endl;
        std::cout << "  " << Colors::DIM << "# Download a torrent file" << Colors::RESET << std::endl;
        std::cout << "  " << Colors::BRIGHT_GREEN << programName << " download_file sample.torrent" << Colors::RESET << std::endl;
        std::cout << "  " << Colors::BRIGHT_GREEN << programName << " download_file /path/to/movie.torrent" << Colors::RESET << std::endl << std::endl;
        
        std::cout << Colors::BRIGHT_WHITE << Colors::BOLD << "FEATURES:" << Colors::RESET << std::endl;
        std::cout << "  " << Colors::GREEN << Symbols::CHECK << " Block-based piece downloading" << Colors::RESET << std::endl;
        std::cout << "  " << Colors::GREEN << Symbols::CHECK << " Piece integrity verification" << Colors::RESET << std::endl;
        std::cout << "  " << Colors::GREEN << Symbols::CHECK << " Multi-peer connections" << Colors::RESET << std::endl;
        std::cout << "  " << Colors::GREEN << Symbols::CHECK << " Tracker communication" << Colors::RESET << std::endl;
        std::cout << "  " << Colors::GREEN << Symbols::CHECK << " Cross-platform support" << Colors::RESET << std::endl << std::endl;
    }
    
    // Clear line (for progress updates)
    inline void clearLine() {
        std::cout << "\r\033[K";
    }
    
    // Print final success message
    inline void printDownloadComplete(const std::string& filePath, long long fileSize) {
        std::cout << std::endl;
        std::cout << Colors::BRIGHT_GREEN << Colors::BOLD 
                  << "╔══════════════════════════════════════════════════════════════╗" << std::endl;
        std::cout << "║                    " << Symbols::ROCKET << " DOWNLOAD COMPLETE! " << Symbols::ROCKET << "                    ║" << std::endl;
        std::cout << "╚══════════════════════════════════════════════════════════════╝" << Colors::RESET << std::endl;
        
        std::cout << Colors::BRIGHT_WHITE << "  File saved to: " << Colors::RESET 
                  << Colors::BRIGHT_CYAN << filePath << Colors::RESET << std::endl;
        std::cout << Colors::BRIGHT_WHITE << "  Total size:    " << Colors::RESET 
                  << Colors::BRIGHT_GREEN << formatFileSize(fileSize) << Colors::RESET << std::endl;
        
        std::cout << std::endl << Colors::BRIGHT_YELLOW << Symbols::LIGHTNING 
                  << " Happy torrenting! " << Symbols::LIGHTNING << Colors::RESET << std::endl << std::endl;
    }
} 
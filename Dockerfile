# Multi-stage Dockerfile for BitTorrent Client
# Stage 1: Build environment
FROM alpine:3.19 AS builder

# Install build dependencies
RUN apk update && \
    apk add --no-cache \
        build-base \
        cmake \
        boost-dev \
        openssl-dev \
        linux-headers \
        pkgconfig

# Set working directory
WORKDIR /app

# Copy source files
COPY src/ ./src/
COPY build.sh ./

# Build the application
RUN mkdir -p build && \
    # Find the correct main file
    if [ -f "src/main.cpp" ]; then \
        MAIN_FILE="src/main.cpp"; \
    elif [ -f "src/Main.cpp" ]; then \
        MAIN_FILE="src/Main.cpp"; \
    else \
        echo "Neither src/main.cpp nor src/Main.cpp found!" && exit 1; \
    fi && \
    echo "Using main file: $MAIN_FILE" && \
    # Compile the application
    g++ -std=c++20 \
        -I./src \
        -I./src/core \
        -I./src/utils \
        -I./src/lib \
        -o build/bittorrent \
        $MAIN_FILE \
        src/core/*.cpp \
        src/utils/*.cpp \
        -lboost_system \
        -lssl \
        -lcrypto \
        -lpthread \
        -O2 \
        -DNDEBUG \
        -static-libgcc \
        -static-libstdc++ && \
    chmod +x build/bittorrent

# Stage 2: Runtime environment
FROM alpine:3.19 AS runtime

# Install runtime dependencies
RUN apk update && \
    apk add --no-cache \
        libstdc++ \
        libssl3 \
        libcrypto3 && \
    rm -rf /var/cache/apk/*

# Create a non-root user
RUN addgroup -g 1000 -S bittorrent && \
    adduser -u 1000 -S bittorrent -G bittorrent

# Set working directory
WORKDIR /app

# Copy the built executable
COPY --from=builder /app/build/bittorrent /app/bittorrent

# Create directories for downloads and torrents
RUN mkdir -p /app/downloads /app/torrents && \
    chown -R bittorrent:bittorrent /app

# Switch to non-root user
USER bittorrent

# Create volumes for data persistence
VOLUME ["/app/downloads", "/app/torrents"]

# Expose default port (if your BitTorrent client uses one)
EXPOSE 6881

# Health check
HEALTHCHECK --interval=30s --timeout=10s --start-period=5s --retries=3 \
    CMD ./bittorrent --help > /dev/null 2>&1 || exit 1

# Default command
ENTRYPOINT ["./bittorrent"]
CMD ["--help"] 
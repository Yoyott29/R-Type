/*
** EPITECH PROJECT, 2026
** Gringo Engine
** File description:
** UdpSocket
*/

#pragma once
#include <cstddef>
#include <cstdint>
#include <functional>
#include <memory>
#include <span>
#include <string>
#include <vector>

// IPv4 peer address; `ip` is in host byte order (127.0.0.1 == 0x7F000001)
struct Endpoint {
    std::uint32_t ip;
    std::uint16_t port;

    bool operator==(Endpoint const &other) const { return ip == other.ip && port == other.port; }
};

template <> struct std::hash<Endpoint> {
    std::size_t operator()(Endpoint const &ep) const noexcept
    {
        return std::hash<std::uint64_t>{}((static_cast<std::uint64_t>(ep.ip) << 16) | ep.port);
    }
};

// A received datagram and its sender
struct Datagram {
    Endpoint from;
    std::vector<std::byte> bytes;
};

// Datagrams held between two drain() calls; the newer ones are dropped once it is full
inline constexpr std::size_t RECEIVE_QUEUE_CAPACITY = 1024;

// UDP socket owned by its own I/O thread; the only Asio user (Asio stays out of this header).
// drain(), send() and close() are called from one thread (the main thread)
class UdpSocket
{
  public:
    // Binds host:port (host = IPv4 literal, default 0.0.0.0; port 0 = ephemeral) and starts the
    // I/O thread; throws NetError on a bad host or bind failure
    static UdpSocket bindServer(std::uint16_t port, std::string const &host = "0.0.0.0");
    // Resolves host once (first IPv4 result), binds port 0, stays unconnected, starts the I/O
    // thread; throws NetError
    static UdpSocket bindClient(std::string const &host, std::uint16_t port, Endpoint &server);

    UdpSocket(UdpSocket &&) noexcept;
    // Closes this socket first
    UdpSocket &operator=(UdpSocket &&) noexcept;
    // close()
    ~UdpSocket();

    // The datagrams received since the last call, oldest first
    std::vector<Datagram> drain();
    // Copies the data and hands it to the I/O thread; a failed send is dropped
    void send(Endpoint const &to, std::span<const std::byte> data);
    // Runs the sends already handed over, closes the socket and joins the thread; idempotent
    void close();
    std::uint16_t localPort() const;

  private:
    struct Impl;
    explicit UdpSocket(std::unique_ptr<Impl> impl);

    std::unique_ptr<Impl> _impl;
};

/*
** EPITECH PROJECT, 2026
** R-Type
** File description:
** UdpSocket
*/

#pragma once
#include <cstddef>
#include <cstdint>
#include <functional>
#include <memory>
#include <optional>
#include <span>
#include <string>

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

// Non-blocking UDP socket; the only Asio user (Asio stays out of this header)
class UdpSocket
{
  public:
    // Binds host:port (host = IPv4 literal, default 0.0.0.0; port 0 = ephemeral);
    // throws NetError on a bad host or bind failure
    static UdpSocket bindServer(std::uint16_t port, std::string const &host = "0.0.0.0");
    // Resolves host once (first IPv4 result), binds port 0, stays unconnected; throws NetError
    static UdpSocket bindClient(std::string const &host, std::uint16_t port, Endpoint &server);

    UdpSocket(UdpSocket &&) noexcept;
    UdpSocket &operator=(UdpSocket &&) noexcept;
    ~UdpSocket();

    // nullopt = nothing pending (or no socket); resets/refusals and bad datagrams are skipped
    std::optional<std::size_t> receive(std::span<std::byte> buf, Endpoint &from);
    // false = datagram not sent
    bool send(Endpoint const &to, std::span<const std::byte> data);
    std::uint16_t localPort() const;

  private:
    struct Impl;
    explicit UdpSocket(std::unique_ptr<Impl> impl);

    std::unique_ptr<Impl> _impl;
};

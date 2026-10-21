/*
** EPITECH PROJECT, 2026
** R-Type
** File description:
** UdpSocket
*/

#include "UdpSocket.hpp"
#include "ByteStream.hpp"
#include <asio.hpp>
#include <iostream>
#include <utility>

using asio::ip::udp;

struct UdpSocket::Impl {
    asio::io_context io;
    udp::socket socket{io};
    bool loggedReceive = false;
    bool loggedSend = false;
};

namespace
{
void configure(udp::socket &socket, udp::endpoint const &local)
{
    socket.open(udp::v4());
    socket.bind(local);
    socket.non_blocking(true);
    socket.set_option(asio::socket_base::receive_buffer_size(1 << 20));
}

Endpoint toEndpoint(udp::endpoint const &endpoint)
{
    return Endpoint{endpoint.address().to_v4().to_uint(), endpoint.port()};
}

void logOnce(bool &flag, char const *what, asio::error_code const &ec)
{
    if (flag)
        return;
    flag = true;
    std::cerr << "NetworkPlugin: " << what << " failed: " << ec.message() << '\n';
}
} // namespace

UdpSocket::UdpSocket(std::unique_ptr<Impl> impl) : _impl(std::move(impl)) {}

UdpSocket::UdpSocket(UdpSocket &&) noexcept = default;
UdpSocket &UdpSocket::operator=(UdpSocket &&) noexcept = default;
UdpSocket::~UdpSocket() = default;

UdpSocket UdpSocket::bindServer(std::uint16_t port, std::string const &host)
{
    auto impl = std::make_unique<Impl>();
    try {
        configure(impl->socket, udp::endpoint(asio::ip::make_address_v4(host), port));
    } catch (std::system_error const &e) {
        throw NetError(e.what());
    }
    return UdpSocket(std::move(impl));
}

UdpSocket UdpSocket::bindClient(std::string const &host, std::uint16_t port, Endpoint &server)
{
    auto impl = std::make_unique<Impl>();
    try {
        udp::resolver resolver(impl->io);
        auto results = resolver.resolve(udp::v4(), host, std::to_string(port));
        if (results.begin() == results.end())
            throw NetError("No IPv4 address found for " + host);
        udp::endpoint remote = results.begin()->endpoint();
        configure(impl->socket, udp::endpoint(udp::v4(), 0));
        server = toEndpoint(remote);
    } catch (std::system_error const &e) {
        throw NetError(e.what());
    }
    return UdpSocket(std::move(impl));
}

std::optional<std::size_t> UdpSocket::receive(std::span<std::byte> buf, Endpoint &from)
{
    if (!_impl)
        return std::nullopt;
    for (;;) {
        udp::endpoint sender;
        asio::error_code ec;
        std::size_t n =
            _impl->socket.receive_from(asio::buffer(buf.data(), buf.size()), sender, 0, ec);
        if (!ec) {
            from = toEndpoint(sender);
            return n;
        }
        if (ec == asio::error::would_block || ec == asio::error::try_again)
            return std::nullopt;
        if (ec == asio::error::connection_reset || ec == asio::error::connection_refused ||
            ec == asio::error::message_size)
            continue;
        logOnce(_impl->loggedReceive, "receive", ec);
        return std::nullopt;
    }
}

bool UdpSocket::send(Endpoint const &to, std::span<const std::byte> data)
{
    if (!_impl)
        return false;
    asio::error_code ec;
    udp::endpoint dest(asio::ip::address_v4(to.ip), to.port);
    _impl->socket.send_to(asio::buffer(data.data(), data.size()), dest, 0, ec);
    if (!ec)
        return true;
    if (ec != asio::error::would_block && ec != asio::error::try_again)
        logOnce(_impl->loggedSend, "send", ec);
    return false;
}

std::uint16_t UdpSocket::localPort() const
{
    if (!_impl)
        return 0;
    asio::error_code ec;
    auto ep = _impl->socket.local_endpoint(ec);
    return ec ? 0 : ep.port();
}

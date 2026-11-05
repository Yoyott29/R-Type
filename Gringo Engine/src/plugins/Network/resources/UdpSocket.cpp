/*
** EPITECH PROJECT, 2026
** Gringo Engine
** File description:
** UdpSocket
*/

#include "UdpSocket.hpp"
#include "ByteStream.hpp"
#include "Packet.hpp"
#include <array>
#include <asio.hpp>
#include <chrono>
#include <iostream>
#include <mutex>
#include <thread>
#include <utility>

using asio::ip::udp;

// Bigger than MAX_DATAGRAM_SIZE, so a valid datagram is never truncated
static const std::size_t READ_BUFFER_SIZE = 1500;
static_assert(READ_BUFFER_SIZE > MAX_DATAGRAM_SIZE);
// Pause before re-arming after an unexpected receive error, so a persistent one cannot spin
static const std::chrono::milliseconds RECEIVE_RETRY_DELAY{10};

namespace
{
void configure(udp::socket &socket, udp::endpoint const &local)
{
    socket.open(udp::v4());
    socket.bind(local);
    // A send never blocks the I/O thread: it fails with would_block and is dropped
    socket.non_blocking(true);
    socket.set_option(asio::socket_base::receive_buffer_size(1 << 20));
}

Endpoint toEndpoint(udp::endpoint const &endpoint)
{
    return Endpoint{endpoint.address().to_v4().to_uint(), endpoint.port()};
}

void logOnce(bool &flag, std::string const &what, asio::error_code const &ec)
{
    if (flag)
        return;
    flag = true;
    std::cerr << "NetworkPlugin: " << what << " failed: " << ec.message() << '\n';
}
} // namespace

// Heap state shared with the I/O thread, so moving a UdpSocket never moves it.
// The thread owns the socket, buffer, sender and logged* flags; the queue is behind the mutex
struct UdpSocket::Impl {
    asio::io_context io;
    udp::socket socket{io};
    asio::steady_timer retry{io}; // I/O thread only, like the socket
    asio::executor_work_guard<asio::io_context::executor_type> work = asio::make_work_guard(io);
    std::thread thread;
    std::uint16_t localPort = 0;
    bool closed = false; // main thread only

    std::mutex mutex;
    std::vector<Datagram> queue;

    std::array<std::byte, READ_BUFFER_SIZE> buffer{};
    udp::endpoint sender;
    bool loggedReceive = false;
    bool loggedSend = false;

    // I/O thread: one receive at a time, re-armed until the socket is closed (after a delay
    // on an unexpected error)
    void receiveNext()
    {
        socket.async_receive_from(
            asio::buffer(buffer), sender,
            [this](asio::error_code const &ec, std::size_t size) { onReceive(ec, size); });
    }

    void onReceive(asio::error_code const &ec, std::size_t size)
    {
        if (ec == asio::error::operation_aborted || !socket.is_open())
            return;
        if (!ec) {
            Datagram datagram{toEndpoint(sender),
                              std::vector<std::byte>(buffer.begin(), buffer.begin() + size)};
            std::lock_guard<std::mutex> lock(mutex);
            if (queue.size() < RECEIVE_QUEUE_CAPACITY)
                queue.push_back(std::move(datagram));
        } else if (ec != asio::error::connection_refused && ec != asio::error::connection_reset &&
                   ec != asio::error::message_size) {
            logOnce(loggedReceive, "receive", ec);
            retry.expires_after(RECEIVE_RETRY_DELAY);
            retry.async_wait([this](asio::error_code const &waitError) {
                if (!waitError && socket.is_open())
                    receiveNext();
            });
            return;
        }
        receiveNext();
    }
};

UdpSocket::UdpSocket(std::unique_ptr<Impl> impl) : _impl(std::move(impl))
{
    _impl->localPort = _impl->socket.local_endpoint().port();
    _impl->receiveNext();
    _impl->thread = std::thread([io = &_impl->io] { io->run(); });
}

UdpSocket::UdpSocket(UdpSocket &&) noexcept = default;

UdpSocket &UdpSocket::operator=(UdpSocket &&other) noexcept
{
    if (this != &other) {
        close();
        _impl = std::move(other._impl);
    }
    return *this;
}

UdpSocket::~UdpSocket() { close(); }

UdpSocket UdpSocket::bindServer(std::uint16_t port, std::string const &host)
{
    auto impl = std::make_unique<Impl>();
    try {
        configure(impl->socket, udp::endpoint(asio::ip::make_address_v4(host), port));
        return UdpSocket(std::move(impl));
    } catch (std::system_error const &e) {
        throw NetError(e.what());
    }
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
        return UdpSocket(std::move(impl));
    } catch (std::system_error const &e) {
        throw NetError(e.what());
    }
}

std::vector<Datagram> UdpSocket::drain()
{
    if (!_impl)
        return {};
    std::lock_guard<std::mutex> lock(_impl->mutex);
    std::vector<Datagram> datagrams;
    datagrams.swap(_impl->queue);
    return datagrams;
}

void UdpSocket::send(Endpoint const &to, std::span<const std::byte> data)
{
    if (!_impl || _impl->closed)
        return;
    asio::post(_impl->io,
               [impl = _impl.get(), to, bytes = std::vector<std::byte>(data.begin(), data.end())] {
                   asio::error_code ec;
                   udp::endpoint dest(asio::ip::address_v4(to.ip), to.port);
                   impl->socket.send_to(asio::buffer(bytes), dest, 0, ec);
                   if (ec && ec != asio::error::would_block && ec != asio::error::try_again)
                       logOnce(impl->loggedSend, "send", ec);
               });
}

void UdpSocket::close()
{
    if (!_impl || _impl->closed)
        return;
    _impl->closed = true;
    // FIFO: the sends posted before run first; the close aborts the pending receive or retry wait
    asio::post(_impl->io, [impl = _impl.get()] {
        asio::error_code ec;
        impl->socket.close(ec);
        impl->retry.cancel();
    });
    _impl->work.reset();
    _impl->thread.join();
}

std::uint16_t UdpSocket::localPort() const { return _impl ? _impl->localPort : 0; }

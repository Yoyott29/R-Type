/*
** EPITECH PROJECT, 2026
** R-Type
** File description:
** ByteStream
*/

#pragma once
#include "EngineError.hpp"
#include <bit>
#include <cstddef>
#include <cstdint>
#include <limits>
#include <span>
#include <string>
#include <type_traits>
#include <vector>

class NetError : public EngineError
{
  public:
    using EngineError::EngineError;
};

template <std::size_t N> struct ByteStreamUnsigned;
template <> struct ByteStreamUnsigned<1> {
    using type = std::uint8_t;
};
template <> struct ByteStreamUnsigned<2> {
    using type = std::uint16_t;
};
template <> struct ByteStreamUnsigned<4> {
    using type = std::uint32_t;
};
template <> struct ByteStreamUnsigned<8> {
    using type = std::uint64_t;
};

// Little endian writer, correct on any host byte order
class ByteWriter
{
  public:
    template <typename T> void write(T value)
    {
        static_assert(std::is_arithmetic_v<T> || std::is_enum_v<T>,
                      "ByteWriter::write only handles arithmetic and enum types");
        using U = typename ByteStreamUnsigned<sizeof(T)>::type;
        U bits;
        if constexpr (std::is_enum_v<T>)
            bits = static_cast<U>(static_cast<std::underlying_type_t<T>>(value));
        else if constexpr (std::is_same_v<T, bool>)
            bits = value ? 1 : 0;
        else
            bits = std::bit_cast<U>(value);
        for (std::size_t i = 0; i < sizeof(T); ++i)
            _bytes.push_back(static_cast<std::byte>((bits >> (8 * i)) & 0xFF));
    }

    void write(std::string const &value)
    {
        if (value.size() > std::numeric_limits<std::uint16_t>::max())
            throw NetError("String too long for the wire (max 65535 bytes)");
        write<std::uint16_t>(static_cast<std::uint16_t>(value.size()));
        for (char c : value)
            _bytes.push_back(static_cast<std::byte>(c));
    }

    void write(std::span<const std::byte> bytes)
    {
        _bytes.insert(_bytes.end(), bytes.begin(), bytes.end());
    }

    void reserve(std::size_t capacity) { _bytes.reserve(capacity); }

    std::vector<std::byte> take() { return std::move(_bytes); }

  private:
    std::vector<std::byte> _bytes;
};

// Little endian reader, every read is bounds checked
class ByteReader
{
  public:
    explicit ByteReader(std::span<const std::byte> data) : _data(data) {}

    template <typename T> T read()
    {
        static_assert(std::is_arithmetic_v<T> || std::is_enum_v<T>,
                      "ByteReader::read only handles arithmetic and enum types");
        using U = typename ByteStreamUnsigned<sizeof(T)>::type;
        auto raw = readBytes(sizeof(T));
        U bits = 0;
        for (std::size_t i = 0; i < sizeof(T); ++i)
            bits |= static_cast<U>(static_cast<U>(raw[i]) << (8 * i));
        if constexpr (std::is_enum_v<T>)
            return static_cast<T>(static_cast<std::underlying_type_t<T>>(bits));
        else if constexpr (std::is_same_v<T, bool>)
            return bits != 0;
        else
            return std::bit_cast<T>(bits);
    }

    std::string readString()
    {
        auto length = read<std::uint16_t>();
        auto raw = readBytes(length);
        std::string result;
        result.reserve(length);
        for (std::byte b : raw)
            result.push_back(static_cast<char>(b));
        return result;
    }

    std::span<const std::byte> readBytes(std::size_t count)
    {
        if (count > remaining())
            throw NetError("Read past the end of the buffer");
        auto result = _data.subspan(_offset, count);
        _offset += count;
        return result;
    }

    std::size_t remaining() const { return _data.size() - _offset; }

  private:
    std::span<const std::byte> _data;
    std::size_t _offset = 0;
};

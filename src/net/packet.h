// 协议数据包
#pragma once
#include <cstddef>
#include <cstring>
#include <vector>
#include <cstdint>
#include <string>

namespace net {
// 判断是否小端
inline bool is_little_endian() {
    static const uint16_t test = 0x1;
    return *reinterpret_cast<const uint8_t*>(&test) == 0x1;
}

// Host -> Network (截断 + 大端)
inline void to_network_bytes(uint64_t val, char* out, size_t size) {
    // 截断到 size 字节
    val &= ((1ULL << (size * 8)) - 1);

    if (is_little_endian()) {
        // 小端主机，需要反转成大端
        for (size_t i = 0; i < size; i++) {
            out[size - 1 - i] = static_cast<char>((val >> (i * 8)) & 0xFF);
        }
    } else {
        // 大端主机，直接顺序写
        for (size_t i = 0; i < size; i++) {
            out[i] = static_cast<char>((val >> ((size - 1 - i) * 8)) & 0xFF);
        }
    }
}

// Network -> Host (大端 -> 主机字节序)
inline uint64_t from_network_bytes(const char* in, size_t size) {
    uint64_t val = 0;
    if (is_little_endian()) {
        // 小端主机，需要翻转
        for (size_t i = 0; i < size; i++) {
            val |= (static_cast<uint64_t>(static_cast<unsigned char>(in[size - 1 - i])) << (i * 8));
        }
    } else {
        // 大端主机，直接拼接
        for (size_t i = 0; i < size; i++) {
            val = (val << 8) | static_cast<unsigned char>(in[i]);
        }
    }
    return val;
}
class PacketBuffer{
    size_t rpos_ = 0;
    size_t wpos_ = 0;
    std::vector<char> buffer_;
public:
    explicit PacketBuffer(size_t init_size = 1024): buffer_(init_size){};
    PacketBuffer(char* p, size_t size): buffer_(p, p + size){}
    void reserve(size_t n);

    void append(const void *data, size_t n);

    void read(void* out, size_t n);

    void peek(void* out, size_t n);

    void prepare();

    size_t readableBytes() const ;

    size_t writableBytes();
    const char* data() const;
};

class Packet {
    PacketBuffer buffer_;

public:
    explicit Packet(size_t init_size = 1024) : buffer_(init_size) {}
    Packet(char* p, size_t size): buffer_(p, size){}

    // 写整数（支持 1,2,4,8 字节）
    void addInt(long long val, size_t size);

    // 读整数
    long long unpackInt(size_t size);

    // 写浮点
    void addFloat(float v);

    // 读浮点
    float unpackFloat();

    // 写字符串（裸写，不加长度）
    void addString(const std::string& s);

    // 读字符串（指定长度）
    std::string unpackString(size_t len);

    // 写字节数组
    void addBytes(const void* data, size_t len);

    // 读字节数组
    std::vector<char> unpackBytes(size_t len);

    void prepare();

    // 获取剩余可读数据
    size_t size() const;
    const char* data() const;
};

void test_packet();
}


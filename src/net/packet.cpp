#include "packet.h"
#include <cstring>
#include <algorithm>
#include <cassert>
#include <cstddef>
#include <cstring>
#include <stdexcept>
#include <iostream>

namespace net{

void PacketBuffer::reserve(size_t n){
    if(wpos_ + n > buffer_.size()){
        buffer_.resize(std::max(wpos_ + n , buffer_.size() * 2));
    }
}

void PacketBuffer::append(const void *data, size_t n){
    reserve(n);
    std::memcpy(buffer_.data() + wpos_, data, n);
    wpos_ += n;
}

void PacketBuffer::read(void* out, size_t n){
    if(n + rpos_ > buffer_.size()){
        throw std::out_of_range("read overflow");
    }
    std::memcpy(out, buffer_.data() + rpos_, n);
    rpos_ += n;
}

void PacketBuffer::peek(void* out, size_t n){
    if(n + rpos_ > buffer_.size()){
        throw std::out_of_range("peek overflow");
    }
    std::memcpy(out, buffer_.data() + rpos_, n);
}

size_t PacketBuffer::readableBytes() const {
    return wpos_ - rpos_;
}

size_t PacketBuffer::writableBytes(){
    return buffer_.size() - wpos_;
}

const char* PacketBuffer::data() const {
    return buffer_.data();
}

void PacketBuffer::prepare(){
    wpos_ = rpos_ = 0;
}

// 写整数（支持 1,2,4,8 字节）
void Packet::addInt(long long val, size_t size) {
    char buf[8];
    to_network_bytes(static_cast<uint64_t>(val), buf, size);
    buffer_.append(buf, size);
}

// 读整数
long long Packet::unpackInt(size_t size) {
    char buf[8];
    buffer_.read(buf, size);
    return static_cast<long long>(from_network_bytes(buf, size));
}

// 写浮点
void Packet::addFloat(float v) {
    static_assert(sizeof(float) == 4);
    uint32_t tmp;
    std::memcpy(&tmp, &v, 4);
    char buf[4];
    to_network_bytes(tmp, buf, 4);
    buffer_.append(buf, 4);
}

// 读浮点
float Packet::unpackFloat() {
    char buf[4];
    buffer_.read(buf, 4);
    uint32_t tmp = static_cast<uint32_t>(from_network_bytes(buf, 4));
    float v;
    std::memcpy(&v, &tmp, 4);
    return v;
}

// 写字符串（裸写，不加长度）
void Packet::addString(const std::string& s) {
    buffer_.append(s.data(), s.size());
}

// 读字符串（指定长度）
std::string Packet::unpackString(size_t len) {
    std::string s(len, '\0');
    buffer_.read(s.data(), len);
    return s;
}

// 写字节数组
void Packet::addBytes(const void* data, size_t len) {
    buffer_.append(data, len);
}

// 读字节数组
std::vector<char> Packet::unpackBytes(size_t len) {
    std::vector<char> out(len);
    buffer_.read(out.data(), len);
    return out;
}

void Packet::prepare(){
    buffer_.prepare();
}

// 获取剩余可读数据
size_t Packet::size() const { return buffer_.readableBytes(); }
const char* Packet::data() const { return buffer_.data(); }



void test_packet(){
    Packet p;

    // int 测试
    p.addInt(123, 1);
    p.addInt(30000, 2);
    p.addInt(123456789, 4);
    p.addInt(0x1122334455667788LL, 8);

    // float
    p.addFloat(3.14f);

    // string
    p.addString("hello");
    p.addString(" world!");

    // 解包
    auto v1 = p.unpackInt(1);
    auto v2 = p.unpackInt(2);
    auto v3 = p.unpackInt(4);
    auto v4 = p.unpackInt(8);
    auto f1 = p.unpackFloat();
    auto s1 = p.unpackString(5);
    auto s2 = p.unpackString(7);

    std::cout << "解包结果:" << std::endl;
    std::cout << "Int(1B): " << v1 << std::endl;
    std::cout << "Int(2B): " << v2 << std::endl;
    std::cout << "Int(4B): " << v3 << std::endl;
    std::cout << "Int(8B): 0x" << std::hex << v4 << std::dec << std::endl;
    std::cout << "Float: " << f1 << std::endl;
    std::cout << "Str: " << s1 << s2 << std::endl;
}

};
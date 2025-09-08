#include "asio/detail/socket_ops.hpp"
#include "asio/read.hpp"
#include <asio.hpp>
#include <cstddef>
#include <cstring>
#include <iostream>
#include <memory>

namespace net::client {
using asio::ip::tcp;

    struct Client : std::enable_shared_from_this<Client> {
        asio::io_context& io;
        tcp::resolver resolver;
        tcp::socket socket;
        asio::steady_timer connect_timer;

        // 为了演示“首发”，我们把要发的内容放在成员里，确保生命周期足够长
        std::string first_payload = R"({"type":"login","name":"PlayerA","version":1})";
        std::array<unsigned char, 4> lenbuf{}; // 长度前缀（4字节，网络序）

        std::array<unsigned char,4> read_len{};
        std::vector<unsigned char> body;

        Client(asio::io_context& io_) 
            : io(io_), resolver(io_), socket(io_), connect_timer(io_) {}

        void start(const std::string& host, const std::string& service, int timeout_ms = 5000) {
            resolver.async_resolve(host, service,
                [self = shared_from_this(), timeout_ms](asio::error_code ec, tcp::resolver::results_type results) {
                    if (ec) return self->fail("resolve", ec);

                    // 启动连接超时
                    self->connect_timer.expires_after(std::chrono::milliseconds(timeout_ms));
                    self->connect_timer.async_wait([self](asio::error_code ec) {
                        if (!ec) {
                            // 超时：cancel/close 都可以，这里直接关闭
                            asio::error_code ignore;
                            self->socket.close(ignore);
                        }
                    });

                    asio::async_connect(self->socket, results,
                        [self](asio::error_code ec, const tcp::endpoint&) {
                            // 先取消计时器
                            self->connect_timer.cancel();

                            if (ec) return self->fail("connect", ec);

                            // 可选优化
                            self->socket.set_option(tcp::no_delay(true));
                            self->socket.set_option(asio::socket_base::keep_alive(true));

                            // 连接成功 → 首发：发送一个带长度前缀的数据帧
                            self->send_first_frame();
                            self->read();
                        });
                });
        }

        void send_first_frame() {
            // 组长度前缀（4字节大端：网络序）
            auto n = static_cast<uint32_t>(first_payload.size());
            // boost::endian::native_to_big_inplace(n);
            n = asio::detail::socket_ops::host_to_network_long(n);
            std::memcpy(lenbuf.data(), &n, 4);

            // 用两个 buffer 组成一帧：[len][body]
            std::array<asio::const_buffer, 2> bufs = {
                asio::buffer(lenbuf),
                asio::buffer(first_payload)
            };

            asio::async_write(socket, bufs,
                [self = shared_from_this()](asio::error_code ec, std::size_t /*bytes*/) {
                    if (ec) return self->fail("first write", ec);
                    std::cout << "First frame sent\n";
                    // → 后续可以继续 async_read / async_write ...
                });
        }

        void read(){
            auto self = shared_from_this();
            asio::async_read(socket, asio::buffer(read_len), [self](asio::error_code ec, std::size_t){
                if(ec){
                    self->fail("read", ec);
                    return;
                }
                auto n = 0;
                std::memcpy(&n, self->read_len.data(), 4);
                n = asio::detail::socket_ops::network_to_host_long(n);
                self->body.resize(n);
                self->_read_body();
            });
        }

        void _read_body(){
            auto self = shared_from_this();
            asio::async_read(socket, asio::buffer(body), [self](asio::error_code ec, std::size_t){
                if(ec){
                    self->fail("read_body", ec);
                    return;
                }
                char* msg = new char[self->body.size()];
                std::memcpy(msg, self->body.data(), self->body.size());
                std::cout << msg << std::endl;
                self->read();
            });
        }

        void fail(const char* where, asio::error_code ec) {
            std::cerr << "[Client] " << where << " error: " << ec.message() << "\n";
        }
    };

    int test() {
        asio::io_context io;
        std::make_shared<Client>(io)->start("127.0.0.1", "12345", 5000);
        io.run();
        return 0;
    }
}

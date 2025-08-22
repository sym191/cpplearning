#include "proactor.h"
#include "asio/error_code.hpp"
#include "asio/io_context.hpp"
#include "asio/ip/address_v4.hpp"
#include "asio/ip/detail/endpoint.hpp"
#include "asio/ip/tcp.hpp"
#include "spdlog/spdlog.h"

namespace proactor{
    void foo(const std::string& ip, const short& port){
        auto io_ct = asio::io_context();
        auto ec = asio::error_code();
        auto addr = asio::ip::make_address_v4(ip, ec);
        if(ec.value() != 0){
            spdlog::info("make addr err: {}", ec.message());
            return;
        }
        auto dp = asio::ip::tcp::endpoint(addr, port);
        auto sc = asio::ip::tcp::socket(io_ct);
        auto res =sc.connect(dp, ec);
        if(!res.value()){
            spdlog::info("connect err: {}", res.message());
            return;
        }
        
    }
}
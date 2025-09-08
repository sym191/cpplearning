#include "asio/io_context.hpp"
#include "multithread/async.h"
#include "net/net.h"
#include "ds/ds.h"
#include "util/util.h"
#include <string>
#include "multithread/multithread.h"
#include "spdlog/spdlog.h"
#include "asio.hpp"
#include "designpattern/dp.h"
#include "algorithm/algorithm.h"

int main(){
    // mythread::task::foo();
    // dp::composite::test();
    // hanota::test();
    // net::select::tcp_test(12345, "127.0.0.1");
    net::client::test();
}
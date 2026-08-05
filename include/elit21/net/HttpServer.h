#pragma once

#include "elit21/core/Result.h"

#include <atomic>
#include <cstddef>
#include <cstdint>
#include <functional>
#include <map>
#include <string>
#include <thread>

namespace elit21 {

struct IncomingRequest {
    std::string method, path, body;
    std::map<std::string, std::string> headers;
};

struct OutgoingResponse {
    int status{200};
    std::string content_type{"application/json"};
    std::string body{"{\"ok\":true}"};
};

class HttpServer {
public:
    using Handler = std::function<OutgoingResponse(const IncomingRequest&)>;

    HttpServer();
    ~HttpServer();
    Result<void> start(int port, Handler handler);
    void stop();
    bool running() const { return running_; }
    void setMaximumRequestBytes(std::size_t bytes) { maximum_request_bytes_ = bytes; }

private:
    void run(int port);
    Handler handler_;
    std::atomic_bool running_{false};
    std::thread thread_;
    std::intptr_t socket_{-1};
    std::size_t maximum_request_bytes_{2U * 1024U * 1024U};
};

} // namespace elit21

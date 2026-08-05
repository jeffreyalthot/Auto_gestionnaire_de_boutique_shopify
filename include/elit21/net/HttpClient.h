#pragma once

#include "elit21/core/Result.h"

#include <cstddef>
#include <map>
#include <string>

namespace elit21 {

struct HttpResponse {
    long status{0};
    std::string body;
    std::map<std::string, std::string> headers;
    double elapsed_seconds{0.0};
    std::string effective_url;
};

class HttpClient {
public:
    HttpClient();
    ~HttpClient();

    Result<HttpResponse> request(const std::string& method,
                                 const std::string& url,
                                 const std::string& body = {},
                                 const std::map<std::string, std::string>& headers = {},
                                 const std::string& content_type = {});
    Result<HttpResponse> get(const std::string& url,
                             const std::map<std::string, std::string>& headers = {});
    Result<HttpResponse> postJson(const std::string& url,
                                  const std::string& body,
                                  const std::map<std::string, std::string>& headers = {});
    Result<HttpResponse> postForm(const std::string& url,
                                  const std::map<std::string, std::string>& form,
                                  const std::map<std::string, std::string>& headers = {});

    void setTimeoutSeconds(long seconds);
    void setConnectTimeoutSeconds(long seconds);
    void setMaximumResponseBytes(std::size_t bytes);
    void setUserAgent(std::string user_agent);
    void setCaBundle(std::string path);
    void setProxy(std::string proxy_url);
    void setTlsVerification(bool enabled);

private:
    long timeout_{45};
    long connect_timeout_{10};
    std::size_t maximum_response_bytes_{16U * 1024U * 1024U};
    std::string user_agent_{"ELIT21-Shop-Manager/8.0"};
    std::string ca_bundle_;
    std::string proxy_url_;
    bool verify_tls_{true};
};

} // namespace elit21

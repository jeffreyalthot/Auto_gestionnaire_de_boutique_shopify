#include "elit21/net/HttpClient.h"

#include "elit21/util/StringUtil.h"

#include <curl/curl.h>

#include <algorithm>
#include <cctype>
#include <mutex>
#include <sstream>
#include <utility>

namespace elit21 {
namespace {

std::once_flag curl_init_flag;

struct ResponseBuffer {
    std::string* output{nullptr};
    std::size_t maximum_bytes{0};
    bool overflow{false};
};

size_t bodyCallback(char* pointer, size_t size, size_t count, void* user_data) {
    const auto bytes = size * count;
    auto* buffer = static_cast<ResponseBuffer*>(user_data);
    if (buffer == nullptr || buffer->output == nullptr) return 0;
    if (bytes > buffer->maximum_bytes || buffer->output->size() > buffer->maximum_bytes - bytes) {
        buffer->overflow = true;
        return 0;
    }
    buffer->output->append(pointer, bytes);
    return bytes;
}

size_t headerCallback(char* pointer, size_t size, size_t count, void* user_data) {
    const auto bytes = size * count;
    std::string line(pointer, bytes);
    const auto separator = line.find(':');
    if (separator != std::string::npos) {
        auto* headers = static_cast<std::map<std::string, std::string>*>(user_data);
        (*headers)[util::lower(util::trim(line.substr(0, separator)))] =
            util::trim(line.substr(separator + 1));
    }
    return bytes;
}

bool isSupportedMethod(const std::string& method) {
    return method == "GET" || method == "POST" || method == "PUT" || method == "PATCH" ||
           method == "DELETE" || method == "HEAD";
}

std::string normalizeMethod(std::string method) {
    std::transform(method.begin(), method.end(), method.begin(), [](unsigned char c) {
        return static_cast<char>(std::toupper(c));
    });
    return method;
}

} // namespace

HttpClient::HttpClient() {
    std::call_once(curl_init_flag, [] {
        curl_global_init(CURL_GLOBAL_DEFAULT);
    });
}

HttpClient::~HttpClient() = default;

void HttpClient::setTimeoutSeconds(long seconds) {
    timeout_ = std::max(1L, seconds);
}

void HttpClient::setConnectTimeoutSeconds(long seconds) {
    connect_timeout_ = std::max(1L, seconds);
}

void HttpClient::setMaximumResponseBytes(std::size_t bytes) {
    maximum_response_bytes_ = std::max<std::size_t>(1024, bytes);
}

void HttpClient::setUserAgent(std::string user_agent) {
    if (!user_agent.empty()) user_agent_ = std::move(user_agent);
}

void HttpClient::setCaBundle(std::string path) {
    ca_bundle_ = std::move(path);
}

void HttpClient::setProxy(std::string proxy_url) {
    proxy_url_ = std::move(proxy_url);
}

void HttpClient::setTlsVerification(bool enabled) {
    verify_tls_ = enabled;
}

Result<HttpResponse> HttpClient::get(const std::string& url,
                                     const std::map<std::string, std::string>& headers) {
    return request("GET", url, {}, headers, {});
}

Result<HttpResponse> HttpClient::postJson(const std::string& url,
                                          const std::string& body,
                                          const std::map<std::string, std::string>& headers) {
    return request("POST", url, body, headers, "application/json");
}

Result<HttpResponse> HttpClient::postForm(const std::string& url,
                                          const std::map<std::string, std::string>& form,
                                          const std::map<std::string, std::string>& headers) {
    std::ostringstream encoded;
    bool first = true;
    for (const auto& [key, value] : form) {
        if (!first) encoded << '&';
        first = false;
        encoded << util::urlEncode(key) << '=' << util::urlEncode(value);
    }
    return request("POST", url, encoded.str(), headers, "application/x-www-form-urlencoded");
}

Result<HttpResponse> HttpClient::request(const std::string& requested_method,
                                         const std::string& url,
                                         const std::string& body,
                                         const std::map<std::string, std::string>& headers,
                                         const std::string& content_type) {
    const auto method = normalizeMethod(requested_method);
    if (!isSupportedMethod(method)) {
        return Result<HttpResponse>::failure("Méthode HTTP non prise en charge: " + method);
    }
    if (url.rfind("https://", 0) != 0 && url.rfind("http://", 0) != 0) {
        return Result<HttpResponse>::failure("URL HTTP invalide: " + url);
    }

    CURL* curl = curl_easy_init();
    if (curl == nullptr) return Result<HttpResponse>::failure("curl_easy_init a échoué");

    HttpResponse response;
    ResponseBuffer response_buffer{&response.body, maximum_response_bytes_, false};
    curl_slist* header_list = nullptr;

    curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
    curl_easy_setopt(curl, CURLOPT_NOSIGNAL, 1L);
    curl_easy_setopt(curl, CURLOPT_FOLLOWLOCATION, 1L);
    curl_easy_setopt(curl, CURLOPT_MAXREDIRS, 5L);
    curl_easy_setopt(curl, CURLOPT_TIMEOUT, timeout_);
    curl_easy_setopt(curl, CURLOPT_CONNECTTIMEOUT, connect_timeout_);
    curl_easy_setopt(curl, CURLOPT_LOW_SPEED_LIMIT, 32L);
    curl_easy_setopt(curl, CURLOPT_LOW_SPEED_TIME, 20L);
    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, bodyCallback);
    curl_easy_setopt(curl, CURLOPT_WRITEDATA, &response_buffer);
    curl_easy_setopt(curl, CURLOPT_HEADERFUNCTION, headerCallback);
    curl_easy_setopt(curl, CURLOPT_HEADERDATA, &response.headers);
    curl_easy_setopt(curl, CURLOPT_USERAGENT, user_agent_.c_str());
    curl_easy_setopt(curl, CURLOPT_ACCEPT_ENCODING, "");
    curl_easy_setopt(curl, CURLOPT_SSL_VERIFYPEER, verify_tls_ ? 1L : 0L);
    curl_easy_setopt(curl, CURLOPT_SSL_VERIFYHOST, verify_tls_ ? 2L : 0L);

    if (!ca_bundle_.empty()) curl_easy_setopt(curl, CURLOPT_CAINFO, ca_bundle_.c_str());
    if (!proxy_url_.empty()) curl_easy_setopt(curl, CURLOPT_PROXY, proxy_url_.c_str());

    if (!content_type.empty()) {
        header_list = curl_slist_append(header_list, ("Content-Type: " + content_type).c_str());
    }
    header_list = curl_slist_append(header_list, "Accept: application/json");
    for (const auto& [key, value] : headers) {
        header_list = curl_slist_append(header_list, (key + ": " + value).c_str());
    }
    if (header_list != nullptr) curl_easy_setopt(curl, CURLOPT_HTTPHEADER, header_list);

    if (method == "HEAD") {
        curl_easy_setopt(curl, CURLOPT_NOBODY, 1L);
    } else if (method != "GET") {
        curl_easy_setopt(curl, CURLOPT_CUSTOMREQUEST, method.c_str());
        curl_easy_setopt(curl, CURLOPT_POSTFIELDS, body.data());
        curl_easy_setopt(curl, CURLOPT_POSTFIELDSIZE_LARGE, static_cast<curl_off_t>(body.size()));
    }

    const auto curl_code = curl_easy_perform(curl);
    if (curl_code == CURLE_OK) {
        curl_easy_getinfo(curl, CURLINFO_RESPONSE_CODE, &response.status);
        curl_easy_getinfo(curl, CURLINFO_TOTAL_TIME, &response.elapsed_seconds);
        char* effective_url = nullptr;
        curl_easy_getinfo(curl, CURLINFO_EFFECTIVE_URL, &effective_url);
        if (effective_url != nullptr) response.effective_url = effective_url;
    }

    if (header_list != nullptr) curl_slist_free_all(header_list);
    curl_easy_cleanup(curl);

    if (response_buffer.overflow) {
        return Result<HttpResponse>::failure("Réponse HTTP supérieure à la limite configurée");
    }
    if (curl_code != CURLE_OK) {
        return Result<HttpResponse>::failure(curl_easy_strerror(curl_code));
    }
    return Result<HttpResponse>::success(std::move(response));
}

} // namespace elit21

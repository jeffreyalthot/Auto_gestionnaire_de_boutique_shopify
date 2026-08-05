#include "elit21/net/HttpServer.h"
#include "elit21/util/StringUtil.h"

#include <algorithm>
#include <cstring>
#include <sstream>

#ifdef _WIN32
#include <winsock2.h>
#include <ws2tcpip.h>
using Sock = SOCKET;
static constexpr Sock bad_socket = INVALID_SOCKET;
#else
#include <arpa/inet.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <sys/time.h>
#include <unistd.h>
using Sock = int;
static constexpr Sock bad_socket = -1;
#endif

namespace elit21 {
namespace {

void closeSocket(Sock socket) {
#ifdef _WIN32
    closesocket(socket);
#else
    close(socket);
#endif
}

std::string reason(int status) {
    switch (status) {
        case 200: return "OK";
        case 202: return "Accepted";
        case 400: return "Bad Request";
        case 401: return "Unauthorized";
        case 403: return "Forbidden";
        case 404: return "Not Found";
        case 405: return "Method Not Allowed";
        case 413: return "Payload Too Large";
        case 500: return "Internal Server Error";
        case 503: return "Service Unavailable";
        default: return "Response";
    }
}

void setReceiveTimeout(Sock socket) {
#ifdef _WIN32
    DWORD timeout = 10000;
    setsockopt(socket, SOL_SOCKET, SO_RCVTIMEO, reinterpret_cast<const char*>(&timeout), sizeof(timeout));
#else
    timeval timeout{10, 0};
    setsockopt(socket, SOL_SOCKET, SO_RCVTIMEO, &timeout, sizeof(timeout));
#endif
}

OutgoingResponse errorResponse(int status, const std::string& code) {
    return {status, "application/json", "{\"error\":\"" + code + "\"}"};
}

} // namespace

HttpServer::HttpServer() {
#ifdef _WIN32
    WSADATA data{};
    WSAStartup(MAKEWORD(2, 2), &data);
#endif
}

HttpServer::~HttpServer() {
    stop();
#ifdef _WIN32
    WSACleanup();
#endif
}

Result<void> HttpServer::start(int port, Handler handler) {
    if (port < 1 || port > 65535) return Result<void>::failure("Port HTTP invalide");
    if (!handler) return Result<void>::failure("Gestionnaire HTTP manquant");
    if (running_) return Result<void>::failure("Serveur déjà actif");
    handler_ = std::move(handler);
    running_ = true;
    thread_ = std::thread([this, port] { run(port); });
    return Result<void>::success();
}

void HttpServer::stop() {
    running_ = false;
    if (socket_ != -1) {
#ifdef _WIN32
        shutdown(static_cast<Sock>(socket_), SD_BOTH);
#else
        shutdown(static_cast<Sock>(socket_), SHUT_RDWR);
#endif
        closeSocket(static_cast<Sock>(socket_));
        socket_ = -1;
    }
    if (thread_.joinable()) thread_.join();
}

void HttpServer::run(int port) {
    Sock listen_socket = ::socket(AF_INET, SOCK_STREAM, 0);
    if (listen_socket == bad_socket) { running_ = false; return; }
    socket_ = static_cast<std::intptr_t>(listen_socket);
    int reuse = 1;
    setsockopt(listen_socket, SOL_SOCKET, SO_REUSEADDR,
               reinterpret_cast<const char*>(&reuse), sizeof(reuse));

    sockaddr_in address{};
    address.sin_family = AF_INET;
    address.sin_addr.s_addr = htonl(INADDR_ANY);
    address.sin_port = htons(static_cast<unsigned short>(port));
    if (bind(listen_socket, reinterpret_cast<sockaddr*>(&address), sizeof(address)) < 0 ||
        listen(listen_socket, 32) < 0) {
        closeSocket(listen_socket);
        socket_ = -1;
        running_ = false;
        return;
    }

    while (running_) {
        sockaddr_in client_address{};
#ifdef _WIN32
        int client_size = sizeof(client_address);
#else
        socklen_t client_size = sizeof(client_address);
#endif
        Sock client = accept(listen_socket, reinterpret_cast<sockaddr*>(&client_address), &client_size);
        if (client == bad_socket) {
            if (!running_) break;
            continue;
        }
        setReceiveTimeout(client);

        std::string raw;
        raw.reserve(8192);
        char buffer[8192];
        std::size_t header_end = std::string::npos;
        std::size_t content_length = 0;
        bool malformed = false;
        bool too_large = false;

        while (raw.size() <= maximum_request_bytes_) {
#ifdef _WIN32
            const int received = recv(client, buffer, static_cast<int>(sizeof(buffer)), 0);
#else
            const ssize_t received = recv(client, buffer, sizeof(buffer), 0);
#endif
            if (received <= 0) break;
            raw.append(buffer, static_cast<std::size_t>(received));
            if (raw.size() > maximum_request_bytes_) { too_large = true; break; }

            if (header_end == std::string::npos) {
                header_end = raw.find("\r\n\r\n");
                if (header_end != std::string::npos) {
                    if (header_end > 64U * 1024U) { too_large = true; break; }
                    const std::string header_text = raw.substr(0, header_end);
                    const std::string lowered = util::lower(header_text);
                    const auto position = lowered.find("content-length:");
                    if (position != std::string::npos) {
                        const auto end = header_text.find("\r\n", position);
                        try {
                            content_length = static_cast<std::size_t>(std::stoull(
                                util::trim(header_text.substr(position + 15, end - position - 15))));
                        } catch (...) {
                            malformed = true;
                            break;
                        }
                        if (content_length > maximum_request_bytes_) { too_large = true; break; }
                    }
                }
            }
            if (header_end != std::string::npos && raw.size() >= header_end + 4U + content_length) break;
        }

        IncomingRequest request;
        OutgoingResponse response;
        if (too_large) {
            response = errorResponse(413, "payload_too_large");
        } else if (malformed) {
            response = errorResponse(400, "malformed_request");
        } else {
            const auto split = raw.find("\r\n\r\n");
            if (split == std::string::npos) {
                response = errorResponse(400, "incomplete_request");
            } else {
                std::istringstream headers(raw.substr(0, split));
                std::string line;
                std::getline(headers, line);
                if (!line.empty() && line.back() == '\r') line.pop_back();
                std::string protocol;
                std::istringstream first_line(line);
                first_line >> request.method >> request.path >> protocol;
                while (std::getline(headers, line)) {
                    if (!line.empty() && line.back() == '\r') line.pop_back();
                    const auto colon = line.find(':');
                    if (colon != std::string::npos) {
                        request.headers[util::lower(util::trim(line.substr(0, colon)))] =
                            util::trim(line.substr(colon + 1));
                    }
                }
                request.body = raw.substr(split + 4U, content_length == 0 ? std::string::npos : content_length);
                if (request.method.empty() || request.path.empty() || protocol.rfind("HTTP/", 0) != 0) {
                    response = errorResponse(400, "malformed_request_line");
                } else {
                    try {
                        response = handler_ ? handler_(request) : errorResponse(404, "not_found");
                    } catch (const std::exception&) {
                        response = errorResponse(500, "handler_exception");
                    } catch (...) {
                        response = errorResponse(500, "handler_exception");
                    }
                }
            }
        }

        std::ostringstream output;
        output << "HTTP/1.1 " << response.status << ' ' << reason(response.status)
               << "\r\nContent-Type: " << response.content_type
               << "\r\nContent-Length: " << response.body.size()
               << "\r\nConnection: close"
               << "\r\nCache-Control: no-store"
               << "\r\nX-Content-Type-Options: nosniff"
               << "\r\nX-Frame-Options: DENY\r\n\r\n"
               << response.body;
        const auto text = output.str();
#ifdef _WIN32
        send(client, text.data(), static_cast<int>(text.size()), 0);
#else
        send(client, text.data(), text.size(), 0);
#endif
        closeSocket(client);
    }

    if (socket_ != -1) {
        closeSocket(listen_socket);
        socket_ = -1;
    }
    running_ = false;
}

} // namespace elit21

#include "elit21/logging/Logger.h"
#include "elit21/security/DataRedactor.h"
#include "elit21/util/TimeUtil.h"

#include <algorithm>
#include <filesystem>

namespace elit21 {
Logger::Logger(std::string path) {
    const auto parent = std::filesystem::path(path).parent_path();
    if (!parent.empty()) std::filesystem::create_directories(parent);
    file_.open(path, std::ios::app);
}
Logger::~Logger() { std::lock_guard lock(mutex_); if (file_) file_.flush(); }
std::string Logger::name(LogLevel level) const {
    switch (level) {
        case LogLevel::Debug: return "DEBUG";
        case LogLevel::Info: return "INFO";
        case LogLevel::Warning: return "WARNING";
        case LogLevel::Error: return "ERROR";
        case LogLevel::Critical: return "CRITICAL";
    }
    return "UNKNOWN";
}
void Logger::log(LogLevel level, const std::string& category, const std::string& message) {
    const std::string safe_category = security::DataRedactor::redact(category);
    const std::string safe_message = security::DataRedactor::redact(message);
    const std::string line = util::localNowDisplay() + " [" + name(level) + "] [" + safe_category + "] " + safe_message;
    std::lock_guard lock(mutex_);
    if (file_) { file_ << line << '\n'; file_.flush(); }
    recent_.push_back(line);
    while (recent_.size() > 500) recent_.pop_front();
}
std::vector<std::string> Logger::recent(std::size_t count) const {
    std::lock_guard lock(mutex_);
    count = std::min(count, recent_.size());
    return std::vector<std::string>(recent_.end() - static_cast<std::ptrdiff_t>(count), recent_.end());
}
}

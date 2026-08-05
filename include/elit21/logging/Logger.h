#pragma once
#include <deque>
#include <fstream>
#include <mutex>
#include <string>
#include <vector>
namespace elit21 {
enum class LogLevel{Debug,Info,Warning,Error,Critical};
class Logger {
public:
 explicit Logger(std::string file_path); ~Logger();
 void log(LogLevel level,const std::string& category,const std::string& message);
 void debug(const std::string& c,const std::string& m){log(LogLevel::Debug,c,m);} void info(const std::string& c,const std::string& m){log(LogLevel::Info,c,m);} void warning(const std::string& c,const std::string& m){log(LogLevel::Warning,c,m);} void error(const std::string& c,const std::string& m){log(LogLevel::Error,c,m);} void critical(const std::string& c,const std::string& m){log(LogLevel::Critical,c,m);}
 std::vector<std::string> recent(std::size_t count) const;
private: std::string name(LogLevel)const; mutable std::mutex mutex_; std::ofstream file_; std::deque<std::string> recent_;};
}

#pragma once

#include "elit21/core/Result.h"

#include <filesystem>
#include <map>
#include <string>
#include <vector>

namespace elit21::progress {

struct FileProgress {
    std::filesystem::path relative_path;
    int percentage{0};
    std::string emoji;
    std::string state;
    std::string detail;
    std::uintmax_t bytes{0};
    std::size_t lines{0};
};

struct ProjectProgressSummary {
    std::size_t files{0};
    std::size_t directories{0};
    std::size_t cpp_files{0};
    std::size_t header_files{0};
    std::size_t test_files{0};
    std::size_t empty_files{0};
    double weighted_percentage{0.0};
    double source_percentage{0.0};
    double shopify_percentage{0.0};
    double test_percentage{0.0};
    double configuration_percentage{0.0};
    std::map<std::string, double> category_percentages;
    std::vector<FileProgress> entries;
};

class ProjectProgressAnalyzer {
public:
    Result<ProjectProgressSummary> analyze(const std::filesystem::path& root) const;
    Result<void> writeTree(const std::filesystem::path& root,
                           const ProjectProgressSummary& summary,
                           const std::filesystem::path& output) const;
    Result<void> writeCsv(const ProjectProgressSummary& summary,
                          const std::filesystem::path& output) const;
    Result<void> writeJson(const ProjectProgressSummary& summary,
                           const std::filesystem::path& output) const;

private:
    static FileProgress scoreFile(const std::filesystem::path& root,
                                  const std::filesystem::path& file);
};

} // namespace elit21::progress

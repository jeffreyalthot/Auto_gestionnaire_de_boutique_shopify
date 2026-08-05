#include "elit21/progress/ProjectProgressAnalyzer.h"

#include <filesystem>
#include <iomanip>
#include <iostream>

int main(int argc, char** argv) {
    const std::filesystem::path root = argc > 1 ? argv[1] : std::filesystem::current_path();
    const std::filesystem::path output = argc > 2 ? argv[2] : root / "PROJECT_TREE_CMD_EMOJI_CURRENT.txt";
    elit21::progress::ProjectProgressAnalyzer analyzer;
    auto summary = analyzer.analyze(root);
    if (!summary) {
        std::cerr << summary.error() << '\n';
        return 2;
    }
    if (auto result = analyzer.writeTree(root, summary.value(), output); !result) {
        std::cerr << result.error() << '\n';
        return 3;
    }
    analyzer.writeCsv(summary.value(), root / "PROJECT_FILE_STATUS_CURRENT.csv");
    analyzer.writeJson(summary.value(), root / "PROJECT_STATUS_CURRENT.json");
    std::cout << "files=" << summary.value().files
              << " directories=" << summary.value().directories
              << " progress=" << std::fixed << std::setprecision(1)
              << summary.value().weighted_percentage << "%\n"
              << "tree=" << output.string() << '\n';
    return summary.value().empty_files == 0 ? 0 : 4;
}

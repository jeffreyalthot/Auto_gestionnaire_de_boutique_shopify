#include "elit21/config/Config.h"
#include "elit21/security/Crypto.h"
#include "elit21/util/TimeUtil.h"

#include <filesystem>
#include <fstream>
#include <iostream>
#include <sqlite3.h>
#include <string>

namespace {
std::string safeTimestamp() {
    auto value = elit21::util::utcNowIso();
    for (char& character : value) if (character == ':' || character == '-') character = '_';
    return value;
}

bool integrityCheck(sqlite3* database, std::string& error) {
    sqlite3_stmt* statement = nullptr;
    if (sqlite3_prepare_v2(database, "PRAGMA integrity_check", -1, &statement, nullptr) != SQLITE_OK) {
        error = sqlite3_errmsg(database);
        return false;
    }
    const bool ok = sqlite3_step(statement) == SQLITE_ROW &&
        std::string(reinterpret_cast<const char*>(sqlite3_column_text(statement, 0))) == "ok";
    if (!ok) error = "PRAGMA integrity_check a echoue";
    sqlite3_finalize(statement);
    return ok;
}
}

int main(int argc, char** argv) {
    const std::string config_path = argc > 1 ? argv[1] : "config/app.json";
    auto loaded = elit21::Config::load(config_path);
    if (!loaded) { std::cerr << loaded.error() << '\n'; return 2; }
    namespace fs = std::filesystem;
    const fs::path source = loaded.value().app.database;
    if (!fs::exists(source)) { std::cerr << "Base introuvable: " << source << '\n'; return 3; }
    const fs::path directory = argc > 2 ? argv[2] : "data/backups";
    fs::create_directories(directory);
    const fs::path target = directory / ("elit21_" + safeTimestamp() + ".db");

    sqlite3* source_db = nullptr;
    sqlite3* target_db = nullptr;
    if (sqlite3_open_v2(source.string().c_str(), &source_db, SQLITE_OPEN_READONLY, nullptr) != SQLITE_OK ||
        sqlite3_open(target.string().c_str(), &target_db) != SQLITE_OK) {
        std::cerr << "Ouverture SQLite impossible\n";
        if (source_db) sqlite3_close(source_db);
        if (target_db) sqlite3_close(target_db);
        return 4;
    }
    sqlite3_backup* backup = sqlite3_backup_init(target_db, "main", source_db, "main");
    if (!backup) {
        std::cerr << sqlite3_errmsg(target_db) << '\n';
        sqlite3_close(source_db); sqlite3_close(target_db); return 5;
    }
    int code = SQLITE_OK;
    do { code = sqlite3_backup_step(backup, 128); if (code == SQLITE_BUSY || code == SQLITE_LOCKED) sqlite3_sleep(50); }
    while (code == SQLITE_OK || code == SQLITE_BUSY || code == SQLITE_LOCKED);
    const int finish_code = sqlite3_backup_finish(backup);
    std::string integrity_error;
    const bool integrity = code == SQLITE_DONE && finish_code == SQLITE_OK && integrityCheck(target_db, integrity_error);
    sqlite3_close(source_db);
    sqlite3_close(target_db);
    if (!integrity) {
        std::cerr << "Sauvegarde invalide: " << integrity_error << '\n';
        std::error_code ignored; fs::remove(target, ignored); return 6;
    }
    std::ifstream input(target, std::ios::binary);
    const std::string bytes((std::istreambuf_iterator<char>(input)), std::istreambuf_iterator<char>());
    const auto digest = elit21::crypto::sha256Hex(bytes);
    std::ofstream checksum(target.string() + ".sha256.txt");
    checksum << digest << "  " << target.filename().string() << '\n';
    std::cout << "backup=" << target.string() << '\n'
              << "bytes=" << fs::file_size(target) << '\n'
              << "sha256=" << digest << '\n'
              << "integrity=ok\n";
    return 0;
}

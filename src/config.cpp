#include "brkchrd/config.hpp"

#include <charconv>
#include <cmath>
#include <cstdio>
#include <cstring>
#include <fstream>
#include <sstream>
#include <system_error>

#if !defined(_WIN32)
#include <fcntl.h>
#include <unistd.h>
#endif

namespace brkchrd {
namespace {

void set_error(std::string* target, std::string message) {
    if (target != nullptr) *target = std::move(message);
}

std::filesystem::path parent_or_current(const std::filesystem::path& path) {
    return path.has_parent_path() ? path.parent_path() : std::filesystem::path{"."};
}

} // namespace

ConfigReadResult read_numeric_config(const std::filesystem::path& path) {
    ConfigReadResult result;
    std::ifstream input(path);
    if (!input) {
        std::error_code ec;
        if (std::filesystem::exists(path, ec)) {
            result.warnings.push_back("cannot open config: " + path.string());
        }
        return result;
    }

    std::string line;
    std::size_t line_number = 0U;
    while (std::getline(input, line)) {
        ++line_number;
        if (line.empty()) continue;

        std::istringstream stream(line);
        std::string key;
        float value = 0.0F;
        std::string trailing;
        if (!(stream >> key >> value) || (stream >> trailing) || !std::isfinite(value)) {
            result.warnings.push_back("ignored malformed config line " + std::to_string(line_number));
            continue;
        }
        result.entries.push_back({std::move(key), value, line_number});
    }
    return result;
}

std::optional<int> parse_indexed_key(std::string_view key,
                                     std::string_view prefix,
                                     int maximum_exclusive) {
    if (maximum_exclusive <= 0 || key.size() <= prefix.size() || key.substr(0, prefix.size()) != prefix) {
        return std::nullopt;
    }

    const std::string_view suffix = key.substr(prefix.size());
    int value = 0;
    const char* begin = suffix.data();
    const char* end = suffix.data() + suffix.size();
    const auto parsed = std::from_chars(begin, end, value);
    if (parsed.ec != std::errc{} || parsed.ptr != end || value < 0 || value >= maximum_exclusive) {
        return std::nullopt;
    }
    return value;
}

bool atomic_write_text(const std::filesystem::path& path,
                       std::string_view contents,
                       std::string* error) {
    const std::filesystem::path parent = parent_or_current(path);
    std::error_code ec;
    std::filesystem::create_directories(parent, ec);
    if (ec) {
        set_error(error, "cannot create config directory: " + ec.message());
        return false;
    }

    std::filesystem::path temporary = path;
    temporary += ".tmp";
    std::filesystem::remove(temporary, ec);
    ec.clear();

    std::FILE* file = std::fopen(temporary.string().c_str(), "wb");
    if (file == nullptr) {
        set_error(error, "cannot open temporary config: " + std::string(std::strerror(errno)));
        return false;
    }

    const std::size_t written = std::fwrite(contents.data(), 1U, contents.size(), file);
    bool ok = written == contents.size() && std::fflush(file) == 0;
#if !defined(_WIN32)
    if (ok) ok = ::fsync(::fileno(file)) == 0;
#endif
    if (std::fclose(file) != 0) ok = false;

    if (!ok) {
        std::filesystem::remove(temporary, ec);
        set_error(error, "cannot flush temporary config");
        return false;
    }

    if (std::rename(temporary.string().c_str(), path.string().c_str()) != 0) {
        const std::string message = std::strerror(errno);
        std::filesystem::remove(temporary, ec);
        set_error(error, "cannot replace config: " + message);
        return false;
    }

#if !defined(_WIN32)
    const int directory = ::open(parent.string().c_str(), O_RDONLY | O_DIRECTORY);
    if (directory >= 0) {
        static_cast<void>(::fsync(directory));
        static_cast<void>(::close(directory));
    }
#endif

    return true;
}

} // namespace brkchrd

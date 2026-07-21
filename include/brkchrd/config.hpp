#pragma once

#include <filesystem>
#include <optional>
#include <string>
#include <string_view>
#include <vector>

namespace brkchrd {

struct ConfigEntry {
    std::string key;
    float value = 0.0F;
    std::size_t line = 0U;
};

struct ConfigReadResult {
    std::vector<ConfigEntry> entries;
    std::vector<std::string> warnings;
};

ConfigReadResult read_numeric_config(const std::filesystem::path& path);

std::optional<int> parse_indexed_key(std::string_view key,
                                     std::string_view prefix,
                                     int maximum_exclusive);

bool atomic_write_text(const std::filesystem::path& path,
                       std::string_view contents,
                       std::string* error = nullptr);

} // namespace brkchrd

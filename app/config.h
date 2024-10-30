#pragma once
#include "domain/config.h"

#include <filesystem>
#include <expected>

std::filesystem::path getConfigPath();
domain::Config getDefaultConfig();
std::expected<domain::Config, std::string> loadConfig(const std::filesystem::path& config_filepath);
std::expected<void, std::string> validateConfig(const domain::Config& config);
std::expected<void, std::string> saveConfig(const domain::Config& config, const std::filesystem::path& path);

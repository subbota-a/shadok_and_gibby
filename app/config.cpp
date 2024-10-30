#include "config.h"

#include "paths/paths.h"

#define TOML_EXCEPTIONS 0
#include <toml++/toml.hpp>

std::filesystem::path getConfigPath()
{
    return paths::getAppConfigPath() / PROJECT_NAME ".toml";
}

domain::Config getDefaultConfig()
{
    return domain::Config{
        .field_size = {18, 18},
        .number_of_enemies = 5,
        .number_of_flowers = 15,
        .flower_scores_range = {5, 10},
        .max_player_steps = 100,
        .min_player_scores = 100,
    };
}

std::expected<domain::Config, std::string> loadConfig(const std::filesystem::path& config_filepath)
{
    toml::parse_result result = toml::parse_file(config_filepath.c_str());
    if (!result) {
        return std::unexpected(std::format(
            "Configuration file '{}' parsing failed: {}",
            config_filepath.string(),
            result.error().description()));
    } else {
        auto& table = result.table();
        domain::Config config;
        config.field_size[0] = table["field_width"].value_or(config.field_size[0]);
        config.field_size[1] = table["field_height"].value_or(config.field_size[1]);
        config.number_of_enemies = table["number_of_enemies"].value_or(config.number_of_enemies);
        config.number_of_flowers = table["number_of_flowers"].value_or(config.number_of_flowers);
        config.flower_scores_range.first = table["flower_scores_min"].value_or(config.flower_scores_range.first);
        config.flower_scores_range.second = table["flower_scores_max"].value_or(config.flower_scores_range.second);
        config.max_player_steps = table["max_player_steps"].value_or(config.max_player_steps);
        config.min_player_scores = table["min_player_scores"].value_or(config.min_player_scores);
        return config;
    }
}

std::expected<void, std::string> validateConfig(const domain::Config& config)
{
    if (config.flower_scores_range.first >= config.flower_scores_range.second) {
        return std::unexpected("Invalid flowers scores range, flower_scores_min < flower_scores_max expected.");
    }
    return {};
}

std::expected<void, std::string> saveConfig(const domain::Config& config, const std::filesystem::path& path)
{
    auto tbl = toml::table{
        {"field_width", config.field_size[0]},
        {"field_height", config.field_size[1]},
        {"number_of_enemies", config.number_of_enemies},
        {"number_of_flowers", config.number_of_flowers},
        {"flower_scores_min", config.flower_scores_range.first},
        {"flower_scores_max", config.flower_scores_range.second},
        {"max_player_steps", config.max_player_steps},
        {"min_player_scores", config.min_player_scores},
    };
    std::ofstream out;
    out.open(path, std::ios::out);
    if (!out) {
        return std::unexpected(std::format("Failed to create config file '{}'", path.string()));
    } else {
        out << "# Shadok and Gibby config\n\n";
        out << tbl << std::endl;
    }
    return {};
}

#pragma once
#include <filesystem>
namespace paths {

std::filesystem::path getAppDataPath();
std::filesystem::path getAppConfigPath();

}

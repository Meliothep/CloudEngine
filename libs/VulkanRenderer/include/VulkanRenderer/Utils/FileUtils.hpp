#pragma once

#include <string>
#include <vector>
#include <filesystem>

std::filesystem::path GetExecutableDir();

std::vector<char> ReadFile(const std::string& filename);


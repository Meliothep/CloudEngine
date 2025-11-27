#pragma once

#include <string>
#include <vector>
#include <filesystem>

namespace VulkanRenderer::Utils {

// Returns the directory containing the current executable. Falls back to
// current working directory on failure.
std::filesystem::path GetExecutableDir();

// Reads a binary file into a byte vector. Throws on failure.
std::vector<char> ReadFile(const std::string& filename);

} // namespace VulkanRenderer::Utils

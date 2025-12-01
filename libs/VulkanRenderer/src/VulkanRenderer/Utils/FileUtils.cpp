#include "VulkanRenderer/Utils/FileUtils.hpp"

#include <fstream>
#include <system_error>

#ifdef _WIN32
#  include <windows.h>
#else
#  include <unistd.h>
#  include <limits.h>
#endif

std::filesystem::path GetExecutableDir() {
    namespace fs = std::filesystem;
#ifdef _WIN32
    std::wstring buf;
    buf.resize(MAX_PATH);
    DWORD len = GetModuleFileNameW(NULL, buf.data(), static_cast<DWORD>(buf.size()));
    if (len == 0) {
        return fs::current_path();
    }
    buf.resize(len);
    return fs::path(buf).parent_path();
#else
    char buf[PATH_MAX];
    ssize_t len = readlink("/proc/self/exe", buf, sizeof(buf) - 1);
    if (len == -1) {
        return fs::current_path();
    }
    buf[len] = '\0';
    return fs::path(buf).parent_path();
#endif
}

std::vector<char> ReadFile(const std::string& filename) {
    std::ifstream file(filename, std::ios::ate | std::ios::binary);
    if (!file.is_open()) {
        throw std::system_error(errno, std::system_category(), "Failed to open file: " + filename);
    }

    size_t fileSize = static_cast<size_t>(file.tellg());
    std::vector<char> buffer(fileSize);

    file.seekg(0);
    file.read(buffer.data(), static_cast<std::streamsize>(fileSize));
    file.close();

    return buffer;
}


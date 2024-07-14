#ifndef CORE_H
#define CORE_H

#include <filesystem>

bool is_bmp(const std::filesystem::path &i_filePath);
bool is_barch(const std::filesystem::path &i_filePath);

std::filesystem::path encode(const std::filesystem::path &i_file_path);
std::filesystem::path decode(const std::filesystem::path &i_file_path);

#endif // CORE_H

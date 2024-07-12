#ifndef CORE_H
#define CORE_H

#include <filesystem>

bool is_bmp(const std::filesystem::path &i_filePath);
bool is_barch(const std::filesystem::path &i_filePath);

std::wstring encode(const std::wstring &i_file_path);
std::wstring decode(const std::wstring &i_file_path);

#endif // CORE_H

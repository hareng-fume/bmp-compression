#ifndef BMP_H
#define BMP_H

#include <filesystem>

struct RawImageData;
struct CompressedImageData;

namespace bmp {

bool read(const std::filesystem::path &i_file_path, RawImageData &o_data);
bool write(const std::filesystem::path &i_file_path, const RawImageData &i_data);

void compress(const RawImageData &i_data, CompressedImageData &o_compressed_data);

} // namespace bmp

#endif // BMP_H

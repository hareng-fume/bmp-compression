#ifndef BARCH_H
#define BARCH_H

#include <string>

struct RawImageData;
struct CompressedImageData;

namespace barch {

bool read(const std::wstring &i_file_path, CompressedImageData &o_data);
bool write(const std::wstring &i_file_path, const CompressedImageData &i_data);

void decompress(const CompressedImageData &i_compressed_data, RawImageData &o_data);

} // namespace barch

#endif // BARCH_H

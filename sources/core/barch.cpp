#include "barch.h"
#include "image.h"
#include "utils.h"

#include <cstring>
#include <fstream>

namespace {

//-----------------------------------------------------------------------------
inline constexpr bool _0 = false;
inline constexpr std::uint16_t _BA = 0x4142;

/* PoketBook suggested compression image format with some extensions:
        - format identifier      / std::uint16_t - BA (save these two letters in binary at the very beginning of saving compressed data)
        - image width            / std::int32_t
        - image height           / std::int32_t
        - compressed data offset / std::uint32_t
        - compressed data size   / std::int32_t
        - index data
        - compressed data
     */

#pragma pack(push, 1)
struct BarchHeader
{
    std::uint16_t file_type;
    std::int32_t width;
    std::int32_t height;
    std::uint32_t offset_data;
    std::uint32_t data_size;
};
#pragma pack(pop)

} // namespace

namespace barch {

//-----------------------------------------------------------------------------
bool read(const std::filesystem::path &i_file_path, CompressedImageData &o_data)
{
    std::ifstream ifs(i_file_path, std::ios::binary);
    if (!ifs)
        return false;

    BarchHeader header;
    ifs.read(reinterpret_cast<char *>(&header), sizeof(header));

    if (header.file_type != 0x4142) // not a barch file
        return false;

    const auto index_size = header.offset_data - sizeof(header);
    std::vector<unsigned char> index_data(index_size, 0);
    ifs.read(reinterpret_cast<char *>(index_data.data()), index_size);

    std::vector<unsigned char> compressed_data(header.data_size, 0);
    ifs.read(reinterpret_cast<char *>(compressed_data.data()), header.data_size);

    o_data.width = header.width;
    o_data.height = header.height;
    o_data.index = utils::_get_bits(index_data);
    o_data.data = std::move(compressed_data);

    return true;
}

//-----------------------------------------------------------------------------
bool write(const std::filesystem::path &i_file_path, const CompressedImageData &i_data)
{
    std::ofstream ofs(i_file_path, std::ios::binary);
    if (!ofs)
        return false;

    const auto index_data = utils::_get_bytes(i_data.index);
    const auto index_data_size = index_data.size();

    const size_t compressed_data_offset = sizeof(BarchHeader) + index_data_size;
    const size_t compressed_data_size = i_data.data.size();

    BarchHeader header{_BA,
                       i_data.width,
                       i_data.height,
                       static_cast<std::uint32_t>(compressed_data_offset),
                       static_cast<std::uint32_t>(compressed_data_size)};

    ofs.write(reinterpret_cast<const char *>(&header), sizeof(header));
    ofs.write(reinterpret_cast<const char *>(index_data.data()), index_data_size);
    ofs.write(reinterpret_cast<const char *>(i_data.data.data()), compressed_data_size);

    return true;
}

//-----------------------------------------------------------------------------
void decompress(const CompressedImageData &i_compressed_data, RawImageData &o_data)
{
    const auto rows_number = std::abs(i_compressed_data.height);
    const auto stride = utils::_get_stride(i_compressed_data.width);

    std::vector<unsigned char> decompressed_data;
    decompressed_data.reserve(rows_number * stride);

    const auto compressed_bits = utils::_get_bits(i_compressed_data.data);
    int _bits_idx = 0;

    std::vector<bool> _bit_vector(32, false);
    for (auto row = 0; row < rows_number; ++row) {
        if (i_compressed_data.index[row] == utils::_EMPTY) { // ALL WHITE
            for (int col = 0; col < stride; ++col)
                decompressed_data.push_back(0xff);
        } else {
            for (int _bytes = 0; _bytes < stride; _bytes += 4) {
                if (compressed_bits[_bits_idx] == _0) { // WHITE
                    for (int i = 0; i < 4; ++i)
                        decompressed_data.push_back(0xff);
                    _bits_idx += 1;
                } else {
                    if (compressed_bits[_bits_idx + 1] == _0) { // BLACK
                        for (int i = 0; i < 4; ++i)
                            decompressed_data.push_back(0x00);
                        _bits_idx += 2;
                    } else { // GREY
                        for (int i = _bits_idx + 2; i < _bits_idx + 34; ++i)
                            _bit_vector[i - _bits_idx - 2] = compressed_bits[i];

                        for (const auto byte : utils::_get_bytes(_bit_vector)) {
                            decompressed_data.push_back(byte);
                        }
                        std::fill_n(_bit_vector.begin(), _bit_vector.size(), false);
                        _bits_idx += 34;
                    }
                }
            }
        }
    }

    o_data.width = i_compressed_data.width;
    o_data.height = i_compressed_data.height;
    o_data.data = reinterpret_cast<unsigned char *>(std::malloc(decompressed_data.size()));
    std::memcpy(o_data.data, decompressed_data.data(), decompressed_data.size());
}

} // namespace barch

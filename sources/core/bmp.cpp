#include "bmp.h"
#include "image.h"
#include "utils.h"

#include <bitset>
#include <cstring>
#include <fstream>
#include <memory>
#include <vector>

namespace {

//-----------------------------------------------------------------------------
#pragma pack(push, 1)

// File header (14 bytes)
struct BMPFileHeader
{
    std::uint16_t file_type; // signature
    std::uint32_t file_size; // file size in Bytes
    std::uint16_t reserved_1;
    std::uint16_t reserved_2;
    std::uint32_t offset_data; // offset to pixel data (54-Byte header + 256*4-Byte palette)
};

//-----------------------------------------------------------------------------

// Info header (40 bytes)
struct BMPInfoHeader
{
    std::uint32_t size;
    std::int32_t width;
    std::int32_t height;
    std::uint16_t planes;      // number of color planes
    std::uint16_t bpp;         // bits per pixel
    std::uint32_t compression; // compression method (0 = no compression)
    std::uint32_t image_size;
    std::int32_t x_ppm;           // horizontal resolution (pixels per meter)
    std::int32_t y_ppm;           // vertical resolution (pixels per meter)
    std::uint32_t clrs_used;      // number of colors in the palette
    std::uint32_t clrs_important; // number of important colors (0 = all colors are important)
};
#pragma pack(pop)

//-----------------------------------------------------------------------------
template<utils::EColor Color>
auto _get_bitset(std::uint32_t);

//-----------------------------------------------------------------------------
template<>
auto _get_bitset<utils::EColor::Black>([[maybe_unused]] std::uint32_t i_pixel_data)
{
    return std::bitset<2>{2}; // 2 bits => 10 => 2 in decimal
}

//-----------------------------------------------------------------------------
template<>
auto _get_bitset<utils::EColor::White>([[maybe_unused]] std::uint32_t i_pixel_data)
{
    return std::bitset<1>{0}; // 1 bit => 0
}

//-----------------------------------------------------------------------------
template<>
auto _get_bitset<utils::EColor::Grey>(std::uint32_t i_pixel_data)
{
    std::bitset<2 + 32> bitset; // 2 bits => 11, + 32 bits => 4x(1Byte) Pixel Value

    struct
    {
        std::bitset<32> bits;
        size_t i;
    } tmp = {i_pixel_data, 0};

    for (; tmp.i < tmp.bits.size(); ++tmp.i)
        bitset[tmp.i] = tmp.bits[tmp.i];

    bitset[bitset.size() - 2] = 1;
    bitset[bitset.size() - 1] = 1;

    return bitset;
}

//-----------------------------------------------------------------------------
template<size_t N>
std::vector<bool> &operator<<(std::vector<bool> &o_vec, const std::bitset<N> &i_bits)
{
    for (size_t i = 0; i < N; ++i)
        o_vec.push_back(i_bits[N - 1 - i]);
    return o_vec;
}

} // namespace

namespace bmp {

//-----------------------------------------------------------------------------
bool read(const std::filesystem::path &i_file_path, RawImageData &o_data)
{
    std::ifstream ifs(i_file_path, std::ios::binary);
    if (!ifs)
        return false;

    BMPFileHeader file_header;
    ifs.read(reinterpret_cast<char *>(&file_header), sizeof(file_header));

    if (file_header.file_type != 0x4D42) // not a bmp file
        return false;

    BMPInfoHeader info_header;
    ifs.read(reinterpret_cast<char *>(&info_header), sizeof(info_header));

    if (info_header.bpp != 8) // only 8-bit BMPs are supported
        return false;

    const auto image_width = info_header.width;
    const auto image_height = static_cast<std::int32_t>(std::abs(info_header.height));
    const auto stride = utils::_get_stride(image_width, info_header.bpp);
    const auto pixel_data_size = image_height * stride;
    auto pixel_data = std::make_unique<unsigned char[]>(pixel_data_size);

    ifs.seekg(file_header.offset_data, std::ios::beg);
    ifs.read(reinterpret_cast<char *>(pixel_data.get()), pixel_data_size);

    auto image_data = std::make_unique<unsigned char[]>(image_height * stride);
    for (int i = 0; i < image_height; ++i)
        for (int j = 0; j < image_width; ++j)
            image_data[i * stride + j] = pixel_data[i * stride + j];

    o_data.width = info_header.width;
    o_data.height = info_header.height;
    o_data.data = image_data.release();

    return true;
}

//-----------------------------------------------------------------------------
bool write(const std::filesystem::path &i_file_path, const RawImageData &i_data)
{
    const auto image_height = static_cast<std::int32_t>(std::abs(i_data.height));

    BMPInfoHeader info_header = {sizeof(BMPInfoHeader),
                                 i_data.width,
                                 image_height,
                                 1,
                                 8,
                                 0,
                                 static_cast<std::uint32_t>(i_data.width * image_height),
                                 0, // x_ppm
                                 0, // y_ppm
                                 256,
                                 256};

    // 256 * 4 => color palette
    const std::uint32_t offset_data = sizeof(BMPFileHeader) + sizeof(BMPInfoHeader) + 256 * 4;
    const auto file_size = offset_data + info_header.image_size;
    BMPFileHeader file_header = {0x4D42, file_size, 0, 0, offset_data};

    std::ofstream ofs(i_file_path, std::ios::binary);
    if (!ofs)
        return false;

    ofs.write(reinterpret_cast<const char *>(&file_header), sizeof(file_header));
    ofs.write(reinterpret_cast<const char *>(&info_header), sizeof(info_header));

    // write grayscale color palette (1024 bytes for 256 colors)
    unsigned char color[4] = {0, 0, 0, 0};
    unsigned char value = 0;
    for (auto i = 0; i < 256; ++i) {
        value = static_cast<unsigned char>(
            i); // each entry has the same value for Blue, Green, Red components
                // and zero for Reserved

        color[0] = value;
        color[1] = value;
        color[2] = value;
        ofs.write(reinterpret_cast<const char *>(color), sizeof(color));
    }

    // write pixel data
    const auto stride = utils::_get_stride(i_data.width);
    ofs.write(reinterpret_cast<const char *>(i_data.data), image_height * stride);

    return true;
}

//-----------------------------------------------------------------------------
void compress(const RawImageData &i_data, CompressedImageData &o_compressed_data)
{
    const auto rows_number = std::abs(i_data.height);
    const auto stride = utils::_get_stride(i_data.width);

    std::vector<bool> index(rows_number, utils::_EMPTY);

    std::vector<bool> compressed_bits;
    compressed_bits.reserve(rows_number * (stride + stride / 2));

    std::vector<bool> compressed_row_bits;
    compressed_row_bits.reserve(stride + stride / 2);

    for (int i = 0; i < rows_number; ++i) {
        for (int j = 0; j < stride; j += 4) {
            const auto [color, word] = utils::_get_pixels_data(i_data.data + i * stride + j);
            if (color == utils::EColor::White) {
                compressed_row_bits << _get_bitset<utils::EColor::White>(word);
            } else {
                if (index[i] == utils::_EMPTY)
                    index[i] = !utils::_EMPTY;

                if (color == utils::EColor::Black)
                    compressed_row_bits << _get_bitset<utils::EColor::Black>(word);
                else // Grey
                    compressed_row_bits << _get_bitset<utils::EColor::Grey>(word);
            }
        }
        if (index[i] != utils::_EMPTY) { // not a white row
            compressed_bits.insert(compressed_bits.end(),
                                   compressed_row_bits.begin(),
                                   compressed_row_bits.end());
        }
        compressed_row_bits.clear();
    }
    compressed_bits.shrink_to_fit();

    // convert bit-vector compressed_bits to byte-vector std::vector<unsigned char>
    auto data = utils::_get_bytes(compressed_bits);

    o_compressed_data.width = i_data.width;
    o_compressed_data.height = i_data.height;
    o_compressed_data.index = std::move(index);
    o_compressed_data.data = std::move(data);
}

} // namespace bmp

#include "utils.h"

namespace utils {

//-----------------------------------------------------------------------------
std::vector<unsigned char> _get_bytes(const std::vector<bool> &i_bit_vector)
{
    if (i_bit_vector.empty())
        return {};

    // divide by rounding up to avoid half-byte situation in the output data
    const auto bytes_number = (i_bit_vector.size() + 7) / 8;
    std::vector<unsigned char> bytes(bytes_number, 0x00);

    int i_last = static_cast<int>(i_bit_vector.size() - 1);
    int i_first = static_cast<int>(i_bit_vector.size() > 8 ? i_last - 8 + 1 : 0);

    while (i_first >= 0) {
        std::uint8_t mask = 1;
        const auto idx = (i_bit_vector.size() - i_last) / 8;
        for (auto i = i_last; i >= i_first; --i) {
            if (i_bit_vector[i])
                bytes[idx] |= mask;
            mask <<= 1;
        }
        i_last -= 8;
        i_first -= 8;

        if (i_last >= 0 && i_first < 0)
            i_first = 0;
    }

    return bytes;
}

//-----------------------------------------------------------------------------
std::vector<bool> _get_bits(const std::vector<unsigned char> &i_bytes_vector)
{
    std::vector<bool> bits;
    if (i_bytes_vector.empty())
        return bits;

    bits.reserve(i_bytes_vector.size() * 8);

    std::uint8_t mask = 1 << 7;
    int i = static_cast<int>(i_bytes_vector.size() - 1);
    for (; i >= 0; --i) {
        while (mask) {
            bits.push_back(i_bytes_vector[i] & mask);
            mask >>= 1;
        }
        mask = 1 << 7;
    }
    return bits;
}

/*-----------------------------------------------------------------------------
 The number of bytes needed to go from the start of one row of pixels to the
 start of the next row. For uncompressed RGB formats, the minimum stride is
 always the image width in bytes, rounded up to the nearest 4 Bytes.
-----------------------------------------------------------------------------*/
std::int32_t _get_stride(const std::int32_t i_bitmap_width, const std::uint16_t i_bpp /*= 8*/)
{
    return ((((i_bitmap_width * i_bpp) + 31) & ~31) >> 3);
}

//-----------------------------------------------------------------------------
std::pair<EColor, std::uint32_t> _get_pixels_data(unsigned char *ip_data)
{
    const auto word = *reinterpret_cast<std::uint32_t *>(ip_data);
    auto color = EColor::White;
    if (word != 0xffffffff)
        color = word == 0 ? EColor::Black : EColor::Grey;
    return {color, word};
}

} // namespace utils

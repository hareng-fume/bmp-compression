#ifndef UTILS_H
#define UTILS_H

#include <cstdint>
#include <utility>
#include <vector>

namespace utils {

inline constexpr bool _EMPTY = true;
enum class EColor { White, Black, Grey };

std::vector<unsigned char> _get_bytes(const std::vector<bool> &i_bit_vector);
std::vector<bool> _get_bits(const std::vector<unsigned char> &i_byte_vector);
std::int32_t _get_stride(const std::int32_t i_bitmap_width, const std::uint16_t i_bpp = 8);
std::pair<EColor, std::uint32_t> _get_pixels_data(unsigned char *ip_data);

} // namespace utils
#endif // UTILS_H

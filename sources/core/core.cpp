#include "core.h"
#include "barch.h"
#include "bmp.h"
#include "image.h"

#include <filesystem>

namespace {

//-----------------------------------------------------------------------------
static constexpr wchar_t _PACKED[] = L"-packed";
static constexpr wchar_t _UNPACKED[] = L"-unpacked";
static constexpr wchar_t _BMP[] = L".bmp";
static constexpr wchar_t _BARCH[] = L".barch";

//-----------------------------------------------------------------------------
std::filesystem::path _convert_file_path(const std::filesystem::path &i_from_path,
                                         const std::filesystem::path &i_to_extension)
{
    std::filesystem::path packed_or_unpacked;
    if (i_to_extension == _BMP)
        packed_or_unpacked = _UNPACKED;
    else if (i_to_extension == _BARCH)
        packed_or_unpacked = _PACKED;

    std::filesystem::path file_name = i_from_path.stem();
    file_name += packed_or_unpacked;
    file_name += i_to_extension;
    return i_from_path.parent_path() / file_name;
}

} // namespace

//-----------------------------------------------------------------------------
bool is_bmp(const std::filesystem::path &i_file_path)
{
    return i_file_path.extension() == _BMP;
}

//-----------------------------------------------------------------------------
bool is_barch(const std::filesystem::path &i_file_path)
{
    return i_file_path.extension() == _BARCH;
}

//-----------------------------------------------------------------------------
std::filesystem::path encode(const std::filesystem::path &i_file_path)
{
    auto image_data = std::make_unique<RawImageData>();
    if (!bmp::read(i_file_path, *image_data))
        return {};

    CompressedImageData compressed_data;
    bmp::compress(*image_data, compressed_data);

    const std::filesystem::path output_path = _convert_file_path(i_file_path, _BARCH);
    if (!barch::write(output_path, compressed_data))
        return {};

    return output_path;
}

//-----------------------------------------------------------------------------
std::filesystem::path decode(const std::filesystem::path &i_file_path)
{
    CompressedImageData compressed_data;
    if (!barch::read(i_file_path, compressed_data))
        return {};

    auto image_data = std::make_unique<RawImageData>();
    barch::decompress(compressed_data, *image_data);

    const std::filesystem::path output_path = _convert_file_path(i_file_path, _BMP);
    if (!bmp::write(output_path, *image_data))
        return {};

    return output_path;
}

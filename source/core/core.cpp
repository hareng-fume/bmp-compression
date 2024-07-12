#include "core.h"
#include "barch.h"
#include "bmp.h"
#include "image.h"

#include <filesystem>
#include <iostream>

namespace {

static constexpr char _PACKED[] = "-packed";
static constexpr char _UNPACKED[] = "-unpacked";
static constexpr char _BMP[] = ".bmp";
static constexpr char _BARCH[] = ".barch";

//-----------------------------------------------------------------------------
std::filesystem::path _convert_file_path(const std::filesystem::path &i_from_path,
                                         const std::filesystem::path &i_to_extension)
{
    if (i_from_path.empty() || i_to_extension.empty())
        return {};

    if (i_from_path.extension() == i_to_extension)
        return {};

    std::string packed_or_unpacked;
    if (i_to_extension == _BMP)
        packed_or_unpacked = _UNPACKED;
    else if (i_to_extension == _BARCH)
        packed_or_unpacked = _PACKED;

    auto file_name = i_from_path.stem();
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
std::wstring encode(const std::wstring &i_file_path)
{
    if (!is_bmp(i_file_path))
        return {};

    auto image_data = std::make_unique<RawImageData>();
    if (!bmp::read(i_file_path, *image_data))
        return {};

#ifdef DEBUG_BUILD
    std::wcout << L"compressing: " << i_file_path << std::endl;
#endif

    CompressedImageData compressed_data;
    bmp::compress(*image_data, compressed_data);

    const auto output_path = _convert_file_path(i_file_path, _BARCH);
    barch::write(output_path, compressed_data);

#ifdef DEBUG_BUILD
    std::wcout << L"compressed and saved to: " << output_path.filename() << std::endl;
#endif

    return output_path;
}

//-----------------------------------------------------------------------------
std::wstring decode(const std::wstring &i_file_path)
{
    if (!is_barch(i_file_path))
        return {};

    CompressedImageData compressed_data;
    if (!barch::read(i_file_path, compressed_data))
        return {};

#ifdef DEBUG_BUILD
    std::wcout << "decompressing: " << i_file_path << std::endl;
#endif

    auto image_data = std::make_unique<RawImageData>();
    barch::decompress(compressed_data, *image_data);

    const auto output_path = _convert_file_path(i_file_path, _BMP);
    bmp::write(output_path, *image_data);

#ifdef DEBUG_BUILD
    std::wcout << L"decompressed and saved to: " << output_path.filename() << std::endl;
#endif

    return output_path;
}

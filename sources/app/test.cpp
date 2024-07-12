#include "test.h"

#include <QDebug>
#include <bitset>
#include <functional>
#include <iomanip>
#include <random>
#include <sstream>
#include <unordered_map>
#include <utils.h>
#include <vector>

namespace {

//-----------------------------------------------------------------------------
template<utils::EColor Color>
auto _get_bitset(std::uint32_t);

//-----------------------------------------------------------------------------
template<>
auto _get_bitset<utils::EColor::Black>(std::uint32_t)
{
    return std::bitset<2>{2}; // 2 bits => 10 => 2 in decimal
}

//-----------------------------------------------------------------------------
template<>
auto _get_bitset<utils::EColor::White>(std::uint32_t)
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

//-----------------------------------------------------------------------------
// bytes view in hex
auto _get_hex_view(const std::vector<unsigned char> &i_data)
{
    std::ostringstream oss;
    if (!i_data.empty()) {
        oss << "0x" << std::hex << std::setw(2) << std::setfill('0')
            << (static_cast<std::uint32_t>(i_data.front()) & 0xff);
    }
    for (size_t i = 1; i < i_data.size(); ++i) {
        oss << ", 0x" << std::hex << std::setw(2) << std::setfill('0')
            << (static_cast<std::uint32_t>(i_data[i]) & 0xff);
    }
    return oss.str();
};

//-----------------------------------------------------------------------------
auto _get_hex_view(const unsigned char *ip_data, const std::uint32_t i_size)
{
    std::ostringstream oss;
    if (i_size != 0) {
        oss << "0x" << std::hex << std::setw(2) << std::setfill('0')
            << (static_cast<std::uint32_t>(ip_data[0]) & 0xff);
    }
    for (std::uint32_t i = 1; i < i_size; ++i) {
        oss << ", 0x" << std::hex << std::setw(2) << std::setfill('0')
            << (static_cast<std::uint32_t>(ip_data[i]) & 0xff);
    }
    return oss.str();
}

//-----------------------------------------------------------------------------
// bits view in 'true', 'false'
std::string _get_oct_view(const std::vector<bool> &i_bits)
{
    QStringList bit_view;
    for (size_t i = 0; i < i_bits.size(); ++i) {
        if (i > 0 && i % 8 == 0)
            bit_view << " ";
        bit_view << (i_bits[i] ? "1" : "0");
    }

    return bit_view.join("").toStdString();
}

//-----------------------------------------------------------------------------
auto _get_full_view(const std::string &i_input_view,
                    const std::string &i_actual_view,
                    const std::string &i_expected_view)
{
    std::ostringstream oss;
    oss << "{" << i_input_view << "} => {" << i_actual_view << "}, expected: {" << i_expected_view
        << "}";
    return oss.str();
}

//-----------------------------------------------------------------------------
std::vector<bool> _compress(unsigned char *ip_data, const std::uint32_t i_stride)
{
    std::vector<bool> compressed_bits;
    for (std::uint32_t j = 0; j < i_stride; j += 4) {
        const auto [color, word] = utils::_get_pixels_data(ip_data + j);
        if (color == utils::EColor::White) {
            compressed_bits << _get_bitset<utils::EColor::White>(word);
        } else {
            if (color == utils::EColor::Black)
                compressed_bits << _get_bitset<utils::EColor::Black>(word);
            else // Grey
                compressed_bits << _get_bitset<utils::EColor::Grey>(word);
        }
    }
    return compressed_bits;
}

//-----------------------------------------------------------------------------
std::vector<unsigned char> _decompress(const std::vector<bool> &i_compressed_bits,
                                       const std::uint32_t i_stride)
{
    int _bits_idx = 0;
    const bool _0 = false;

    std::vector<unsigned char> decompressed_data;
    decompressed_data.reserve(i_stride);

    std::vector<bool> _bit_vector(32, false);
    for (std::uint32_t _bytes = 0; _bytes < i_stride; _bytes += 4) {
        if (i_compressed_bits[_bits_idx] == _0) { // WHITE
            for (int i = 0; i < 4; ++i)
                decompressed_data.push_back(0xff);
            _bits_idx += 1;
        } else {
            if (i_compressed_bits[_bits_idx + 1] == _0) { // BLACK
                for (int i = 0; i < 4; ++i)
                    decompressed_data.push_back(0x00);
                _bits_idx += 2;
            } else { // GREY
                for (int j = _bits_idx + 2; j < _bits_idx + 34; ++j)
                    _bit_vector[j - _bits_idx - 2] = i_compressed_bits[j];

                for (const auto byte : utils::_get_bytes(_bit_vector)) {
                    decompressed_data.push_back(byte);
                }
                std::fill_n(_bit_vector.begin(), _bit_vector.size(), false);
                _bits_idx += 34;
            }
        }
    }
    return decompressed_data;
}

//-----------------------------------------------------------------------------
std::vector<unsigned char> _generate_random_bytes(const std::uint32_t i_length)
{
    // create a random number generator
    std::random_device rd;  // Seed generator
    std::mt19937 gen(rd()); // Mersenne Twister RNG

    // define the distribution range for a byte (0 to 255)
    std::uniform_int_distribution<> dis(0, 255);

    // create a vector to store the random bytes
    std::vector<unsigned char> random_bytes(i_length);

    // Generate the random bytes
    for (size_t i = 0; i < i_length; ++i)
        random_bytes[i] = static_cast<unsigned char>(dis(gen));

    return random_bytes;
}

//-----------------------------------------------------------------------------
void _print_status(const bool i_status_ok)
{
    qDebug() << "[" << (i_status_ok ? "" : "N")
             << "OK]--------------------------------------------------------------------";
};

} // namespace

namespace _Test {

//-----------------------------------------------------------------------------
bool _check_compress_decompress()
{
    auto _check = [](unsigned char *ip_data,
                     const std::uint32_t i_size,
                     const std::uint32_t i_stride) mutable -> bool {
        qDebug() << "[INFO]: orig :        " << _get_hex_view(ip_data, i_size);
        qDebug() << "[INFO]: orig rounded: " << _get_hex_view(ip_data, i_stride);

        const auto orig_bits = utils::_get_bits(
            std::vector<unsigned char>{ip_data, ip_data + i_stride});
        qDebug() << "[INFO]: orig bits:    " << _get_oct_view(orig_bits);

        const auto orig_bytes = utils::_get_bytes(orig_bits);
        qDebug() << "[INFO]: orig bytes:   " << _get_hex_view(orig_bytes);

        const auto compressed_bits = _compress(ip_data, i_stride);
        qDebug() << "[INFO]: compressed:   " << _get_oct_view(compressed_bits);
        const auto decompressed_data = _decompress(compressed_bits, i_stride);

        qDebug() << "[INFO]: res rounded:  " << _get_hex_view(decompressed_data);

        bool status_ok = true;
        if (i_stride != decompressed_data.size()) {
            qDebug() << "ERROR:        "
                     << " orig size: " << i_stride << " != res size: " << decompressed_data.size();
            status_ok = false;
        } else {
            for (std::uint32_t i = 0; i < i_stride; ++i)
                if (ip_data[i] != decompressed_data[i]) {
                    qDebug() << "ERROR:        "
                             << "orig data != res data";
                    status_ok = false;
                }
        }
        _print_status(status_ok);
        return status_ok;
    };

    bool passed = true;

    if (true) {
        unsigned char data[12] = {0x00, 0xff, 0x00, 0xff, 0x00, 0xff, 0x00, 0xff, 0x00};
        passed = passed && _check(data, 9, utils::_get_stride(9));
    }

    if (true) {
        unsigned char data[8] = {0x00, 0x00, 0x00, 0x00, 0xff, 0xff, 0xff, 0xff};
        passed = passed && _check(data, 8, utils::_get_stride(8));
    }

    if (true) {
        unsigned char data[8] = {0x22, 0x22, 0x22, 0x22, 0x22, 0x22, 0x22, 0x22};
        passed = passed && _check(data, 8, utils::_get_stride(8));
    }

    if (true) { // all black 8Bytes
        unsigned char data[8] = {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};
        passed = passed && _check(data, 8, utils::_get_stride(8));
    }

    if (true) { // all black 9Bytes
        unsigned char data[9] = {0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};
        passed = passed && _check(data, 9, utils::_get_stride(9));
    }

    if (true) {
        auto data = _generate_random_bytes(5);
        passed = passed && _check(data.data(), 5, utils::_get_stride(5));
    }

    if (true) {
        auto data = _generate_random_bytes(8);
        passed = passed && _check(data.data(), 8, utils::_get_stride(8));
    }

    if (true) {
        auto data = _generate_random_bytes(13);
        passed = passed && _check(data.data(), 13, utils::_get_stride(13));
    }

    if (true) {
        auto data = _generate_random_bytes(120);
        passed = passed && _check(data.data(), 120, utils::_get_stride(120));
    }

    if (true) {
        auto data = _generate_random_bytes(121);
        passed = passed && _check(data.data(), 121, utils::_get_stride(121));
    }

    if (true) {
        auto data = _generate_random_bytes(825);
        passed = passed && _check(data.data(), 825, utils::_get_stride(825));
    }

    return passed;
}

//-----------------------------------------------------------------------------
bool _check_bits_to_bytes()
{
    auto _check = [](const std::vector<bool> &i_input_bits,
                     const std::vector<unsigned char> &i_expected_bytes) {
        const auto actual_bytes = utils::_get_bytes(i_input_bits);

        const auto full_view = _get_full_view(_get_oct_view(i_input_bits),
                                              _get_hex_view(actual_bytes),
                                              _get_hex_view(i_expected_bytes));

        bool status_ok = true;
        if (actual_bytes != i_expected_bytes) {
            qDebug() << "ERROR:" << full_view;
            status_ok = false;
        }
        _print_status(status_ok);
        return status_ok;
    };

    bool passed = true;

    if (true) {
        passed = passed
                 && _check({true, false, true, false, true, false, true, false, true}, {0x55, 0x01});
    }

    if (true) {
        passed = passed
                 && _check({true,  true,  true,  false, false, false, true,  false, false,
                            true,  true,  false, false, false, true,  false, false, true,
                            true,  false, false, false, true,  false, false, true,  true,
                            false, false, false, true,  false, false, true},
                           {0x89, 0x89, 0x89, 0x89, 0x03});
    }

    if (true) {
        passed = passed
                 && _check({false, false, false, false, false, false, false, false, false},
                           {0x00, 0x00});
    }

    if (true) {
        passed = passed && _check({true, false}, {0x02});
        passed = passed && _check({true, false, false}, {0x04});
        passed = passed && _check({false, true}, {0x01});
        passed = passed && _check({true, true}, {0x03});
        passed = passed && _check({false}, {0x00});
        passed = passed && _check({false, false}, {0x00});
        passed = passed && _check({}, {});
    }

    return passed;
}

//-----------------------------------------------------------------------------
bool _check_bytes_to_bits()
{
    auto _check = [](const std::vector<unsigned char> &i_input_bytes,
                     const std::vector<bool> &i_expected_bits) {
        const auto actual_bits = utils::_get_bits(i_input_bytes);

        const auto full_view = _get_full_view(_get_hex_view(i_input_bytes),
                                              _get_oct_view(actual_bits),
                                              _get_oct_view(i_expected_bits));

        bool status_ok = true;
        if (actual_bits != i_expected_bits) {
            qDebug() << "ERROR:" << full_view;
            status_ok = false;
        }
        _print_status(status_ok);
        return status_ok;
    };

    bool passed = true;

    if (true) {
        passed = passed
                 && _check({0x55, 0x01},
                           {false,
                            false,
                            false,
                            false,
                            false,
                            false,
                            false,
                            true,
                            false,
                            true,
                            false,
                            true,
                            false,
                            true,
                            false,
                            true});
    }

    if (true) {
        passed = passed
                 && _check({0x89, 0x89, 0x89, 0x89, 0x03},
                           {false, false, false, false, false, false, true,  true,  true,  false,
                            false, false, true,  false, false, true,  true,  false, false, false,
                            true,  false, false, true,  true,  false, false, false, true,  false,
                            false, true,  true,  false, false, false, true,  false, false, true});
    }
    if (true) {
        passed = passed && _check({0x02}, {false, false, false, false, false, false, true, false});
        passed = passed && _check({0x04}, {false, false, false, false, false, true, false, false});
        passed = passed && _check({0x01}, {false, false, false, false, false, false, false, true});

        passed = passed && _check({0x03}, {false, false, false, false, false, false, true, true});
        passed = passed && _check({0x00}, {false, false, false, false, false, false, false, false});
        passed = passed && _check({}, {});
    }

    return passed;
}

//-----------------------------------------------------------------------------
void _run_tests()
{
    std::unordered_map<QString, std::function<bool()>> tests
        = {{"bits-to-bytes", _check_bits_to_bytes},
           {"bytes-to_bits", _check_bytes_to_bits},
           {"compress-decompress", _check_compress_decompress}};

    size_t _passed = 0, _test_no = 0;
    for (const auto &[test_name, test_check] : tests) {
        QString test_no = "/test #%1: "
                          "[%2]------------------------------------"
                          "--------------------------------";

        qDebug() << test_no.arg(++_test_no).arg(test_name);
        _passed += test_check();
    }
    if (_passed == tests.size())
        qDebug() << "ALL-INFO: "
                 << "[" << _passed << "/" << tests.size() << "]"
                 << " tests passed successfully.";
    else
        qDebug() << "ALL-ERROR: "
                 << "[" << _passed << "/" << tests.size() << "]"
                 << " tests passed.";
}

} // namespace _Test

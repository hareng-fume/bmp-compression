#ifndef IMAGE_H
#define IMAGE_H

#include <memory>
#include <vector>

struct RawImageData final
{
    int width;  // image width in pixels
    int height; // image height in pixels
    unsigned char *
        data; // Pointer to image data. data[j * width + i] is color of pixel in row j and column i.
};

template<>
struct std::default_delete<RawImageData>
{
public:
    void operator()(RawImageData *ptr) const
    {
        delete[] ptr->data;
        ptr->data = nullptr;

        delete ptr;
    }
};

struct CompressedImageData final
{
    int width;
    int height;

    std::vector<bool> index;
    std::vector<unsigned char> data;
};

#endif // IMAGE_H

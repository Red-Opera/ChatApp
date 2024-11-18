#include "../Utill/Log.h"
#include "StrToImage.h"

#include <cstring>

std::string StrToImage::WriteBmpHeader(int width, int height, int padding)
{
    std::string bmp_data;

    // BMP 헤더 작성
    unsigned char bmpfileheader[14] =
    {
        'B', 'M',
        0, 0, 0, 0,
        0, 0, 0, 0,
        54, 0, 0, 0
    };

    unsigned char bmpinfoheader[40] =
    {
        40, 0, 0, 0,
        0, 0, 0, 0,
        0, 0, 0, 0,
        1, 0,
        24, 0,
        0, 0, 0, 0,
        0, 0, 0, 0,
        0, 0, 0, 0,
        0, 0, 0, 0
    };

    int file_size = 54 + (width * 3 + padding) * height;
    bmpfileheader[2] = (unsigned char)(file_size);
    bmpfileheader[3] = (unsigned char)(file_size >> 8);
    bmpfileheader[4] = (unsigned char)(file_size >> 16);
    bmpfileheader[5] = (unsigned char)(file_size >> 24);

    bmpinfoheader[4] = (unsigned char)(width);
    bmpinfoheader[5] = (unsigned char)(width >> 8);
    bmpinfoheader[6] = (unsigned char)(width >> 16);
    bmpinfoheader[7] = (unsigned char)(width >> 24);

    bmpinfoheader[8] = (unsigned char)(height);
    bmpinfoheader[9] = (unsigned char)(height >> 8);
    bmpinfoheader[10] = (unsigned char)(height >> 16);
    bmpinfoheader[11] = (unsigned char)(height >> 24);

    bmp_data.append(reinterpret_cast<char*>(bmpfileheader), 14);
    bmp_data.append(reinterpret_cast<char*>(bmpinfoheader), 40);

    return bmp_data;
}

void StrToImage::StrToBit(const char* str, unsigned char* bit_array, int* bit_length)
{
    *bit_length = strlen(str) * 8;

    for (int i = 0; i < strlen(str); i++)
    {
        for (int j = 0; j < 8; j++)
            bit_array[i * 8 + j] = (str[i] >> (7 - j)) & 1;
    }
}

void StrToImage::BitToStr(const unsigned char* bit_array, int bit_length, char* str)
{
    int byte_count = bit_length / 8;

    for (int i = 0; i < byte_count; i++)
    {
        str[i] = 0;

        for (int j = 0; j < 8; j++)
            str[i] |= (bit_array[i * 8 + j] << (7 - j));
    }

    str[byte_count] = '\0';
}

std::string StrToImage::CraeteImageFromBit(const unsigned char* bit_array, int bit_length, int width, int height)
{
    int padding = (4 - (width * 3) % 4) % 4;
    int bit_index = 0;

    std::string bmp_data = WriteBmpHeader(width, height, padding);

    // 픽셀 데이터 작성
    for (int y = 0; y < height; y++)
    {
        for (int x = 0; x < width; x++)
        {
            unsigned char color = 255;

            if (bit_index < bit_length && bit_array[bit_index] == 1)
                color = 0;

            unsigned char pixel[3] = { color, color, color };
            bmp_data.append(reinterpret_cast<char*>(pixel), 3);

            bit_index++;
        }

        unsigned char padding_bytes[3] = { 0, 0, 0 };
        bmp_data.append(reinterpret_cast<char*>(padding_bytes), padding);
    }

    return bmp_data;
}
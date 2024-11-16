#include "BitChanger.h"

#include <string>
#include <stdexcept>
#include <windows.h>

std::string BitChanger::UnicodeToUTF8(const std::wstring& wstr)
{
    int bufferSize = WideCharToMultiByte(CP_UTF8, 0, wstr.c_str(), -1, NULL, 0, NULL, NULL);

    if (bufferSize == 0)
        return "";

    std::string utf8Str(bufferSize - 1, 0);  // 널 문자는 제외
    WideCharToMultiByte(CP_UTF8, 0, wstr.c_str(), -1, &utf8Str[0], bufferSize, NULL, NULL);

    return utf8Str;
}

std::wstring BitChanger::UTF8ToUnicode(const std::string& str)
{
    int bufferSize = MultiByteToWideChar(CP_UTF8, 0, str.c_str(), -1, NULL, 0);

    if (bufferSize == 0)
        return L"";

    std::wstring wstr(bufferSize - 1, 0);  // 널 문자는 제외
    MultiByteToWideChar(CP_UTF8, 0, str.c_str(), -1, &wstr[0], bufferSize);

    return wstr;
}

void BitChanger::StrToBit(const char* str, unsigned char* bit_array, int* bit_length)
{
    *bit_length = strlen(str) * 8;

    for (int i = 0; i < strlen(str); i++)
    {
        for (int j = 0; j < 8; j++)
            bit_array[i * 8 + j] = (str[i] >> (7 - j)) & 1;
    }
}

void BitChanger::BitToStr(unsigned char* bit_array, int bit_length, char* str)
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

std::vector<unsigned char> BitChanger::CraeteBitFromImage(const std::string& bmp_data, int width, int height)
{
    // BMP 헤더 크기
    const int BMP_HEADER_SIZE = 54;
    if (bmp_data.size() < BMP_HEADER_SIZE)
        throw std::runtime_error("Invalid BMP data");

    // 비트맵 데이터 시작점
    const unsigned char* pixel_data = reinterpret_cast<const unsigned char*>(bmp_data.data() + BMP_HEADER_SIZE);

    int padding = (4 - (width * 3) % 4) % 4;
    std::vector<unsigned char> bit_array;

    int bit_index = 0;

    for (int y = 0; y < height; y++)
    {
        for (int x = 0; x < width; x++)
        {
            if (bit_index < (width * height))
            {
                // 픽셀의 첫 번째 색상 채널(R)을 기준으로 0 또는 1 판별
                unsigned char color = pixel_data[(y * (width * 3 + padding)) + (x * 3)];
                bit_array.push_back(color == 0 ? 1 : 0);
                bit_index++;
            }
        }
    }

    return bit_array;
}
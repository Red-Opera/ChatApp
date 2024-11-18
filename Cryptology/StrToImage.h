#pragma once

#include <fstream>
#include <vector>

class StrToImage
{
public:
    static void StrToBit(const char* str, unsigned char* bit_array, int* bit_length);
    static void BitToStr(const unsigned char* bit_array, int bit_length, char* str);
    static std::string WriteBmpHeader(int width, int height, int padding);
    static std::string CraeteImageFromBit(const unsigned char* bit_array, int bit_length, int width, int height);
};
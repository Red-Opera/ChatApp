#pragma once

#include <fstream>
#include <vector>

class BitChanger
{
public:
	static std::string UnicodeToUTF8(const std::wstring& wstr);							// 유니코드 문자열을 UTF-8로 변환하는 함수
	static std::wstring UTF8ToUnicode(const std::string& str);							// UTF-8 문자열을 유니코드로 변환하는 함수

	static void StrToBit(const char* str, unsigned char* bit_array, int* bit_length);		// 문자열 배열 -> 비트 변환
	static void BitToStr(unsigned char* bit_array, int bit_length, char* str);				// 비트 배열 -> 문자열 변환
	static std::vector<unsigned char> CraeteBitFromImage(const std::string& bmp_data, int width, int height);
};
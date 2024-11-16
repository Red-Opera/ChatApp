#pragma once

#include <fstream>
#include <vector>

class BitChanger
{
public:
	static std::string UnicodeToUTF8(const std::wstring& wstr);							// �����ڵ� ���ڿ��� UTF-8�� ��ȯ�ϴ� �Լ�
	static std::wstring UTF8ToUnicode(const std::string& str);							// UTF-8 ���ڿ��� �����ڵ�� ��ȯ�ϴ� �Լ�

	static void StrToBit(const char* str, unsigned char* bit_array, int* bit_length);		// ���ڿ� �迭 -> ��Ʈ ��ȯ
	static void BitToStr(unsigned char* bit_array, int bit_length, char* str);				// ��Ʈ �迭 -> ���ڿ� ��ȯ
	static std::vector<unsigned char> CraeteBitFromImage(const std::string& bmp_data, int width, int height);
};
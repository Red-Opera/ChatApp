#pragma once

#include "AESDefine.h"

#include <string>
#include <vector>

class AES
{
public:
	static void AES_Inverse_Cipher(BYTEAES* in, BYTEAES* out, BYTEAES* key);	// AES 복호화
	static std::vector<BYTEAES> initialize_sboxes(std::string message, int& blockCount, BYTEAES* key);

private:

	static void KeyExpansion(BYTEAES* key, WORDAES* W);					// AES 키 확장 함수
	static void AddRoundKey(BYTEAES state[][4], WORDAES* rKey);			// AddRoundKey
	static WORDAES SubWord(WORDAES W);									// SubWord
	static WORDAES RotWord(WORDAES W);									// RotWord
	static BYTEAES x_time(BYTEAES n, BYTEAES b);								// GF(2^8) 상에서 곱셈 연산 함수
	static BYTEAES x_time_1(BYTEAES n, BYTEAES b);							// GF(2^8) 상에서 곱셈 연산 함수

	static void Inv_SubBytes(BYTEAES state[][4]);						// Invers SubBytes
	static void Inv_ShiftRows(BYTEAES state[][4]);						// Invers ShiftRows
	static void Inv_MixColumns(BYTEAES state[][4]);					// Invers MixColumns
	static void Inv_CirShiftRows(BYTEAES* row);						// state의 한 행을 1회 왼쪽으로 순환 시프트
};

static unsigned char S_box[SIZE][SIZE];
static unsigned char Inv_S_box[SIZE][SIZE];
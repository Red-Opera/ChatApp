#include "AESDefine.h"

#include <string>

class AES
{
public:
	static void AES_Cipher(BYTE* in, BYTE* out, BYTE* key);	// AES 암호화
	static void initialize_sboxes();
	static void generate_key_from_time(BYTE* key);			// 키 생성 함수
	static std::string GetSBox();

private:
	static void SubBytes(BYTE state[][4]);					// SubBytes
	static void ShiftRows(BYTE state[][4]);					// ShiftRows
	static void MixColumns(BYTE state[][4]);				// MixColumns

	static void AddRoundKey(BYTE state[][4], WORD* rKey);	// AddRoundKey

	static void KeyExpansion(BYTE* key, WORD* W);			// AES 키 확장 함수
	static void CirShiftRows(BYTE* row);					// state의 한 행을 1회 오른쪽으로 순환 시프트

	static WORD SubWord(WORD W);							// SubWord
	static WORD RotWord(WORD W);							// RotWord
	static BYTE x_time(BYTE n, BYTE b);						// GF(2^8) 상에서 곱셈 연산 함수
	static BYTE x_time_1(BYTE n, BYTE b);					// GF(2^8) 상에서 곱셈 연산 함수
};
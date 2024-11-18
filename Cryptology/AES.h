#include "AESDefine.h"

#include <string>

class AES
{
public:
	static void AES_Cipher(BYTE* in, BYTE* out, BYTE* key);	// AES ��ȣȭ
	static void initialize_sboxes();
	static void generate_key_from_time(BYTE* key);			// Ű ���� �Լ�
	static std::string GetSBox();

private:
	static void SubBytes(BYTE state[][4]);					// SubBytes
	static void ShiftRows(BYTE state[][4]);					// ShiftRows
	static void MixColumns(BYTE state[][4]);				// MixColumns

	static void AddRoundKey(BYTE state[][4], WORD* rKey);	// AddRoundKey

	static void KeyExpansion(BYTE* key, WORD* W);			// AES Ű Ȯ�� �Լ�
	static void CirShiftRows(BYTE* row);					// state�� �� ���� 1ȸ ���������� ��ȯ ����Ʈ

	static WORD SubWord(WORD W);							// SubWord
	static WORD RotWord(WORD W);							// RotWord
	static BYTE x_time(BYTE n, BYTE b);						// GF(2^8) �󿡼� ���� ���� �Լ�
	static BYTE x_time_1(BYTE n, BYTE b);					// GF(2^8) �󿡼� ���� ���� �Լ�
};
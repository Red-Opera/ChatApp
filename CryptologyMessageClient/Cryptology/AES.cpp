#include "AES.h"

#include <stdlib.h>
#include <vector>
#include <cstring>
#include <string>
#include <iostream>

void AES::AES_Inverse_Cipher(BYTEAES* in, BYTEAES* out, BYTEAES* key)
{
	int i, j;
	BYTEAES state[4][4];
	WORDAES* W;

	// 정의된 키 길의에 따른 라운드 수와 워드의 개수를 계산하여 메모리 할당
	if (Nk == 4)
	{
		Nr = 10;
		W = (WORDAES*)malloc(sizeof(WORDAES) * Nb * (Nr + 1));
	}

	if (Nk == 6)
	{
		Nr = 12;
		W = (WORDAES*)malloc(sizeof(WORDAES) * Nb * (Nr + 1));
	}

	if (Nk == 8)
	{
		Nr = 14;
		W = (WORDAES*)malloc(sizeof(WORDAES) * Nb * (Nr + 1));
	}

	for (i = 0; i < 4; i++)
		for (j = 0; j < 4; j++)
			state[j][i] = in[i * 4 + j];

	KeyExpansion(key, W);				// 키 확장

	// 0 라운드 키로 AddRoundKey 수행
	AddRoundKey(state, &W[Nr * Nb]);		// 복호화에서는 라운드 키가 역순으로 들어감

	// AES Round 1 ~ (라운드 수 - 1)
	for (i = 0; i < Nr - 1; i++)
	{
		Inv_ShiftRows(state);
		Inv_SubBytes(state);
		AddRoundKey(state, &W[(Nr - i - 1) * Nb]);
		Inv_MixColumns(state);
	}

	// 마지막 라운드는 Inv_MixColumns가 들어가지 않음
	Inv_ShiftRows(state);
	Inv_SubBytes(state);
	AddRoundKey(state, &W[(Nr - i - 1) * Nb]);

	// 결과 값 저장
	for (i = 0; i < 4; i++)
		for (j = 0; j < 4; j++)
			out[i * 4 + j] = state[j][i];

	free(W);	// 메모리 해제
}

std::vector<BYTEAES> AES::initialize_sboxes(std::string message, int& blockCount, BYTEAES* key)
{
	int i, j;
	int index = 0;

	size_t delimiterPos = message.find("\n\n\n\n\n\n\n\n\n");

	if (delimiterPos == std::string::npos)
		throw std::runtime_error("Invalid format: delimiter not found");

	// 키 부분 추출
	std::string keyPart = message.substr(0, delimiterPos);

	if (keyPart.size() >= Nk * 4 + 1)
		throw std::runtime_error("Key size exceeds the expected limit");

	// 키를 BYTE 배열로 복사
	std::memset(key, 0, Nk * 4 + 1);
	std::memcpy(key, keyPart.data(), keyPart.size());
	
	// message에서 키 부분 제거
	message.erase(0, delimiterPos + 9); // 키와 구분자 제거

	// blockCount 부분 추출
	size_t blockCountEndPos = message.find("\n\n\n\n\n\n\n\n\n");

	if (blockCountEndPos == std::string::npos)
		throw std::runtime_error("Invalid format: blockCount delimiter not found");

	std::string blockCountPart = message.substr(0, blockCountEndPos);
	try {
		blockCount = std::stoi(blockCountPart);
	}
	catch (const std::exception& e) {
		throw std::runtime_error("Invalid blockCount format");
	}

	// message에서 blockCount 부분 제거
	message.erase(0, blockCountEndPos + 9); // blockCount와 구분자 제거

	// S_box를 메세지에서 복원
	for (int i = 0; i < SIZE; ++i) {
		for (int j = 0; j < SIZE; ++j) {
			size_t space_pos = message.find(' ', index);
			size_t newline_pos = message.find('\n', index);

			std::string hex_value;
			if (newline_pos != std::string::npos && (space_pos == std::string::npos || newline_pos < space_pos)) {
				// 마지막 값 처리 (\n 전까지)
				hex_value = message.substr(index, newline_pos - index);
				index = newline_pos + 1; // \n 이후로 이동
			}
			else if (space_pos != std::string::npos) {
				// 일반 값 처리 (공백 전까지)
				hex_value = message.substr(index, space_pos - index);
				index = space_pos + 1; // 공백 이후로 이동
			}
			else {
				throw std::runtime_error("Invalid message format: no space or newline separator");
			}

			S_box[i][j] = static_cast<unsigned char>(std::stoi(hex_value, nullptr, 16));
		}
	}

	// S-박스와 역 S-박스 채우기
	for (i = 0; i < SIZE; i++) 
	{
		for (j = 0; j < SIZE; j++)
			Inv_S_box[S_box[i][j] / SIZE][S_box[i][j] % SIZE] = i * SIZE + j;
	}

	// SBox 이후 문자열 추출 (암호문)
	std::string encrypted_message = message.substr(index + 8); // 남은 메시지를 암호문으로 처리

	return std::vector<BYTEAES>(encrypted_message.begin(), encrypted_message.end());;
}

void AES::KeyExpansion(BYTEAES* key, WORDAES* W)
{
	WORDAES temp;
	int i = 0;

	// 첫번째 word에는 입력된 키 값이 들어감
	while (i < Nk)
	{
		W[i] = BTOW(key[4 * i], key[4 * i + 1], key[4 * i + 2], key[4 * i + 3]);
		i = i + 1;
	}

	i = Nk;

	// 두번째 word부터는 이전 word 값을 이용해 SubWord와 RotWord 함수, Rcon 상수, XOR 연산을 적용시켜 구함
	while (i < (Nb * (Nr + 1)))
	{
		temp = W[i - 1];
		if (i % Nk == 0)
			temp = SubWord(RotWord(temp)) ^ Rcon[i / Nk - 1];
		else if ((Nk > 6) && (i % Nk == 4))
			temp = SubWord(temp);

		W[i] = W[i - Nk] ^ temp;
		i += 1;
	}
}

void AES::AddRoundKey(BYTEAES state[][4], WORDAES* rKey)
{
	int i, j;
	WORDAES mask, shift;

	// state와 라운드 키의 XOR 연산
	for (i = 0; i < 4; i++)
	{
		shift = 24;
		mask = 0xFF000000;

		// state는 byte(8bit) 단위이고, 라운드 키는 word(32bit) 단위이므로
		// 라운드 키를 상위비트부터 8bit씩 추출하여 XOR 연산을 함
		for (j = 0; j < 4; j++)
		{
			state[j][i] = ((rKey[i] & mask) >> shift) ^ state[j][i];
			mask >>= 8;
			shift -= 8;
		}
	}
}

WORDAES AES::SubWord(WORDAES W)
{
	int i;
	WORDAES out = 0, mask = 0xFF000000;
	BYTEAES shift = 24;

	// 인자로 들어온 32bit word 값을 상위비트부터 8bit씩 추출하고,
	// 추출한 값을 상위 4bit, 하위 4bit로 나누어 S_box의 행과 열의 값으로 사용해 얻은 결과 값(8bit)을
	// 저장할 32bit word에 상위비트부터 8bit씩 채움
	for (i = 0; i < 4; i++)
	{
		out += (WORDAES)S_box[HIHEX((W & mask) >> shift)][LOWHEX((W & mask) >> shift)] << shift;
		mask >>= 8;
		shift -= 8;
	}

	return out;
}

WORDAES AES::RotWord(WORDAES W)
{
	return ((W & 0xFF000000) >> 24) | (W << 8);
}

BYTEAES AES::x_time(BYTEAES n, BYTEAES b)
{
	int i;
	BYTEAES temp = 0, mask = 0x01;

	for (i = 0; i < 8; i++)
	{
		if (n & mask)
			temp ^= b;

		// 최상위 bit가 1이면 시프트 후 0x1B(x^8 + x^4 + x^3 + x^2 + 1)를 XOR 해줌
		if (b & 0x80)
			b = (b << 1) ^ 0x1B;
		else
			b <<= 1;

		mask <<= 1;
	}

	return temp;
}

BYTEAES AES::x_time_1(BYTEAES n, BYTEAES b)
{
	int i;
	BYTEAES temp = 0, mask = 0x01;

	for (i = 0; i < 8; i++)
	{
		if (n & mask)
			temp ^= b;

		// 최상위 bit가 1이면 시프트 후 0x1B(x^8 + x^4 + x^3 + x^2 + 1)를 XOR 해줌
		if (b & 0x80)
			b = (b << 1) ^ 0x1B;
		else
			b <<= 1;

		mask <<= 1;
	}

	return temp;
}

void AES::Inv_SubBytes(BYTEAES state[][4])
{
	int i, j;

	// state의 하나의 값은 1byte 이므로 이 8bit 값을 상위 4bit, 하위 4bit로 나누어
	// 상위 비트는 Inv_S_box의 행 번호로, 하위 비트는 열 번호로 사용함
	// (예: state[i][j] = 10100011(2) -> 상위 : 1010(2) = 10, 하위 : 0011(2) = 3 -> Inv_S_box[10][3])
	for (i = 0; i < 4; i++)
		for (j = 0; j < 4; j++)
			state[i][j] = Inv_S_box[HIHEX(state[i][j])][LOWHEX(state[i][j])];
}

void AES::Inv_ShiftRows(BYTEAES state[][4])
{
	int i, j;

	// state[4][4]에서 
	// 첫번째 행은 시프트가 없고,
	// 두번째 행은 1번 시프트,
	// 세번째 행은 2번 시프트,
	// 네번째 행은 3번 시프트 함
	for (i = 1; i < 4; i++)
		for (j = 0; j < i; j++)
			Inv_CirShiftRows(state[i]);
}

void AES::Inv_MixColumns(BYTEAES state[][4])
{
	int i, j, k;
	BYTEAES a[4][4] = { 0x0E, 0x0B, 0x0D, 0x09,		// a^-1(x) = 0bx^3 + 0dx^2 + 09x + 0e
					 0x09, 0x0E, 0x0B, 0x0D,
					 0x0D, 0x09, 0x0E, 0x0B,
					 0x0B, 0x0D, 0x09, 0x0E };
	BYTEAES b[4][4] = { 0, };

	// 행렬의 곱셈 (state'[i][4] = a[4][4] * state[i][4])
/*	for(i=0;i<4;i++)
	{
		BYTE temp[4] = {0,};

		for(j=0;j<4;j++)
			for(k=0;k<4;k++)
				temp[j] ^= x_time(state[k][i], a[j][k]);	// 곱셈은 x_time 함수를 통해서 수행

		// 곱셈 결과를 state에 저장
		state[0][i] = temp[0];
		state[1][i] = temp[1];
		state[2][i] = temp[2];
		state[3][i] = temp[3];
	}
*/
/*	for(i=0;i<4;i++)
	{
		for(j=0;j<4;j++)
			for(k=0;k<4;k++)
				b[j][i] ^= x_time_1(a[j][k], state[k][i]);	// 곱셈은 x_time 함수를 통해서 수행
	}
	for(i=0;i<4;i++)
			for(j=0;j<4;j++) state[i][j]=b[i][j];
*/

	for (i = 0; i < 4; i++)
	{
		for (j = 0; j < 4; j++)
			for (k = 0; k < 4; k++)
				b[i][j] ^= x_time_1(a[i][k], state[k][j]);	// 곱셈은 x_time 함수를 통해서 수행
	}
	for (i = 0; i < 4; i++)
		for (j = 0; j < 4; j++) state[i][j] = b[i][j];
}

void AES::Inv_CirShiftRows(BYTEAES* row)
{
	BYTEAES temp = row[3];

	row[3] = row[2];
	row[2] = row[1];
	row[1] = row[0];
	row[0] = temp;
}
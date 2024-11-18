#include "AES.h"

#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <sstream>

// 암호화 S-box
unsigned char S_box[SIZE][SIZE];
unsigned char Inv_S_box[SIZE][SIZE];

void AES::AES_Cipher(BYTE* in, BYTE* out, BYTE* key)
{
	int i, j;
	BYTE state[4][4];
	WORD* W;

	// 정의된 키 길의에 따른 라운드 수와 워드의 개수를 계산하여 메모리 할당
	if (Nk == 4)
	{
		Nr = 10;
		W = (WORD*)malloc(sizeof(WORD) * Nb * (Nr + 1));
	}

	if (Nk == 6)
	{
		Nr = 12;
		W = (WORD*)malloc(sizeof(WORD) * Nb * (Nr + 1));
	}

	if (Nk == 8)
	{
		Nr = 14;
		W = (WORD*)malloc(sizeof(WORD) * Nb * (Nr + 1));
	}

	for (i = 0; i < 4; i++)
		for (j = 0; j < 4; j++)
			state[j][i] = in[i * 4 + j];

	KeyExpansion(key, W);	// 키 확장

	// 0 라운드 키로 AddRoundKey 수행
	AddRoundKey(state, W);

	// AES Round 1 ~ (라운드 수 - 1)
	for (i = 0; i < Nr - 1; i++)
	{
		SubBytes(state);
		ShiftRows(state);
		MixColumns(state);
		AddRoundKey(state, &W[(i + 1) * 4]);
	}

	// 마지막 라운드는 MixColumns가 들어가지 않음
	SubBytes(state);
	ShiftRows(state);
	AddRoundKey(state, &W[(i + 1) * 4]);

	// 결과 값 저장
	for (i = 0; i < 4; i++)
		for (j = 0; j < 4; j++)
			out[i * 4 + j] = state[j][i];

	free(W);	// 메모리 해제
}

void AES::initialize_sboxes()
{
	int i, j;
	unsigned char values[256];
	for (i = 0; i < 256; i++) {
		values[i] = i;
	}

	// 무작위로 섞기
	for (i = 255; i > 0; i--) {
		int j = rand() % (i + 1);
		unsigned char temp = values[i];
		values[i] = values[j];
		values[j] = temp;
	}

	// S-박스와 역 S-박스 채우기
	for (i = 0; i < SIZE; i++) {
		for (j = 0; j < SIZE; j++) {
			S_box[i][j] = values[i * SIZE + j];
			Inv_S_box[S_box[i][j] / SIZE][S_box[i][j] % SIZE] = i * SIZE + j;
		}
	}
}

void AES::SubBytes(BYTE state[][4])
{
	int i, j;

	// state의 하나의 값은 1byte 이므로 이 8bit 값을 상위 4bit, 하위 4bit로 나누어
	// 상위 비트는 S_box의 행 번호로, 하위 비트는 열 번호로 사용함
	// (예: state[i][j] = 10100011(2) -> 상위 : 1010(2) = 10, 하위 : 0011(2) = 3 -> S_box[10][3])
	for (i = 0; i < 4; i++)
		for (j = 0; j < 4; j++)
			state[i][j] = S_box[HIHEX(state[i][j])][LOWHEX(state[i][j])];
}

void AES::ShiftRows(BYTE state[][4])
{
	int i, j;

	// state[4][4]에서 
	// 첫번째 행은 시프트가 없고,
	// 두번째 행은 1번 시프트,
	// 세번째 행은 2번 시프트,
	// 네번째 행은 3번 시프트 함
	for (i = 1; i < 4; i++)
		for (j = 0; j < i; j++)
			CirShiftRows(state[i]);
}

void AES::MixColumns(BYTE state[][4])
{
	int i, j, k;
	BYTE a[4][4] = { 0x02, 0x03, 0x01, 0x01,		// a(x) = 03x^3 + 01x^2 + 01x + 02
					 0x01, 0x02, 0x03, 0x01,
					 0x01, 0x01, 0x02, 0x03,
					 0x03, 0x01, 0x01, 0x02 };
	BYTE b[4][4] = { 0, };

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

void AES::AddRoundKey(BYTE state[][4], WORD* rKey)
{
	int i, j;
	WORD mask, shift;

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

void AES::generate_key_from_time(BYTE* key)
{
	time_t t;
	struct tm* tm_info;

	// 현재 시간 
	time(&t);
	tm_info = localtime(&t);

	// 월, 일, 시, 분, 초를 이용해 키 생성
	key[0] = (BYTE)(tm_info->tm_mon + 1);  // 월 (0-11에서 1-12로 변환)
	key[1] = (BYTE)tm_info->tm_mday;      // 일
	key[2] = (BYTE)tm_info->tm_hour;      // 시
	key[3] = (BYTE)tm_info->tm_min;       // 분
	key[4] = (BYTE)tm_info->tm_sec;       // 초
	key[5] = (BYTE)(t & 0xFF);            // 시간의 마지막 8비트
	key[6] = (BYTE)((t >> 8) & 0xFF);     // 시간의 두 번째 8비트

	// 나머지 키 바이트를 랜덤하게 채움
	srand((unsigned)t);
	for (int i = 7; i < Nk * 4; i++) {
		key[i] = rand() % 256;
	}
}

std::string AES::GetSBox()
{
	std::ostringstream oss;

	// 배열의 모든 요소를 순회하며 문자열로 변환
	for (int i = 0; i < SIZE; ++i) {
		for (int j = 0; j < SIZE; ++j) {
			// 각 요소를 16진수 형식으로 변환
			oss << std::hex << static_cast<int>(S_box[i][j]);
			if (!(i == SIZE - 1 && j == SIZE - 1)) oss << " "; // 마지막 요소가 아니면 공백 추가
		}
	}

	return oss.str();
}

void AES::KeyExpansion(BYTE* key, WORD* W)
{
	WORD temp;
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

void AES::CirShiftRows(BYTE* row)
{
	BYTE temp = row[0];

	row[0] = row[1];
	row[1] = row[2];
	row[2] = row[3];
	row[3] = temp;
}

WORD AES::SubWord(WORD W)
{
	int i;
	WORD out = 0, mask = 0xFF000000;
	BYTE shift = 24;

	// 인자로 들어온 32bit word 값을 상위비트부터 8bit씩 추출하고,
	// 추출한 값을 상위 4bit, 하위 4bit로 나누어 S_box의 행과 열의 값으로 사용해 얻은 결과 값(8bit)을
	// 저장할 32bit word에 상위비트부터 8bit씩 채움
	for (i = 0; i < 4; i++)
	{
		out += (WORD)S_box[HIHEX((W & mask) >> shift)][LOWHEX((W & mask) >> shift)] << shift;
		mask >>= 8;
		shift -= 8;
	}

	return out;
}

WORD AES::RotWord(WORD W)
{
	return ((W & 0xFF000000) >> 24) | (W << 8);
}

BYTE AES::x_time(BYTE n, BYTE b)
{
	int i;
	BYTE temp = 0, mask = 0x01;

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

BYTE AES::x_time_1(BYTE n, BYTE b)
{
	int i;
	BYTE temp = 0, mask = 0x01;

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
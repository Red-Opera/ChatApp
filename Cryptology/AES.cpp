#include "AES.h"

#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <sstream>

// ��ȣȭ S-box
unsigned char S_box[SIZE][SIZE];
unsigned char Inv_S_box[SIZE][SIZE];

void AES::AES_Cipher(BYTE* in, BYTE* out, BYTE* key)
{
	int i, j;
	BYTE state[4][4];
	WORD* W;

	// ���ǵ� Ű ���ǿ� ���� ���� ���� ������ ������ ����Ͽ� �޸� �Ҵ�
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

	KeyExpansion(key, W);	// Ű Ȯ��

	// 0 ���� Ű�� AddRoundKey ����
	AddRoundKey(state, W);

	// AES Round 1 ~ (���� �� - 1)
	for (i = 0; i < Nr - 1; i++)
	{
		SubBytes(state);
		ShiftRows(state);
		MixColumns(state);
		AddRoundKey(state, &W[(i + 1) * 4]);
	}

	// ������ ����� MixColumns�� ���� ����
	SubBytes(state);
	ShiftRows(state);
	AddRoundKey(state, &W[(i + 1) * 4]);

	// ��� �� ����
	for (i = 0; i < 4; i++)
		for (j = 0; j < 4; j++)
			out[i * 4 + j] = state[j][i];

	free(W);	// �޸� ����
}

void AES::initialize_sboxes()
{
	int i, j;
	unsigned char values[256];
	for (i = 0; i < 256; i++) {
		values[i] = i;
	}

	// �������� ����
	for (i = 255; i > 0; i--) {
		int j = rand() % (i + 1);
		unsigned char temp = values[i];
		values[i] = values[j];
		values[j] = temp;
	}

	// S-�ڽ��� �� S-�ڽ� ä���
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

	// state�� �ϳ��� ���� 1byte �̹Ƿ� �� 8bit ���� ���� 4bit, ���� 4bit�� ������
	// ���� ��Ʈ�� S_box�� �� ��ȣ��, ���� ��Ʈ�� �� ��ȣ�� �����
	// (��: state[i][j] = 10100011(2) -> ���� : 1010(2) = 10, ���� : 0011(2) = 3 -> S_box[10][3])
	for (i = 0; i < 4; i++)
		for (j = 0; j < 4; j++)
			state[i][j] = S_box[HIHEX(state[i][j])][LOWHEX(state[i][j])];
}

void AES::ShiftRows(BYTE state[][4])
{
	int i, j;

	// state[4][4]���� 
	// ù��° ���� ����Ʈ�� ����,
	// �ι�° ���� 1�� ����Ʈ,
	// ����° ���� 2�� ����Ʈ,
	// �׹�° ���� 3�� ����Ʈ ��
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

	// ����� ���� (state'[i][4] = a[4][4] * state[i][4])
/*	for(i=0;i<4;i++)
	{
		BYTE temp[4] = {0,};

		for(j=0;j<4;j++)
			for(k=0;k<4;k++)
				temp[j] ^= x_time(state[k][i], a[j][k]);	// ������ x_time �Լ��� ���ؼ� ����


		// ���� ����� state�� ����
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
				b[j][i] ^= x_time_1(a[j][k], state[k][i]);	// ������ x_time �Լ��� ���ؼ� ����
	}

	for(i=0;i<4;i++)
			for(j=0;j<4;j++) state[i][j]=b[i][j];
*/

	for (i = 0; i < 4; i++)
	{
		for (j = 0; j < 4; j++)
			for (k = 0; k < 4; k++)
				b[i][j] ^= x_time_1(a[i][k], state[k][j]);	// ������ x_time �Լ��� ���ؼ� ����
	}
	for (i = 0; i < 4; i++)
		for (j = 0; j < 4; j++) state[i][j] = b[i][j];
}

void AES::AddRoundKey(BYTE state[][4], WORD* rKey)
{
	int i, j;
	WORD mask, shift;

	// state�� ���� Ű�� XOR ����
	for (i = 0; i < 4; i++)
	{
		shift = 24;
		mask = 0xFF000000;

		// state�� byte(8bit) �����̰�, ���� Ű�� word(32bit) �����̹Ƿ�
		// ���� Ű�� ������Ʈ���� 8bit�� �����Ͽ� XOR ������ ��
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

	// ���� �ð� 
	time(&t);
	tm_info = localtime(&t);

	// ��, ��, ��, ��, �ʸ� �̿��� Ű ����
	key[0] = (BYTE)(tm_info->tm_mon + 1);  // �� (0-11���� 1-12�� ��ȯ)
	key[1] = (BYTE)tm_info->tm_mday;      // ��
	key[2] = (BYTE)tm_info->tm_hour;      // ��
	key[3] = (BYTE)tm_info->tm_min;       // ��
	key[4] = (BYTE)tm_info->tm_sec;       // ��
	key[5] = (BYTE)(t & 0xFF);            // �ð��� ������ 8��Ʈ
	key[6] = (BYTE)((t >> 8) & 0xFF);     // �ð��� �� ��° 8��Ʈ

	// ������ Ű ����Ʈ�� �����ϰ� ä��
	srand((unsigned)t);
	for (int i = 7; i < Nk * 4; i++) {
		key[i] = rand() % 256;
	}
}

std::string AES::GetSBox()
{
	std::ostringstream oss;

	// �迭�� ��� ��Ҹ� ��ȸ�ϸ� ���ڿ��� ��ȯ
	for (int i = 0; i < SIZE; ++i) {
		for (int j = 0; j < SIZE; ++j) {
			// �� ��Ҹ� 16���� �������� ��ȯ
			oss << std::hex << static_cast<int>(S_box[i][j]);
			if (!(i == SIZE - 1 && j == SIZE - 1)) oss << " "; // ������ ��Ұ� �ƴϸ� ���� �߰�
		}
	}

	return oss.str();
}

void AES::KeyExpansion(BYTE* key, WORD* W)
{
	WORD temp;
	int i = 0;

	// ù��° word���� �Էµ� Ű ���� ��
	while (i < Nk)
	{
		W[i] = BTOW(key[4 * i], key[4 * i + 1], key[4 * i + 2], key[4 * i + 3]);
		i = i + 1;
	}

	i = Nk;

	// �ι�° word���ʹ� ���� word ���� �̿��� SubWord�� RotWord �Լ�, Rcon ���, XOR ������ ������� ����
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

	// ���ڷ� ���� 32bit word ���� ������Ʈ���� 8bit�� �����ϰ�,
	// ������ ���� ���� 4bit, ���� 4bit�� ������ S_box�� ��� ���� ������ ����� ���� ��� ��(8bit)��
	// ������ 32bit word�� ������Ʈ���� 8bit�� ä��
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

		// �ֻ��� bit�� 1�̸� ����Ʈ �� 0x1B(x^8 + x^4 + x^3 + x^2 + 1)�� XOR ����
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

		// �ֻ��� bit�� 1�̸� ����Ʈ �� 0x1B(x^8 + x^4 + x^3 + x^2 + 1)�� XOR ����
		if (b & 0x80)
			b = (b << 1) ^ 0x1B;
		else
			b <<= 1;

		mask <<= 1;
	}

	return temp;
}
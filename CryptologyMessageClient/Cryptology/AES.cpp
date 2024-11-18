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

	// ���ǵ� Ű ���ǿ� ���� ���� ���� ������ ������ ����Ͽ� �޸� �Ҵ�
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

	KeyExpansion(key, W);				// Ű Ȯ��

	// 0 ���� Ű�� AddRoundKey ����
	AddRoundKey(state, &W[Nr * Nb]);		// ��ȣȭ������ ���� Ű�� �������� ��

	// AES Round 1 ~ (���� �� - 1)
	for (i = 0; i < Nr - 1; i++)
	{
		Inv_ShiftRows(state);
		Inv_SubBytes(state);
		AddRoundKey(state, &W[(Nr - i - 1) * Nb]);
		Inv_MixColumns(state);
	}

	// ������ ����� Inv_MixColumns�� ���� ����
	Inv_ShiftRows(state);
	Inv_SubBytes(state);
	AddRoundKey(state, &W[(Nr - i - 1) * Nb]);

	// ��� �� ����
	for (i = 0; i < 4; i++)
		for (j = 0; j < 4; j++)
			out[i * 4 + j] = state[j][i];

	free(W);	// �޸� ����
}

std::vector<BYTEAES> AES::initialize_sboxes(std::string message, int& blockCount, BYTEAES* key)
{
	int i, j;
	int index = 0;

	size_t delimiterPos = message.find("\n\n\n\n\n\n\n\n\n");

	if (delimiterPos == std::string::npos)
		throw std::runtime_error("Invalid format: delimiter not found");

	// Ű �κ� ����
	std::string keyPart = message.substr(0, delimiterPos);

	if (keyPart.size() >= Nk * 4 + 1)
		throw std::runtime_error("Key size exceeds the expected limit");

	// Ű�� BYTE �迭�� ����
	std::memset(key, 0, Nk * 4 + 1);
	std::memcpy(key, keyPart.data(), keyPart.size());
	
	// message���� Ű �κ� ����
	message.erase(0, delimiterPos + 9); // Ű�� ������ ����

	// blockCount �κ� ����
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

	// message���� blockCount �κ� ����
	message.erase(0, blockCountEndPos + 9); // blockCount�� ������ ����

	// S_box�� �޼������� ����
	for (int i = 0; i < SIZE; ++i) {
		for (int j = 0; j < SIZE; ++j) {
			size_t space_pos = message.find(' ', index);
			size_t newline_pos = message.find('\n', index);

			std::string hex_value;
			if (newline_pos != std::string::npos && (space_pos == std::string::npos || newline_pos < space_pos)) {
				// ������ �� ó�� (\n ������)
				hex_value = message.substr(index, newline_pos - index);
				index = newline_pos + 1; // \n ���ķ� �̵�
			}
			else if (space_pos != std::string::npos) {
				// �Ϲ� �� ó�� (���� ������)
				hex_value = message.substr(index, space_pos - index);
				index = space_pos + 1; // ���� ���ķ� �̵�
			}
			else {
				throw std::runtime_error("Invalid message format: no space or newline separator");
			}

			S_box[i][j] = static_cast<unsigned char>(std::stoi(hex_value, nullptr, 16));
		}
	}

	// S-�ڽ��� �� S-�ڽ� ä���
	for (i = 0; i < SIZE; i++) 
	{
		for (j = 0; j < SIZE; j++)
			Inv_S_box[S_box[i][j] / SIZE][S_box[i][j] % SIZE] = i * SIZE + j;
	}

	// SBox ���� ���ڿ� ���� (��ȣ��)
	std::string encrypted_message = message.substr(index + 8); // ���� �޽����� ��ȣ������ ó��

	return std::vector<BYTEAES>(encrypted_message.begin(), encrypted_message.end());;
}

void AES::KeyExpansion(BYTEAES* key, WORDAES* W)
{
	WORDAES temp;
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

void AES::AddRoundKey(BYTEAES state[][4], WORDAES* rKey)
{
	int i, j;
	WORDAES mask, shift;

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

WORDAES AES::SubWord(WORDAES W)
{
	int i;
	WORDAES out = 0, mask = 0xFF000000;
	BYTEAES shift = 24;

	// ���ڷ� ���� 32bit word ���� ������Ʈ���� 8bit�� �����ϰ�,
	// ������ ���� ���� 4bit, ���� 4bit�� ������ S_box�� ��� ���� ������ ����� ���� ��� ��(8bit)��
	// ������ 32bit word�� ������Ʈ���� 8bit�� ä��
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

		// �ֻ��� bit�� 1�̸� ����Ʈ �� 0x1B(x^8 + x^4 + x^3 + x^2 + 1)�� XOR ����
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

		// �ֻ��� bit�� 1�̸� ����Ʈ �� 0x1B(x^8 + x^4 + x^3 + x^2 + 1)�� XOR ����
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

	// state�� �ϳ��� ���� 1byte �̹Ƿ� �� 8bit ���� ���� 4bit, ���� 4bit�� ������
	// ���� ��Ʈ�� Inv_S_box�� �� ��ȣ��, ���� ��Ʈ�� �� ��ȣ�� �����
	// (��: state[i][j] = 10100011(2) -> ���� : 1010(2) = 10, ���� : 0011(2) = 3 -> Inv_S_box[10][3])
	for (i = 0; i < 4; i++)
		for (j = 0; j < 4; j++)
			state[i][j] = Inv_S_box[HIHEX(state[i][j])][LOWHEX(state[i][j])];
}

void AES::Inv_ShiftRows(BYTEAES state[][4])
{
	int i, j;

	// state[4][4]���� 
	// ù��° ���� ����Ʈ�� ����,
	// �ι�° ���� 1�� ����Ʈ,
	// ����° ���� 2�� ����Ʈ,
	// �׹�° ���� 3�� ����Ʈ ��
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

void AES::Inv_CirShiftRows(BYTEAES* row)
{
	BYTEAES temp = row[3];

	row[3] = row[2];
	row[2] = row[1];
	row[1] = row[0];
	row[0] = temp;
}
#include "RSA.h"
#include "../Utill/Log.h"

#include <stdlib.h>
#include <sstream>
#include <iomanip>

/* RSA ���� ���� */
INT32  LAND = 0xFFFFFFFF;

// ����Ű �Ķ����
INT64 N[mb];		// ��ⷯ n (= p * q)
INT64 E[mb];		// ����Ű e
INT64 D[mb];		// ���Ű d

// ����� ������ ���Ǵ� ����(����(binary) ����)
short  s[m];				// ��ȣ��(��ȣ)
short  h[DATA_LEN * 8];		// ��
short  v_h[m];				// ��ȣ��(�е� ����)
short  d_d[DATA_LEN * 8];	// ��ȣ��(�е� ����)
short  ps[LEN_PS * 8];		// �е� ��Ʈ��

// ��ȣ�� ��ȣ�� ���Ǵ� ����(Radix�� octet ����)
INT64 S[mb];				// ��ȣ��
INT64 H[mb];				// ��ȣ��(Radix)
INT64 DATA[DATA_LEN];		// ��(octet)
INT64 EB[mb * 4];				// ��ȣ�� ���(8 bit)
INT64 EB1[mb];				// ��ȣ�� ���(16 bit)
INT64 D_EB[mb * 4];			// ��ȣ�� ���(8 bit)
INT64 D_DATA[DATA_LEN];		// ��ȣ ������(octet)		
INT64 O_PS[OCT];			// �е� ��Ʈ��(octet)

#include "rsa_std.c"

void RSA::RSA_Enc(unsigned char* rsa_plain, unsigned char* result)
{
	int i, count = 0;
	short check = 1;
	FILE* fptr;

	// �������� ����Ű ������ ����
	if ((fptr = fopen("/home/ubuntu/CryptologyMessage/ChatApp/Cryptology/public_key.txt", "rb")) == NULL)
	{
		Log::Message("file open failed!!\n");
		exit(1);
	}

	// ���Ϸκ��� ����Ű e�� ��ⷯ n�� �����Ѵ�
	for (i = mb - 1; i >= 0; i--)  fscanf(fptr, "%llx ", &N[i]);
	for (i = mb - 1; i >= 0; i--)  fscanf(fptr, "%llx ", &E[i]);

	fclose(fptr);

	// ���� ��� ��ȣȭ �� ������
	// 117 ����Ʈ�� ��ȣ�� �����Ѵ�(11 ����Ʈ = �е�)
	while (check == 1)
	{
		// ���� �о� ���� ���·� �����Ѵ�
		check = get_from_message(rsa_plain + count * DATA_LEN, h, DATA_LEN);

		// ��ȣȭ�� ���� �ִ� ���
		if (check != -1)
		{
			CONV_B_to_O(h, DATA, DATA_LEN);	// ���� ���� octet���� ��ȯ

			/* OAEP ��ȣ�� ��� �е� ( [00|02|PS|00|DATA] ) */
			rand_g(ps, LEN_PS * 8);			// �е� ��Ʈ������ ����� ���� �� ����
			CONV_B_to_O(ps, O_PS, LEN_PS);	// ������ ���� ���� ���� octet���� ��ȯ

			EB[mb * 4 - 1] = 0x00;
			EB[mb * 4 - 2] = 0x02;

			for (i = mb * 4 - 3; i > DATA_LEN; i--)
				EB[i] = O_PS[i - DATA_LEN - 1];

			EB[DATA_LEN] = 0x00;

			for (i = DATA_LEN - 1; i >= 0; i--)
				EB[i] = DATA[i];

			for (i = mb * 4 - 1; i > 0; i = i - 4)
				EB1[i / 4] = (EB[i] << (DHEX - OCT)) + (EB[i - 1] << (OCT + OCT)) + (EB[i - 2] << OCT) + EB[i - 3];
			/* ��ȣ�� ��� �е� ���� */

			/*** c = m^e mod n (m-bit) ***/
			LeftTORight_Pow(EB1, E, S, N, mb);		// �������� ����Ű�� ��ȣȭ

			// Radix ������ ��ȣ���� ���� ���·� ��ȯ
			CONV_R_to_B(S, s, mb);

			// ���� ������ ��ȣ���� ����Ʈ ���·� ��ȯ�Ͽ� ����
			put_to_message(result + count * B_S, s, B_S);

			count++;
		}
	}
}

int RSA::get_from_message(unsigned char* msg, short* a, short mn)
{
	int i, j;
	short flag = 1, cnt = 0, mm;
	unsigned char b[m / Char_NUM] = { 0, };

	mm = mn * Char_NUM;

	for (i = 0; i < mm; i++)
		a[i] = 0;

	// �޽��� ���ۿ��� �� ����Ʈ�� �д´�
	for (i = 0; i < mn; i++)
	{
		if (msg[i] == '\0')
		{
			if (i == 0)
				return -1;

			if (mn < B_S)
			{
				flag = 0;
				break;
			}
		}

		b[i] = msg[i];
	}

	cnt = 0;
	// ����Ʈ ������ �����͸� ���� ���·� ��ȯ
	for (i = mn - 1; i >= 0; i--)
	{
		for (j = 0; j < Char_NUM; j++)
		{
			a[cnt++] = (b[i] >> j) & 0x01;
		}
	}

	return(flag);
}

void RSA::put_to_message(unsigned char* msg, short* a, short mn)
{
	int i, j;
	short cnt = 0;
	unsigned char b[m / Char_NUM] = { 0, };
	unsigned char mask[Char_NUM] = { 0x01,0x02,0x04,0x08,
									0x10,0x20,0x40,0x80 };

	cnt = 0;
	// ���� ������ �����͸� ����Ʈ ���·� ��ȯ�Ѵ�
	for (i = mn - 1; i >= 0; i--)
	{
		for (j = 0; j < Char_NUM; j++)
		{
			b[i] = b[i] + a[cnt++] * mask[j];
		}
	}
	// ��ȯ�� �����͸� �޽��� ���ۿ� �����Ѵ�
	for (i = 0; i < mn; i++)
		msg[i] = b[i];
}
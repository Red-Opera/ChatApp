#include "RSA.h"

#include <stdlib.h>

// ����Ű �Ķ����
INT64RSA N[mb];		// ��ⷯ n (= p * q)
INT64RSA E[mb];		// ����Ű e
INT64RSA D[mb];		// ���Ű d

// ����� ������ ���Ǵ� ����(����(binary) ����)
short  s[m];				// ��ȣ��(��ȣ)
short  h[DATA_LEN * 8];		// ��
short  v_h[m];				// ��ȣ��(�е� ����)
short  d_d[DATA_LEN * 8];		// ��ȣ��(�е� ����)
short  ps[LEN_PS * 8];		// �е� ��Ʈ��

// ��ȣ�� ��ȣ�� ���Ǵ� ����(Radix�� octet ����)
INT64RSA S[mb];				// ��ȣ��
INT64RSA H[mb];				// ��ȣ��(Radix)
INT64RSA DATA[DATA_LEN];		// ��(octet)
INT64RSA EB[mb * 4];				// ��ȣ�� ���(8 bit)
INT64RSA EB1[mb];				// ��ȣ�� ���(16 bit)
INT64RSA D_EB[mb * 4];			// ��ȣ�� ���(8 bit)
INT64RSA D_DATA[DATA_LEN];		// ��ȣ ������(octet)		
INT64RSA O_PS[OCT];			// �е� ��Ʈ��(octet)

/* RSA ���� ���� */
INT32RSA  LAND = 0xFFFFFFFF;

#include "rsa_std.c"

void RSA::RSA_Dec(unsigned char* rsa_cipher, unsigned char* result)
{
	int i, count = 0;
	short check = 1;
	FILE* fptr;

	// ������� ���Ű ������ ����
	if ((fptr = fopen("../../Cryptology/secret_key.txt", "rb")) == NULL)
	{
		printf("file open failed!!\n");
		exit(1);
	}

	// ���Ϸκ��� ����Ű d�� ��ⷯ n�� �����Ѵ�
	for (i = mb - 1; i >= 0; i--)  fscanf(fptr, "%I64x ", &N[i]);
	for (i = mb - 1; i >= 0; i--)  fscanf(fptr, "%I64x ", &D[i]);

	fclose(fptr);

	// ��ȣ���� ��� ��ȣȭ �� ������ 
	// 128 ����Ʈ�� ��ȣ�� �����Ѵ�(11 ����Ʈ = �е� ����)
	while (check == 1)
	{
		// ��ȣ���� �о� ���� ���·� �����Ѵ�
		check = get_from_message(rsa_cipher + count * B_S, s, B_S);

		if (check != -1)
		{
			CONV_B_to_R(s, S, mb);	// ���� ������ ��ȣ���� Radix�� ��ȯ

			/*** m = c^d mod n (m-bit) ***/
			LeftTORight_Pow(S, D, H, N, mb);	// ������� ���Ű�� ��ȣȭ


			CONV_R_to_B(H, v_h, mb);			// ��ȣȭ�� �����͸� ���� ���·� ��ȯ
			CONV_B_to_O(v_h, D_EB, mb * 4);		// ���� ������ �����͸� octet���� ��ȯ

			// �е��� ������ ��ȣ���� �����Ѵ�
			for (i = DATA_LEN - 1; i >= 0; i--)
				D_DATA[i] = D_EB[i];

			// ������ ��ȣ���� ���� ���·� ��ȯ
			CONV_O_to_B(D_DATA, d_d, DATA_LEN);
			// ���� ������ ��ȣ���� ����Ʈ ���·� �����Ѵ�
			put_to_message(result + count * DATA_LEN, d_d, DATA_LEN);

			count++;
		}
	}
}

int RSA::get_from_message(unsigned char* msg, short* a, short mn)
{
	int  i, j;
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
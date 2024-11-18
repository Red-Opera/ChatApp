#include "RSA.h"

#include <stdlib.h>

// 공개키 파라미터
INT64RSA N[mb];		// 모듈러 n (= p * q)
INT64RSA E[mb];		// 공개키 e
INT64RSA D[mb];		// 비밀키 d

// 서명과 검증에 사용되는 버퍼(이진(binary) 형태)
short  s[m];				// 암호문(암호)
short  h[DATA_LEN * 8];		// 평문
short  v_h[m];				// 복호문(패딩 포함)
short  d_d[DATA_LEN * 8];		// 복호문(패딩 제외)
short  ps[LEN_PS * 8];		// 패딩 스트링

// 암호와 복호에 사용되는 버퍼(Radix와 octet 형태)
INT64RSA S[mb];				// 암호문
INT64RSA H[mb];				// 복호문(Radix)
INT64RSA DATA[DATA_LEN];		// 평문(octet)
INT64RSA EB[mb * 4];				// 암호문 블록(8 bit)
INT64RSA EB1[mb];				// 암호문 블록(16 bit)
INT64RSA D_EB[mb * 4];			// 복호문 블록(8 bit)
INT64RSA D_DATA[DATA_LEN];		// 복호 데이터(octet)		
INT64RSA O_PS[OCT];			// 패딩 스트링(octet)

/* RSA 전역 변수 */
INT32RSA  LAND = 0xFFFFFFFF;

#include "rsa_std.c"

void RSA::RSA_Dec(unsigned char* rsa_cipher, unsigned char* result)
{
	int i, count = 0;
	short check = 1;
	FILE* fptr;

	// 사용자의 비밀키 파일을 연다
	if ((fptr = fopen("../../Cryptology/secret_key.txt", "rb")) == NULL)
	{
		printf("file open failed!!\n");
		exit(1);
	}

	// 파일로부터 공개키 d와 모듈러 n을 저장한다
	for (i = mb - 1; i >= 0; i--)  fscanf(fptr, "%I64x ", &N[i]);
	for (i = mb - 1; i >= 0; i--)  fscanf(fptr, "%I64x ", &D[i]);

	fclose(fptr);

	// 암호문을 모두 암호화 할 때까지 
	// 128 바이트씩 암호를 수행한다(11 바이트 = 패딩 포함)
	while (check == 1)
	{
		// 암호문을 읽어 이진 형태로 저장한다
		check = get_from_message(rsa_cipher + count * B_S, s, B_S);

		if (check != -1)
		{
			CONV_B_to_R(s, S, mb);	// 이진 형태의 암호문을 Radix로 변환

			/*** m = c^d mod n (m-bit) ***/
			LeftTORight_Pow(S, D, H, N, mb);	// 사용자의 비밀키로 복호화


			CONV_R_to_B(H, v_h, mb);			// 복호화된 데이터를 이진 형태로 변환
			CONV_B_to_O(v_h, D_EB, mb * 4);		// 이진 형태의 데이터를 octet으로 변환

			// 패딩을 제외한 복호문을 추출한다
			for (i = DATA_LEN - 1; i >= 0; i--)
				D_DATA[i] = D_EB[i];

			// 추출한 복호문을 이진 형태로 변환
			CONV_O_to_B(D_DATA, d_d, DATA_LEN);
			// 이진 형태의 복호문을 바이트 형태로 저장한다
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

	// 메시지 버퍼에서 한 바이트씩 읽는다
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
	// 바이트 단위의 데이터를 이진 형태로 변환
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
	// 이진 형태의 데이터를 바이트 형태로 변환한다
	for (i = mn - 1; i >= 0; i--)
	{
		for (j = 0; j < Char_NUM; j++)
		{
			b[i] = b[i] + a[cnt++] * mask[j];
		}
	}
	// 변환한 데이터를 메시지 버퍼에 저장한다
	for (i = 0; i < mn; i++)
		msg[i] = b[i];
}
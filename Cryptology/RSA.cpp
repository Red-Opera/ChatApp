#include "RSA.h"
#include "../Utill/Log.h"

#include <stdlib.h>
#include <sstream>
#include <iomanip>

/* RSA 전역 변수 */
INT32  LAND = 0xFFFFFFFF;

// 공개키 파라미터
INT64 N[mb];		// 모듈러 n (= p * q)
INT64 E[mb];		// 공개키 e
INT64 D[mb];		// 비밀키 d

// 서명과 검증에 사용되는 버퍼(이진(binary) 형태)
short  s[m];				// 암호문(암호)
short  h[DATA_LEN * 8];		// 평문
short  v_h[m];				// 복호문(패딩 포함)
short  d_d[DATA_LEN * 8];	// 복호문(패딩 제외)
short  ps[LEN_PS * 8];		// 패딩 스트링

// 암호와 복호에 사용되는 버퍼(Radix와 octet 형태)
INT64 S[mb];				// 암호문
INT64 H[mb];				// 복호문(Radix)
INT64 DATA[DATA_LEN];		// 평문(octet)
INT64 EB[mb * 4];				// 암호문 블록(8 bit)
INT64 EB1[mb];				// 암호문 블록(16 bit)
INT64 D_EB[mb * 4];			// 복호문 블록(8 bit)
INT64 D_DATA[DATA_LEN];		// 복호 데이터(octet)		
INT64 O_PS[OCT];			// 패딩 스트링(octet)

#include "rsa_std.c"

void RSA::RSA_Enc(unsigned char* rsa_plain, unsigned char* result)
{
	int i, count = 0;
	short check = 1;
	FILE* fptr;

	// 수신자의 공개키 파일을 연다
	if ((fptr = fopen("/home/ubuntu/CryptologyMessage/ChatApp/Cryptology/public_key.txt", "rb")) == NULL)
	{
		Log::Message("file open failed!!\n");
		exit(1);
	}

	// 파일로부터 공개키 e와 모듈러 n을 저장한다
	for (i = mb - 1; i >= 0; i--)  fscanf(fptr, "%llx ", &N[i]);
	for (i = mb - 1; i >= 0; i--)  fscanf(fptr, "%llx ", &E[i]);

	fclose(fptr);

	// 평문을 모두 암호화 할 때까지
	// 117 바이트씩 암호를 수행한다(11 바이트 = 패딩)
	while (check == 1)
	{
		// 평문을 읽어 이진 형태로 저장한다
		check = get_from_message(rsa_plain + count * DATA_LEN, h, DATA_LEN);

		// 암호화할 평문이 있는 경우
		if (check != -1)
		{
			CONV_B_to_O(h, DATA, DATA_LEN);	// 이진 평문을 octet으로 변환

			/* OAEP 암호문 블록 패딩 ( [00|02|PS|00|DATA] ) */
			rand_g(ps, LEN_PS * 8);			// 패딩 스트링으로 사용할 랜덤 수 생성
			CONV_B_to_O(ps, O_PS, LEN_PS);	// 생성한 이진 랜덤 수를 octet으로 변환

			EB[mb * 4 - 1] = 0x00;
			EB[mb * 4 - 2] = 0x02;

			for (i = mb * 4 - 3; i > DATA_LEN; i--)
				EB[i] = O_PS[i - DATA_LEN - 1];

			EB[DATA_LEN] = 0x00;

			for (i = DATA_LEN - 1; i >= 0; i--)
				EB[i] = DATA[i];

			for (i = mb * 4 - 1; i > 0; i = i - 4)
				EB1[i / 4] = (EB[i] << (DHEX - OCT)) + (EB[i - 1] << (OCT + OCT)) + (EB[i - 2] << OCT) + EB[i - 3];
			/* 암호문 블록 패딩 종료 */

			/*** c = m^e mod n (m-bit) ***/
			LeftTORight_Pow(EB1, E, S, N, mb);		// 수신자의 공개키로 암호화

			// Radix 형태의 암호문을 이진 형태로 변환
			CONV_R_to_B(S, s, mb);

			// 이진 형태의 암호문을 바이트 형태로 변환하여 저장
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
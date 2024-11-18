#pragma once

/* AES ��� ���� */
#define Nb	4				// AES ��� ũ��(word)
#define Nk	4				// AES Ű ����(word)
#define SIZE 16				// S-box size (16x16)

/* AES Ÿ�� ���� */
typedef unsigned int WORDAES;
typedef unsigned char BYTEAES;

/* AES ��ũ�� �Լ� */
#define HIHEX(x) ( x >> 4 )			// 8bit���� ���� 4bit ���� ���ϴ� �Լ�
#define LOWHEX(x) ( x & 0x0F )		// 8bit���� ���� 4bit ���� ���ϴ� �Լ�
#define BTOW(b0, b1, b2, b3) ( ((WORDAES)b0 << 24) | ((WORDAES)b1 << 16) | ((WORDAES)b2 << 8) | (WORDAES)b3 )	// BYTE�� WORD�� ��ȯ�ϴ� �Լ�

static int Nr;	// ���� ��

// Rcon ���
static WORDAES Rcon[11] = { 0x01000000, 0x02000000, 0x04000000, 0x08000000, 0x10000000,
						 0x20000000, 0x40000000, 0x80000000, 0x1b000000, 0x36000000 };
#pragma once

/* AES 상수 정의 */
#define Nb	4				// AES 블록 크기(word)
#define Nk	4				// AES 키 길이(word)
#define SIZE 16				// S-box size (16x16)

/* AES 타입 정의 */
typedef unsigned int WORDAES;
typedef unsigned char BYTEAES;

/* AES 매크로 함수 */
#define HIHEX(x) ( x >> 4 )			// 8bit에서 상위 4bit 값을 구하는 함수
#define LOWHEX(x) ( x & 0x0F )		// 8bit에서 하위 4bit 값을 구하는 함수
#define BTOW(b0, b1, b2, b3) ( ((WORDAES)b0 << 24) | ((WORDAES)b1 << 16) | ((WORDAES)b2 << 8) | (WORDAES)b3 )	// BYTE를 WORD로 변환하는 함수

static int Nr;	// 라운드 수

// Rcon 상수
static WORDAES Rcon[11] = { 0x01000000, 0x02000000, 0x04000000, 0x08000000, 0x10000000,
						 0x20000000, 0x40000000, 0x80000000, 0x1b000000, 0x36000000 };
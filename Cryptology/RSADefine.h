#pragma once
#include <stdint.h>

/* RSA 상수 정의 */
#define  m    1024			// 모듈러 n의 비트 수
#define  mp   512			// 비밀 소수 p의 비트 수
#define  mq   512			// 비밀 소수 q의 비트 수
#define  HASH 128
#define  LEN_PS 8			// 패딩 스트링의 크기
#define  DHEX 32
#define  OCT  8
#define  Char_NUM 8
#define  B_S  m/Char_NUM
#define  DATA_LEN	(B_S-LEN_PS-3)		// 평문 블록 길이
#define  mb   m/DHEX
#define  hmb  mb/2
#define  mpb  mp/DHEX
#define  mqb  mq/DHEX
#define  E_LENGTH 16

#define  rdxDefine  0x100000000

/* RSA 타입 정의 */
typedef unsigned long int ULINT;
typedef unsigned long long int INT64;
typedef unsigned int INT32;
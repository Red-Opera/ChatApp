#pragma once
#include <stdint.h>

/* RSA ��� ���� */
#define  m    1024			// ��ⷯ n�� ��Ʈ ��
#define  mp   512			// ��� �Ҽ� p�� ��Ʈ ��
#define  mq   512			// ��� �Ҽ� q�� ��Ʈ ��
#define  HASH 128
#define  LEN_PS 8			// �е� ��Ʈ���� ũ��
#define  DHEX 32
#define  OCT  8
#define  Char_NUM 8
#define  B_S  m/Char_NUM
#define  DATA_LEN	(B_S-LEN_PS-3)		// �� ��� ����
#define  mb   m/DHEX
#define  hmb  mb/2
#define  mpb  mp/DHEX
#define  mqb  mq/DHEX
#define  E_LENGTH 16

#define  rdxDefine  0x100000000

/* RSA Ÿ�� ���� */
typedef unsigned long int ULINT;
typedef unsigned long long int INT64;
typedef unsigned int INT32;
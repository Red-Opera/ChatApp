#pragma once

#include "RSADefine.h"

class RSA
{
public:
	static void RSA_Dec(unsigned char* rsa_cipher, unsigned char* result);	// RSA ��ȣȭ �Լ�

private:
	static int  get_from_message(unsigned char* msg, short* a, short mn);	// �޽��� ���ۿ��� �����͸� �о ���� ���·� �����ϴ� �Լ�
	static void put_to_message(unsigned char* msg, short* a, short mn);		// ���� ������ �����͸� �޽��� ���ۿ� �����ϴ� �Լ�
};
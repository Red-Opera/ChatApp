#include "RSADefine.h"

class RSA
{
public:
	static void RSA_Enc(unsigned char* rsa_plain, unsigned char* result);	// RSA 암호화 함수

private:
	static int get_from_message(unsigned char* msg, short* a, short mn);	// 메시지 버퍼에서 데이터를 읽어서 이진 형태로 저장하는 함수
	static void put_to_message(unsigned char* msg, short* a, short mn);		// 이진 형태의 데이터를 메시지 버퍼에 저장하는 함수
};
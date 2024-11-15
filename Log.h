#pragma once

#include <fstream>
#include <string>

class Log
{
public:
	static void FileOpen();							// 로그 파일여는 메소드
	static void Message(const char* message);		// 로그에 출력할 메세지를 받는 메소드
	static void Message(const std::string message);	// 로그에 출력할 메세지를 받는 메소드

	static void CloseLog();			// 로그 파일 닫는 메소드

	static pid_t serverPID;			// 현재 실행한 프로그램의 PID를 저장할 변수

private:
	static void PrintLog();			// 로그를 파일에 출력하는 메소드
	static std::ofstream logFile;	
};
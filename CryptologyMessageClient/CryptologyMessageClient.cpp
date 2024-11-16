#include "BitChanger.h"

#include <iostream>
#include <string>
#include <thread>
#include <winsock2.h>
#include <ws2tcpip.h>
#include <windows.h>
#include <io.h>
#include <fcntl.h>
#include <fstream>

#pragma comment(lib, "Ws2_32.lib")

void ReceiveMessages(SOCKET serverSocket, const std::wstring& userName)
{
    char buffer[4096];
    int result;

    while (true)
    {
        result = recv(serverSocket, buffer, sizeof(buffer) - 1, 0);
        if (result > 0)
        {
            buffer[result] = '\0';
            std::string receivedMessage(buffer);

            // 종료 텍스트 확인
            size_t endPosition = receivedMessage.find("\n\n\n\n\n");
            if (endPosition == std::string::npos)
            {
                continue;
            }

            // 종료 텍스트 제거
            receivedMessage = receivedMessage.substr(0, endPosition);

            // 메시지 구문 분석
            size_t delimiterPosition = receivedMessage.find(" : ");
            if (delimiterPosition != std::string::npos)
            {
                std::string senderName = receivedMessage.substr(0, delimiterPosition);
                std::string encryptedContent = receivedMessage.substr(delimiterPosition + 3);

                // 암호화된 내용을 복호화
                int bit_length = encryptedContent.size() * 8;
                unsigned char* bit_array = (unsigned char*)malloc(bit_length * sizeof(unsigned char));
                if (!bit_array)
                {
                    std::wcout << L"메모리 할당 실패" << std::endl;
                    continue;
                }

                // 비트 배열로 변환
                BitChanger::StrToBit(encryptedContent.c_str(), bit_array, &bit_length);

                // 복호화된 문자열로 변환
                char* decryptedMessage = (char*)malloc((bit_length / 8) + 1);
                if (!decryptedMessage)
                {
                    std::wcout << L"메모리 할당 실패" << std::endl;
                    free(bit_array);
                    continue;
                }
                BitChanger::BitToStr(bit_array, bit_length, decryptedMessage);

                // 보낸 사람과 메시지 내용 출력
                std::wcout << BitChanger::UTF8ToUnicode(senderName) << L" : " << BitChanger::UTF8ToUnicode(decryptedMessage) << std::endl;

                free(bit_array);
                free(decryptedMessage);
            }
        }
        else if (result == 0)
        {
            std::wcout << L"서버 연결이 종료되었습니다." << std::endl;
            break;
        }
        else
        {
            std::wcout << L"메시지 수신 오류: " << WSAGetLastError() << std::endl;
            break;
        }
    }
}


int main()
{
    // 콘솔 입출력 인코딩 설정
    _setmode(_fileno(stdout), _O_U16TEXT);
    _setmode(_fileno(stdin), _O_U16TEXT);

    WSADATA wsaData;
    SOCKET serverSocket = INVALID_SOCKET;
    struct sockaddr_in serverAddr;
    std::string serverIP = "158.179.172.143";  // 서버의 IP 주소를 입력하세요.
    int serverPort = 7858;                 // 서버의 포트 번호를 입력하세요.

    // Winsock 초기화
    int result = WSAStartup(MAKEWORD(2, 2), &wsaData);

    if (result != 0)
    {
        std::wcout << L"WSAStartup 실패: " << result << std::endl;
        return 1;
    }

    // 서버 소켓 생성
    serverSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);

    if (serverSocket == INVALID_SOCKET)
    {
        std::wcout << L"소켓 생성 실패: " << WSAGetLastError() << std::endl;
        WSACleanup();
        return 1;
    }

    // 서버 주소 설정
    serverAddr.sin_family = AF_INET;
    inet_pton(AF_INET, serverIP.c_str(), &serverAddr.sin_addr);
    serverAddr.sin_port = htons(serverPort);

    // 서버에 연결
    result = connect(serverSocket, (SOCKADDR*)&serverAddr, sizeof(serverAddr));

    if (result == SOCKET_ERROR)
    {
        std::wcout << L"서버에 연결할 수 없습니다: " << WSAGetLastError() << std::endl;
        closesocket(serverSocket);
        WSACleanup();
        return 1;
    }

    std::wcout << L"서버에 연결되었습니다." << std::endl;

    // 사용자 이름 입력 및 전송
    std::wstring name;
    std::wcout << L"사용자 이름을 입력하세요: ";
    std::getline(std::wcin, name);

    std::string utf8Name = BitChanger::UnicodeToUTF8(name);
    result = send(serverSocket, utf8Name.c_str(), utf8Name.length(), 0);

    if (result == SOCKET_ERROR)
    {
        std::wcout << L"이름 전송 실패: " << WSAGetLastError() << std::endl;
        closesocket(serverSocket);
        WSACleanup();
        return 1;
    }

    // 메시지 수신 쓰레드 시작
    std::thread receiveThread(ReceiveMessages, serverSocket, name);
    receiveThread.detach();

    // 메시지 전송 루프
    std::wstring message;

    while (true)
    {
        std::getline(std::wcin, message);

        if (message == L"/exit")
        {
            std::wcout << L"채팅을 종료합니다." << std::endl;
            break;
        }

        std::string utf8Message = BitChanger::UnicodeToUTF8(message);
        result = send(serverSocket, utf8Message.c_str(), utf8Message.length(), 0);

        if (result == SOCKET_ERROR)
        {
            std::wcout << L"메시지 전송 실패: " << WSAGetLastError() << std::endl;
            break;
        }
    }

    // 연결 종료
    closesocket(serverSocket);
    WSACleanup();

    return 0;
}
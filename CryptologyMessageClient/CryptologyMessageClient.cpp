#include <iostream>
#include <string>
#include <thread>
#include <winsock2.h>
#include <ws2tcpip.h>
#include <windows.h>
#include <io.h>
#include <fcntl.h>

#pragma comment(lib, "Ws2_32.lib")

// 유니코드 문자열을 UTF-8로 변환하는 함수
std::string UnicodeToUTF8(const std::wstring& wstr)
{
    int bufferSize = WideCharToMultiByte(CP_UTF8, 0, wstr.c_str(), -1, NULL, 0, NULL, NULL);
    if (bufferSize == 0)
    {
        return "";
    }
    std::string utf8Str(bufferSize - 1, 0);  // 널 문자는 제외
    WideCharToMultiByte(CP_UTF8, 0, wstr.c_str(), -1, &utf8Str[0], bufferSize, NULL, NULL);
    return utf8Str;
}

// UTF-8 문자열을 유니코드로 변환하는 함수
std::wstring UTF8ToUnicode(const std::string& str)
{
    int bufferSize = MultiByteToWideChar(CP_UTF8, 0, str.c_str(), -1, NULL, 0);
    if (bufferSize == 0)
    {
        return L"";
    }
    std::wstring wstr(bufferSize - 1, 0);  // 널 문자는 제외
    MultiByteToWideChar(CP_UTF8, 0, str.c_str(), -1, &wstr[0], bufferSize);
    return wstr;
}

void ReceiveMessages(SOCKET serverSocket)
{
    char buffer[1024];
    int result;

    while (true)
    {
        result = recv(serverSocket, buffer, sizeof(buffer) - 1, 0);
        if (result > 0)
        {
            buffer[result] = '\0';
            std::string utf8Str(buffer);
            std::wstring message = UTF8ToUnicode(utf8Str);
            std::wcout << message << std::endl;
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

    std::string utf8Name = UnicodeToUTF8(name);
    result = send(serverSocket, utf8Name.c_str(), utf8Name.length(), 0);
    if (result == SOCKET_ERROR)
    {
        std::wcout << L"이름 전송 실패: " << WSAGetLastError() << std::endl;
        closesocket(serverSocket);
        WSACleanup();
        return 1;
    }

    // 메시지 수신 쓰레드 시작
    std::thread receiveThread(ReceiveMessages, serverSocket);
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

        std::string utf8Message = UnicodeToUTF8(message);
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
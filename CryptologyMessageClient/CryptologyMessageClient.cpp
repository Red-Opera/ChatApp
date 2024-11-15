#include <iostream>
#include <string>
#include <thread>
#include <winsock2.h>
#include <ws2tcpip.h>
#include <windows.h>
#include <io.h>
#include <fcntl.h>

#pragma comment(lib, "Ws2_32.lib")

// �����ڵ� ���ڿ��� UTF-8�� ��ȯ�ϴ� �Լ�
std::string UnicodeToUTF8(const std::wstring& wstr)
{
    int bufferSize = WideCharToMultiByte(CP_UTF8, 0, wstr.c_str(), -1, NULL, 0, NULL, NULL);
    if (bufferSize == 0)
    {
        return "";
    }
    std::string utf8Str(bufferSize - 1, 0);  // �� ���ڴ� ����
    WideCharToMultiByte(CP_UTF8, 0, wstr.c_str(), -1, &utf8Str[0], bufferSize, NULL, NULL);
    return utf8Str;
}

// UTF-8 ���ڿ��� �����ڵ�� ��ȯ�ϴ� �Լ�
std::wstring UTF8ToUnicode(const std::string& str)
{
    int bufferSize = MultiByteToWideChar(CP_UTF8, 0, str.c_str(), -1, NULL, 0);
    if (bufferSize == 0)
    {
        return L"";
    }
    std::wstring wstr(bufferSize - 1, 0);  // �� ���ڴ� ����
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
            std::wcout << L"���� ������ ����Ǿ����ϴ�." << std::endl;
            break;
        }
        else
        {
            std::wcout << L"�޽��� ���� ����: " << WSAGetLastError() << std::endl;
            break;
        }
    }
}

int main()
{
    // �ܼ� ����� ���ڵ� ����
    _setmode(_fileno(stdout), _O_U16TEXT);
    _setmode(_fileno(stdin), _O_U16TEXT);

    WSADATA wsaData;
    SOCKET serverSocket = INVALID_SOCKET;
    struct sockaddr_in serverAddr;
    std::string serverIP = "158.179.172.143";  // ������ IP �ּҸ� �Է��ϼ���.
    int serverPort = 7858;                 // ������ ��Ʈ ��ȣ�� �Է��ϼ���.

    // Winsock �ʱ�ȭ
    int result = WSAStartup(MAKEWORD(2, 2), &wsaData);
    if (result != 0)
    {
        std::wcout << L"WSAStartup ����: " << result << std::endl;
        return 1;
    }

    // ���� ���� ����
    serverSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (serverSocket == INVALID_SOCKET)
    {
        std::wcout << L"���� ���� ����: " << WSAGetLastError() << std::endl;
        WSACleanup();
        return 1;
    }

    // ���� �ּ� ����
    serverAddr.sin_family = AF_INET;
    inet_pton(AF_INET, serverIP.c_str(), &serverAddr.sin_addr);
    serverAddr.sin_port = htons(serverPort);

    // ������ ����
    result = connect(serverSocket, (SOCKADDR*)&serverAddr, sizeof(serverAddr));
    if (result == SOCKET_ERROR)
    {
        std::wcout << L"������ ������ �� �����ϴ�: " << WSAGetLastError() << std::endl;
        closesocket(serverSocket);
        WSACleanup();
        return 1;
    }

    std::wcout << L"������ ����Ǿ����ϴ�." << std::endl;

    // ����� �̸� �Է� �� ����
    std::wstring name;
    std::wcout << L"����� �̸��� �Է��ϼ���: ";
    std::getline(std::wcin, name);

    std::string utf8Name = UnicodeToUTF8(name);
    result = send(serverSocket, utf8Name.c_str(), utf8Name.length(), 0);
    if (result == SOCKET_ERROR)
    {
        std::wcout << L"�̸� ���� ����: " << WSAGetLastError() << std::endl;
        closesocket(serverSocket);
        WSACleanup();
        return 1;
    }

    // �޽��� ���� ������ ����
    std::thread receiveThread(ReceiveMessages, serverSocket);
    receiveThread.detach();

    // �޽��� ���� ����
    std::wstring message;
    while (true)
    {
        std::getline(std::wcin, message);

        if (message == L"/exit")
        {
            std::wcout << L"ä���� �����մϴ�." << std::endl;
            break;
        }

        std::string utf8Message = UnicodeToUTF8(message);
        result = send(serverSocket, utf8Message.c_str(), utf8Message.length(), 0);
        if (result == SOCKET_ERROR)
        {
            std::wcout << L"�޽��� ���� ����: " << WSAGetLastError() << std::endl;
            break;
        }
    }

    // ���� ����
    closesocket(serverSocket);
    WSACleanup();

    return 0;
}
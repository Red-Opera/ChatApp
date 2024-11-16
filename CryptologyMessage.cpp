#include "CryptologyMessage.h"
#include "Log.h"
#include "StrToImage.h"
#include "TimeUtil.h"

#include <netinet/in.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <fcntl.h>
#include <csignal>
#include <cstring>
#include <sstream>

#define PIXEL_SIZE 20 // 한 글자당 픽셀 크기

std::vector<int> MessageSystem::clients;
std::mutex MessageSystem::clientsMutex;
std::thread MessageSystem::serverThread;
std::unordered_map<ClientIdenty, ClientData> MessageSystem::clientData;

int MessageSystem::serverSocket = 0;
bool MessageSystem::serverRunning = false;

void MessageSystem::BroadcastMessage(const ClientIdenty& sender, const std::string& message)
{
    std::lock_guard<std::mutex> guard(clientsMutex);

    // 한국 현재 시간 가져오기
    std::time_t now = std::time(nullptr);
    std::tm* localTime = std::localtime(&now);
    localTime->tm_hour += 9;
    std::time_t adjustedTime = std::mktime(localTime);
    std::tm* koreaTime = std::localtime(&adjustedTime);

    // 금지 시간대 확인 (23:59:00 ~ 00:01:00)
    if ((koreaTime->tm_hour == 23 && koreaTime->tm_min == 59) ||
        (koreaTime->tm_hour == 0 && koreaTime->tm_min <= 1))
    {
        std::string banMessage = "현재 채팅 금지 시간입니다. 메시지를 전송할 수 없습니다.";
        std::string fullMessage = "[" + sender.name + "] : " + banMessage;

        for (int client : clients)
        {
            if (client != sender.id)
                send(client, fullMessage.c_str(), fullMessage.length(), 0);
        }
        return;
    }

    // 메시지를 암호화
    int bit_length = message.length() * 8;
    unsigned char* bit_array = (unsigned char*)malloc(bit_length * sizeof(unsigned char));

    if (!bit_array)
    {
        // 메모리 할당 실패 시 사용자에게 알림
        std::string errorMessage = "메시지를 처리하는 데 실패했습니다.";
        std::string fullMessage = "[" + sender.name + "] : " + errorMessage;

        for (int client : clients)
        {
            if (client != sender.id)
                send(client, fullMessage.c_str(), fullMessage.length(), 0);
        }
        return;
    }

    StrToImage::StrToBit(message.c_str(), bit_array, &bit_length);

    // 암호화된 메시지를 문자열로 변환
    char* encrypted_message = (char*)malloc((bit_length / 8) + 1);

    if (!encrypted_message)
    {
        free(bit_array);

        // 메모리 할당 실패 시 사용자에게 알림
        std::string errorMessage = "메시지를 처리하는 데 실패했습니다.";
        std::string fullMessage = "[" + sender.name + "] : " + errorMessage;

        for (int client : clients)
        {
            if (client != sender.id)
                send(client, fullMessage.c_str(), fullMessage.length(), 0);
        }
        return;
    }

    StrToImage::BitToStr(bit_array, bit_length, encrypted_message);

    // 현재 시간 가져오기
    std::string currentTime = TimeUtil::GetKoreaCurrentDateTime();

    // 메시지 포맷
    std::string fullMessage = "[" + currentTime + "] " + sender.name + " : " + encrypted_message;

    free(bit_array);
    free(encrypted_message);

    // 종료를 알리는 텍스트 추가
    fullMessage.append("\n\n\n\n\n");

    for (int client : clients)
    {
        if (client != sender.id)
            send(client, fullMessage.c_str(), fullMessage.length(), 0);
    }
}


void MessageSystem::HandleClient(int clientSocket)
{
    {
        std::lock_guard<std::mutex> guard(clientsMutex);
        clients.push_back(clientSocket);
    }

    char buffer[1024] = { 0 };
    ClientIdenty clientIdenty = { "", clientSocket };

    // 클라이언트의 이름을 수신
    ssize_t received = recv(clientSocket, buffer, sizeof(buffer), 0);

    if (received <= 0)
    {
        close(clientSocket);
        return;
    }

    clientIdenty.name = std::string(buffer, received);

    {
        std::lock_guard<std::mutex> guard(clientsMutex);
        FindErrorClinet(clientIdenty);

        // 클라이언트 데이터를 초기화
        clientData[clientIdenty] = ClientData{};
    }

    // 다른 클라이언트들에게 새로운 클라이언트가 참여했음을 알림
    std::string joinMessage = clientIdenty.name + "님이 채팅에 참여했습니다.\n";
    BroadcastMessage(clientIdenty, joinMessage);

    while (serverRunning)
    {
        memset(buffer, 0, sizeof(buffer));
        received = recv(clientSocket, buffer, sizeof(buffer), 0);

        if (received <= 0)
            break;

        std::string message(buffer, received);

        {
            std::lock_guard<std::mutex> guard(clientsMutex);
            clientData[clientIdenty].messageText = message;
        }

        BroadcastMessage(clientIdenty, message);
    }

    // 클라이언트가 연결을 끊었을 때 처리
    {
        std::lock_guard<std::mutex> guard(clientsMutex);
        auto it = std::find(clients.begin(), clients.end(), clientSocket);

        if (it != clients.end())
            clients.erase(it);

        clientData.erase(clientIdenty);
    }

    // 다른 클라이언트들에게 클라이언트가 떠났음을 알림
    std::string leaveMessage = clientIdenty.name + "님이 채팅을 떠났습니다.\n";
    BroadcastMessage(clientIdenty, leaveMessage);

    close(clientSocket);
}


void MessageSystem::FindErrorClinet(ClientIdenty newClient)
{
    std::vector<ClientIdenty> clientToRemove;

    // 저장되어 있는 모든 클라이언트의 정보를 비교하여 ID 또는 이름이 바뀌는 경우를 찾음
    for (const auto& client : clientData)
    {
        if (client.first.id == newClient.id && client.first.name != newClient.name)
            clientToRemove.push_back(client.first);

        else if (client.first.name == newClient.name && client.first.id != newClient.id)
            clientToRemove.push_back(client.first);
    }

    // 잘못된 클라이언트를 제거함
    for (const auto& clientID : clientToRemove)
        clientData.erase(clientID);
}

int MessageSystem::StartServer(int port)
{
    if (serverRunning)
    {
        Log::Message("Server is already running.");
        return -1;
    }

    serverThread = std::thread(ServerLoop, port);
    serverThread.detach();

    return 0;
}

void MessageSystem::StopServer()
{
    serverRunning = false;

    if (serverSocket >= 0)
        close(serverSocket);

    Log::CloseLog();
}

void MessageSystem::ServerLoop(int port)
{
    serverSocket = socket(AF_INET, SOCK_STREAM, 0);
    if (serverSocket < 0)
    {
        Log::Message("Failed to create server socket.");
        return;
    }

    sockaddr_in serverAddr;
    memset(&serverAddr, 0, sizeof(serverAddr));
    serverAddr.sin_family = AF_INET;
    serverAddr.sin_addr.s_addr = INADDR_ANY;
    serverAddr.sin_port = htons(port);

    if (bind(serverSocket, (sockaddr*)&serverAddr, sizeof(serverAddr)) < 0)
    {
        std::string errorMessage = "Failed to bind server socket. Error: " + std::string(strerror(errno));

        Log::Message(errorMessage.c_str());
        close(serverSocket);
        return;
    }

    if (listen(serverSocket, 5) < 0)
    {
        Log::Message("Failed to listen on server socket.");
        close(serverSocket);
        return;
    }

    serverRunning = true;

    std::string logText = "Server started on port " + std::to_string(port);
    Log::Message(logText.c_str());

    while (serverRunning)
    {
        sockaddr_in clientAddr;
        socklen_t clientAddrLen = sizeof(clientAddr);
        int clientSocket = accept(serverSocket, (sockaddr*)&clientAddr, &clientAddrLen);

        if (clientSocket < 0)
        {
            Log::Message("Failed to accept client connection.");
            continue;
        }

        std::thread clientThread(HandleClient, clientSocket);
        clientThread.detach();
    }

    close(serverSocket);
}

void MessageSystem::Daemonize()
{
    Log::serverPID = getpid();  // 서버 PID를 저장

    pid_t pid = fork();
    if (pid < 0)
    {
        Log::Message("Failed to fork.");
        exit(1);
    }

    // 부모 프로세스 종료
    if (pid > 0)
        exit(0);

    if (setsid() < 0)
    {
        Log::Message("Failed to create new session.");
        exit(1);
    }

    pid = fork();
    if (pid < 0)
    {
        Log::Message("Failed to fork.");
        exit(1);
    }

    // 첫 번째 자식 프로세스 종료
    if (pid > 0)
        exit(0);

    umask(0);

    int fd = open("/dev/null", O_RDWR);

    if (fd != -1)
    {
        dup2(fd, STDIN_FILENO);
        dup2(fd, STDOUT_FILENO);
        dup2(fd, STDERR_FILENO);
        close(fd);
    }
}

void MessageSystem::SignalHandler(int signal)
{
    if (signal == SIGTERM || signal == SIGINT)
    {
        StopServer();
        exit(0);
    }
}
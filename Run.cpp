#include "CryptologyMessage.h"
#include "Log.h"

#include <iostream>
#include <thread>       
#include <chrono>       
#include <csignal>      
#include <cstdlib>

int main(int argc, char* argv[])
{
    if (argc < 2)
    {
        std::cerr << "Usage: " << argv[0] << " <port>" << std::endl;
        return 1;
    }

    int port = std::stoi(argv[1]);

    MessageSystem::Daemonize();
    Log::FileOpen();

    if (MessageSystem::StartServer(port) != 0)
    {
        Log::Message("Failed to start server.");
        return 1;
    }

    Log::Message("Server is running in the background...");
    std::this_thread::sleep_for(std::chrono::milliseconds(10));

    // 신호 처리기 설정
    signal(SIGTERM, MessageSystem::SignalHandler);
    signal(SIGINT, MessageSystem::SignalHandler);

    // 메인 스레드를 종료시키지 않도록 무한 루프 추가
    while (MessageSystem::serverRunning)
        std::this_thread::sleep_for(std::chrono::seconds(5));

    MessageSystem::StopServer();
    return 0;
}
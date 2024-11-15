#include "Log.h"

#include <iostream>
#include <string>

pid_t Log::serverPID = 0;
std::ofstream Log::logFile;

void Log::FileOpen()
{
    logFile.open("logfileCrytologyMessage.txt", std::ios::out | std::ios::app);

    if (!logFile.is_open())
    {
        std::cerr << "Failed to open log file." << std::endl;
        exit(1);  // 로그 파일이 열리지 않으면 종료
    }
}

void Log::PrintLog()
{
    std::ifstream infile("logfileCrytologyMessage.txt");
    if (!infile.is_open())
    {
        std::cerr << "Failed to open log file for reading." << std::endl;
        return;
    }

    std::string line;
    while (std::getline(infile, line))
        std::cout << line << std::endl;
}

void Log::Message(const char* message)
{
    if (logFile.is_open())
        logFile << "[" << serverPID << "] " << message << std::endl;

    else
        std::cout << "[" << serverPID << "] " << message << std::endl;
}

void Log::Message(const std::string message)
{
    if (logFile.is_open())
        logFile << "[" << serverPID << "] " << message << std::endl;

    else
        std::cout << "[" << serverPID << "] " << message << std::endl;
}

void Log::CloseLog()
{
    if (logFile.is_open())
        logFile.close();
}
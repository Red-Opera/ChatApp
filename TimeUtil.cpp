#include "TimeUtil.h"

std::string TimeUtil::GetCurrentDateTime()
{
    std::time_t now = std::time(nullptr);
    char buffer[100];
    std::strftime(buffer, sizeof(buffer), "%Y-%m-%d %H:%M:%S", std::localtime(&now));

    return std::string(buffer);
}

std::string TimeUtil::GetKoreaCurrentDateTime()
{
    // 현재 시간 가져오기
    std::time_t now = std::time(nullptr);

    // tm 구조체로 변환
    std::tm* localTime = std::localtime(&now);

    // 9시간 추가
    localTime->tm_hour += 9;

    // 초과된 시간을 자동으로 조정
    std::time_t adjustedTime = std::mktime(localTime);

    // 결과를 "YYYY-MM-DD HH:MM:SS" 형식으로 변환
    char buffer[100];
    std::strftime(buffer, sizeof(buffer), "%Y-%m-%d %H:%M:%S", std::localtime(&adjustedTime));
    return std::string(buffer);
}
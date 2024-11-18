#include "TimeScheduler.h"

void TimeScheduler::ExecuteOnNextDay(const std::function<void()>& task)
{
    // 이전 날짜 저장 (9시간 더한 시간 기준)
    std::time_t now = std::time(nullptr);
    std::tm* localTime = std::localtime(&now);
    AddNineHours(localTime); // 9시간 추가

    int previousDay = localTime->tm_mday;

    while (true)
    {
        // 1초 대기
        std::this_thread::sleep_for(std::chrono::seconds(1));

        // 현재 시간 확인 (9시간 더한 시간 기준)
        now = std::time(nullptr);
        localTime = std::localtime(&now);
        AddNineHours(localTime); // 9시간 추가

        // 날짜가 바뀌었는지 확인
        if (localTime->tm_mday != previousDay)
        {
            previousDay = localTime->tm_mday;

            // 날짜가 바뀌었으면 작업 실행
            task();
        }
    }
}

void TimeScheduler::AddNineHours(std::tm* time)
{
    time->tm_hour += 9;
    std::mktime(time); // 초과 시간을 자동으로 조정
}
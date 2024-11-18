#include "TimeScheduler.h"

void TimeScheduler::ExecuteOnNextDay(const std::function<void()>& task)
{
    // ���� ��¥ ���� (9�ð� ���� �ð� ����)
    std::time_t now = std::time(nullptr);
    std::tm* localTime = std::localtime(&now);
    AddNineHours(localTime); // 9�ð� �߰�

    int previousDay = localTime->tm_mday;

    while (true)
    {
        // 1�� ���
        std::this_thread::sleep_for(std::chrono::seconds(1));

        // ���� �ð� Ȯ�� (9�ð� ���� �ð� ����)
        now = std::time(nullptr);
        localTime = std::localtime(&now);
        AddNineHours(localTime); // 9�ð� �߰�

        // ��¥�� �ٲ������ Ȯ��
        if (localTime->tm_mday != previousDay)
        {
            previousDay = localTime->tm_mday;

            // ��¥�� �ٲ������ �۾� ����
            task();
        }
    }
}

void TimeScheduler::AddNineHours(std::tm* time)
{
    time->tm_hour += 9;
    std::mktime(time); // �ʰ� �ð��� �ڵ����� ����
}
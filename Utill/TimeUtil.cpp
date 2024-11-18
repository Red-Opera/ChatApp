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
    // ���� �ð� ��������
    std::time_t now = std::time(nullptr);

    // tm ����ü�� ��ȯ
    std::tm* localTime = std::localtime(&now);

    // 9�ð� �߰�
    localTime->tm_hour += 9;

    // �ʰ��� �ð��� �ڵ����� ����
    std::time_t adjustedTime = std::mktime(localTime);

    // ����� "YYYY-MM-DD HH:MM:SS" �������� ��ȯ
    char buffer[100];
    std::strftime(buffer, sizeof(buffer), "%Y-%m-%d %H:%M:%S", std::localtime(&adjustedTime));
    return std::string(buffer);
}
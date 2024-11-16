#pragma once

#include <iostream>
#include <thread>
#include <chrono>
#include <ctime>
#include <functional>

class TimeScheduler
{
public:
    static void ExecuteOnNextDay(const std::function<void()>& task);

private:
    static void AddNineHours(std::tm* time);    // tm ����ü�� 9�ð� �߰�
};
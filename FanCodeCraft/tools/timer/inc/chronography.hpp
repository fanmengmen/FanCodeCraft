/******************************************************
 * C++ timming library
 * @file    chronography.hpp
 * @brief   统计程序运行时间
 * @author  guofan
 * @date    2020/03/12
 ******************************************************/ 

#include<iostream>
#include<chrono>
using namespace std;



/**
 * @brief 适用于windows和linux的计时模板类
 * Timer<#> 其中#为一下三种类型
 * std::chrono::steady_clock
 * std::chrono::system_clock
 * std::chrono::high_resolution_clock
 * 
*/
template <typename T>
class Timer
{
    public:
        Timer();
        ~Timer();
        /**
         * @brief 打印记录的时间，单位ms
        */
        void printData();
        
        /**
         * @brief 开始记录时间
         */
        void start();
        /**
         * @brief 获得记录的时间
         * @return double类型时间
         * count_s() 秒， 
         * count_ms() 毫秒， 
         * count_us() 微秒，
         * count_ns() 纳秒
         */
        double count_s();
        double count_ms();
        double count_us();
        double count_ns();
    private:
        typename T::time_point t_start;
        double t_duration;
};






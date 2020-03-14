#include"chronography.hpp"

template<typename T>
Timer<T>::Timer()
{

}

template<typename T>
Timer<T>::~Timer()
{

}

template<typename T>
void Timer<T>::start()
{
    t_start = T::now();
}


template<typename T>
double Timer<T>::count_s()
{
    std::chrono::duration<double, std::ratio<1L, 1L>> tm = T::now() - t_start;
    return tm.count();
}

template<typename T>
double Timer<T>::count_ms()
{
    std::chrono::duration<double, std::ratio<1L, 1000L>> tm = T::now() - t_start;
    return tm.count();
}

template<typename T>
double Timer<T>::count_us()
{
    std::chrono::duration<double, std::ratio<1L, 1000000L>> tm = T::now() - t_start;
    return tm.count();
}

template<typename T>
double Timer<T>::count_ns()
{
    std::chrono::duration<double, std::ratio<1L, 1000000000L>> tm = T::now() - t_start;
    return tm.count();
}

template<typename T>
void Timer<T>::printData()
{
    std::cout << "time: " << count_ms() << "ms(毫秒)" << endl;
}



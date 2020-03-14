#pragma once 

#include<thread>
#include<vector>
/**
 * @brief 等待线程完成，汇入主线程
*/
class join_threads
{
    std::vector<std::thread>& threads;

    public:
        /**
         * explicit关键字只能用于修饰只有一个参数的类构造函数, 它的作用是表明该构造函数是显示的, 而非隐式的, 
         * 跟它相对应的另一个关键字是implicit, 意思是隐藏的,类构造函数默认情况下即声明为implicit(隐式).
        */
        explicit join_threads(std::vector<std::thread> &threads_):
        threads(threads_){}
        ~join_threads()
        {
            for(unsigned long i = 0; i < threads.size(); i++)
            {
                if(threads[i].joinable())
                    threads[i].join();
            }
        }
};
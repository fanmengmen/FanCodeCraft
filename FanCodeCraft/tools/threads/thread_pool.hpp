#include<atomic>
#include"thread_safe_queue.hpp"
#include"join_threads.hpp"
#include<functional>
#include<vector>
#include<thread>

/**
 * @brief 简单的线程池，无等待，并且没有返回值
 * 该类单纯的将任务加入线程池执行
 * 希望有返回值，使用Thread_pool_wait.
*/
class Thread_pool
{
    std::atomic_bool done;
    thread_safe_queue<std::function<void()>> work_queue;//线程安全队列
    std::vector<std::thread> threads;
    join_threads joiner;

    void worker_thread()
    {
        while(!done)
        {
            // 使用void()无返回值，单纯的将任务加入线程池执行
            std::function<void()> task;
            if(work_queue.try_pop())
            {
                task();
            }
            else
            {
                std::this_thread::yield();
            }
            
        }
    }

    public:
        Thread_pool():done(false), joiner(threads)
        {
            // 获取硬件线程数
            unsigned const thread_count = std::thread::hardware_concurrency();
            try
            {
                for(unsigned i = 0; i < thread_count; i++)
                {
                    threads.push_back(std::thread(&Thread_pool::worker_thread, this));
                }
            }
            catch(...)
            {
                done = true;
                throw;
            }
            
        }

        ~Thread_pool()
        {
            done = true;
        }

        template<typename FunctionType>
        void submit(FunctionType f)
        {
            work_queue.push(std::function<void()> (f));
        }
};
/**
 *@author c++ concurrencu in action 
 *  并行版的std::accumulate
 */

#include<vector>
#include<thread>
#include<algorithm>
#include<numeric>
#include<ctime>
#include<iostream>
#include<functional>
/**
 * 谨记，c++使用模板类时，最好将声明与实现写在同一个h或hpp文件
 * 如果类的声明与实现是分开的，有三种方法：
 * 包含模式
 * 1、在包含头文件时要包含模板类实现的文件
*/
// #include"chronography.cpp"
/**
 * 2、包含模板类的声明文件，用另体外的文件显式地实例化类型，此处在timer库中
 * 使用了include.cpp文件，在其中使用
 * template class Timer<std::chrono::steady_clock>; 
 * template class Timer<std::chrono::system_clock>; 
 * template class Timer<std::chrono::high_resolution_clock>; 
 * 显式地实例化三种类
 */
#include"chronography.hpp"

/**
 * 3、分离模式，在模板定义中使用export关键字，但目前c++不支持export关键字
*/

                            /**************************************
                             * 定义thread_pool时，使用线程池管理线程 
                            ***************************************/
#define thread_pool
#ifdef thread_pool
/**
 * 测试使用《c++ concurrency in Action》中提供的无等待的线程库
*/
#include"thread_pool_wait.hpp"
#endif // thread_pool

using namespace std;
template<typename Iterator, typename T>

struct accumulate_block
{
    //重载（）操作符
    void operator()(Iterator first, Iterator last, T& result)
    {
        result = accumulate(first, last, result);
    }
    // 重载（）为有返回值的函数
    T operator()(Iterator first, Iterator last)
    {
        T result = 0;
        result = accumulate(first, last, result);
        return result;
    }
};

template<typename Iterator, typename T>
T parallel_accumulate(Iterator first, Iterator last, T init)
{
    long length = distance(first, last);
    if(!length)
    {
        return init;
    }
    // 每个线程中的最小任务数
    int min_per_thread = 25;
    int max_threads = (length + min_per_thread - 1)/min_per_thread;
    int hardware_threads = thread::hardware_concurrency();
    // 确定要开的线程的个数
    int num_threads = min(hardware_threads ? hardware_threads : 2, max_threads);
    // cout << "多线程个数: " << num_threads << endl; 
    num_threads = 4;
    long block_size = length/num_threads; // 计算每个block的大小
    // cout << "块大小：" << block_size << endl;
    // 用一个vector记录每个线程计算的结果
    vector<T> results(num_threads);
    Iterator block_start = first;

    #ifdef thread_pool
        std::vector<std::future<T>> futures(num_threads-1);//用一个装有future的容器接受线程的返回值
        Thread_pool_wait pool;
    #else//不使用线程池
    // 主线程除外, 初始化num_threads -1 个子线程
        vector<thread> threads(num_threads - 1);
    #endif // DEBUG


    for(int i = 0; i < (num_threads - 1); i++)
    {
        Iterator block_end = block_start;
        // block_end迭代器指向block末尾
        advance(block_end, block_size);
        #ifdef thread_pool
            futures[i] = pool.submit([=](){return accumulate_block<Iterator, T>()(block_start, block_end);});
        #else
            // 创建线程
            threads[i] = thread(accumulate_block<Iterator, T>(), block_start, block_end, ref(results[i]));
        #endif // thread_pool
        // 
        block_start = block_end;
    }
  
    T result = init;//记录结果
    // 最后一段用主线程处理
    #ifdef thread_pool
        T last_result = accumulate_block<Iterator, T>()(block_start, last);
        cout << "last_result = " << last_result << endl;
        for(unsigned long i = 0; i < num_threads-1; i++)
        {
            result+=futures[i].get();
        }
        result+=last_result;
    #else
        accumulate_block<Iterator, T>()(block_start, last, results[num_threads - 1]);
        // join，等待子线程完成任务
        // 其中，mem_fn表示类的成员函数，用到functional头文件
        /**
         * mem_fun	把成员函数转为函数对象，使用对象指针进行绑定
         * mem_fun_ref	把成员函数转为函数对象，使用对象(引用)进行绑定
         * mem_fn	把成员函数转为函数对象，使用对象指针或对象(引用)进行绑定
         * bind	包括但不限于mem_fn的功能，更为通用的解决方案，详见目录里std::bind的文章
        */
        for_each(threads.begin(), threads.end(),mem_fn(&thread::join));
        result = accumulate(results.begin(), results.end(),init);
    #endif // !pool_thread

    return result;
}


int main()
{
    
    vector<long> numbers;
    long n = 100000000;
    for(long i = 0; i < n; i++)
        numbers.push_back(i + 1);
    // 定义一个计时类timer
    Timer<chrono::high_resolution_clock> tm;

    cout << "—————————————————————————数据构建完成！！————————————————————————————" << endl;
    cout << "多线程并发从：" << 1 << " 加到 " << n;
    tm.start();
    cout << "结果：" << parallel_accumulate(numbers.begin(), numbers.end(), (long)0);
    tm.printData();
    cout << "单线程从：" << 1 << " 加到 " << n;
    tm.start();
    cout << "结果：" << accumulate(numbers.begin(), numbers.end(), (long)0);
    tm.printData();
    long result = 0;
    cout << "单线程循环：" << 1 << " 加到 " << n;
    tm.start();
    for(long i = 0; i < numbers.size(); i++)
        result += numbers[i];
    cout << "结果：" << result;
    tm.printData();
    return 0;
}
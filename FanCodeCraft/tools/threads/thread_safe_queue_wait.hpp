#pragma once
#include<mutex>
#include<memory>
#include<condition_variable>
/**
 * @brief 可上锁和等待的线程安全队列
 * 
*/
template<typename T>
class thread_safe_queue_wait
{
    private:
        //有两个智能指针构成的链表
        struct node
        {
            std::shared_ptr<T> data;/* data */;
            std::unique_ptr<node> next;
        };
        // 头尾互斥量
        std::mutex head_mutex;
        std::mutex tail_mutex;

        std::unique_ptr<node> head;//链表表头
        // 尾指针,以及加锁获取
        node* tail;
        std::condition_variable data_cond;



    private:
        

        /**
         * @brief 获取尾指针
         * 用于后面比较头指针和尾指针是否相等，判断队列是否为空，
         * 如果队列为空，将加锁等待数据
        */
        node* get_tail()
        {
            std::lock_guard<std::mutex> tail_lock(tail_mutex);
            return tail;
        }

        /**
         * @brief 弹出数据
         * try_pop_head() @return node智能指针，指向head头节点
         * try_pop_head(T& value) 通过value引用传递head头节点的数据data
         * 在pop时头指针加锁
        */
       std::unique_ptr<node> pop_head()
        {
            std::unique_ptr<node> old_head = std::move(head);
            head = std::move(old_head->next);
            return old_head;
        }
        std::unique_ptr<node> try_pop_head()
        {
            std::lock_guard<std::mutex> head_lock(head_mutex);
            if(head.get() == get_tail())
            {
                return std::unique_ptr<node>();
            }
            return pop_head();
        }
        std::unique_ptr<node> try_pop_head(T& value)
        {
            std::lock_guard<std::mutex> head_lock(head_mutex);
            if(head.get() == get_tail())
            return std::unique_ptr<node>();
            value = std::move(*head->data);
            return pop_head();
        }
        /**
         * @brief 有等待的弹出数据
         * wait_for_data()，等待队列中有数据，为下面的重载函数提供函数复用，减少代码量
         * wait_pop_head() @return node智能指针，指向head头节点
         * wait_pop_head(T& value) 通过value引用传递head头节点的数据data
         * 在pop时头指针加锁
        */
        std::unique_ptr<std::mutex> wait_for_data()
        {
            std::unique_lock<std::mutex> head_lock(head_mutex);
            // 等待下面push()中data_cond.notify_one()停止阻塞，开始运行
            data_cond.wait(head_lock,[&]{return head.get() != get_tail();});
            return std::move(head_lock);
        }
        std::unique_ptr<node> wait_pop_head()
        {
            std::unique_lock<std::mutex> head_lock(wait_for_data());
            return pop_head();
        }
        std::unique_ptr<node> wait_pop_head(T& value)
        {
            std::unique_lock<std::mutex> head_lock(wait_for_data());
            value = std::move(*head->data);
            return pop_head();
        }


    public:
        /***
         * 构造线程安全队列
        */
        thread_safe_queue_wait(): head(new node), tail(head.get()){}
        // 不支持拷贝构造
        thread_safe_queue_wait(const thread_safe_queue_wait& other) = delete;
        thread_safe_queue_wait& operator=(const thread_safe_queue_wait& other) = delete;
        /**
         * @brief 开放接口，头节点出队列
         * @return 出队列节点智能指针地址
         * 重载后的bool try_pop(T& value)通过value的引用传递节点
        */
        std::shared_ptr<T> try_pop()
        {
            std::unique_ptr<node> old_head = pop_head();
            return old_head? old_head->data : std::shared_ptr<T>();
        }
        bool try_pop(T& value)try
        {
            std::unique_ptr<node> const old_head = try_pop_head(value);
            return old_head? 1:0;
        }

        /**
         * @brief 开放接口，头节点等待并出队列
         * @return 出队列节点智能指针地址
         * 重载后的void wait_and_pop(T& value)通过value的引用传递节点
        */
        std::shared_ptr<T> wait_and_pop()
        {
            std::unique_ptr<node> const old_head = wait_pop_head();
            return old_head->data;
        }
        void wait_and_pop(T& value)
        {
            std::unique_ptr<node> const old_head = wait_pop_head(value);
        }

        /**
         * @brief 开放接口，判断队列是否为空
         * @return 空1，非空0
        */
        bool empty()
        {
            std::lock_guard<std::mutex> head_lock(head_mutex);
            return (head.get() == get_tail());
        }

        /**
         * @brief 开放接口，向队尾添加node节点
        */
        void push(T new_value)
        {
            //通过智能指针创建新的数据指针
            std::shared_ptr<T> new_data(std::make_shared<T>(std::move(new_value)));
            //通过智能指针创建新的节点
            std::unique_ptr<node> p(new node);
            // 加尾锁
            std::lock_guard<std::mutex> tail_lock(tail_mutex);
            tail->data = new_data;//给空的尾节点赋值
            node* const new_tail = p.get();
            tail->next = std::move(p);//连接一个新的空尾节点
            tail = new_tail;
            // 通知等待完成
            data_cond.notify_one();
        }


};
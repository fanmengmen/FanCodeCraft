#pragma once
#include<mutex>
#include<memory>
/**
 * @brief 线程安全队列
*/
template<typename T>
class thread_safe_queue
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
        node* get_tail()
        {
            std::lock_guard<std::mutex> tail_lock(tail_mutex);
            return tail;
        }
        std::unique_ptr<node> pop_head()
        {
            std::lock_guard<std::mutex> head_lock(head_mutex);
            if(head.get() == get_tail())
                return nullptr;
            
            std::unique_ptr<node> old_head = std::move(head);
            head = std::move(old_head->next);
            return old_head;
        }

    public:
        thread_safe_queue(): head(new node), tail(head.get()){}
        // 不支持拷贝构造
        thread_safe_queue(const thread_safe_queue& other) = delete;
        thread_safe_queue& operator=(const thread_safe_queue& other) = delete;



        std::shared_ptr<T> try_pop()
        {
            std::unique_ptr<node> old_head = pop_head();
            return old_head? old_head->data : std::shared_ptr<T>();
        }
        void push(T new_value)
        {
            //通过智能指针创建新的数据指针
            std::shared_ptr<T> new_data(std::make_shared<T>(std::move(new_value)));
            //通过智能指针创建新的节点
            std::unique_ptr<node> p(new node);
            node* const new_tail = p.get();
            std::lock_guard<std::mutex> tail_lock(tail_mutex);
            tail->data = new_data;//给空的尾节点赋值
            tail->next = std::move(p);//连接一个新的空尾节点
            tail = new_tail;
        }
};
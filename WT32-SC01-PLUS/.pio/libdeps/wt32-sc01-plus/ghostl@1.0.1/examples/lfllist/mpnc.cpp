// circular_mp_test.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include <iostream>
#include <thread>
#include <chrono>
#include <vector>
#include <circular_queue.h>
#include <ghostl/lfllist.h>

struct qitem
{
    // producer id
    unsigned id;
    // monotonic increasing value
    unsigned val = 0;
};

#ifdef _DEBUG
const unsigned TOTALMESSAGESTARGET = std::thread::hardware_concurrency() / 2 * 1000000;
#else
const unsigned TOTALMESSAGESTARGET = std::thread::hardware_concurrency() / 2 * 10000000;
#endif // _DEBUG
const auto PRODUCER_THREAD_CNT = std::thread::hardware_concurrency() / 2 > 2 ? std::thread::hardware_concurrency() / 2 - 1 : 1;
const auto CONSUMER_THREAD_CNT = std::thread::hardware_concurrency() / 2 > 2 ? std::thread::hardware_concurrency() / 2 - 1 : 1;
const unsigned MESSAGES = TOTALMESSAGESTARGET / PRODUCER_THREAD_CNT;
circular_queue<std::thread> producer_threads(PRODUCER_THREAD_CNT);
circular_queue<std::thread> consumer_threads(CONSUMER_THREAD_CNT);
ghostl::lfllist<qitem> queue;
std::atomic<unsigned> total_rx{ 0 };

int main()
{
    using namespace std::chrono_literals;
    std::cerr << "Sending " << TOTALMESSAGESTARGET << " total message count" << std::endl;
    std::cerr << "Utilizing " << PRODUCER_THREAD_CNT << " producer threads" << std::endl;
    for (unsigned i = 0; i < PRODUCER_THREAD_CNT; ++i)
    {
        producer_threads.push(std::thread([i]() {
            for (unsigned c = 0; c < MESSAGES; ++c)
            {
                //// simulate some load
                //auto start = std::chrono::system_clock::now();
                //while (std::chrono::system_clock::now() - start < 1us) {}
                while (!queue.emplace_front({ i, c }))
                {
                    std::this_thread::sleep_for(1us);
                }
            }
            std::cerr << "producer thread #" << i << " done" << std::endl;
            }));
    }
    std::cerr << "Utilizing " << CONSUMER_THREAD_CNT << " consumer threads" << std::endl;
    for (unsigned i = 0; i < CONSUMER_THREAD_CNT; ++i)
    {
        consumer_threads.push(std::thread([i]() {
            std::vector<unsigned> checks(PRODUCER_THREAD_CNT);
            while (total_rx.load() < PRODUCER_THREAD_CNT * MESSAGES)
            {
                //auto now = std::chrono::system_clock::now();
                if (qitem item; queue.try_pop(item))
                {
                    if (checks[item.id] > item.val)
                    {
                        std::cerr << "item order failure" << std::endl;
                    }
                    checks[item.id] = item.val;
                    total_rx++;
                    //// simulate some load
                    auto start = std::chrono::system_clock::now();
                    while (std::chrono::system_clock::now() - start < 1us) {}
                }
            }
            std::cerr << "consumer thread #" << i << " done" << std::endl;
            }));
    }
    while (producer_threads.available())
    {
        auto thread = producer_threads.pop();
        thread.join();
    }
    while (consumer_threads.available())
    {
        auto thread = consumer_threads.pop();
        thread.join();
    }
    if (queue.back())
    {
        std::cerr << "queue was non-empty on finish" << std::endl;
        return 1;
    }
    if (total_rx.load() != PRODUCER_THREAD_CNT * MESSAGES)
    {
        std::cerr << "total rx count != tx count on finish" << std::endl;
        return 2;
    }
    return 0;
}

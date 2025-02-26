// circular_mp_test.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include <iostream>
#include <chrono>
#include <vector>
#include <semaphore>
#include <circular_queue.h>
#include <ghostl/lfllist.h>
#include <ghostl/async_queue.h>
#include <ghostl/cancellation_token.h>
#include <ghostl/run_task.h>

#ifdef ESP8266
#include <Schedule.h>
#include <SoftwareSerial.h>
#include <ghostl.h>
#else
#include <ghostl/FastScheduler.h>
#endif

#ifdef ARDUINO
#define PRINTF Serial.printf
#define PRINTLN Serial.println
#elif defined(__ZEPHYR__)
#include <zephyr/kernel.h>
#include <zephyr/arch/cpu.h>
#include <zephyr/sys/printk.h>
#define PRINTF printk
#define PRINTLN(s) printk("%s\n", (s))
#else
#define PRINTF printf
#define PRINTLN puts
#endif

#ifdef __ZEPHYR__
#include <zephyr/kernel.h>
namespace {
    long unsigned micros() { return static_cast<long unsigned>(k_uptime_get() * 1000UL); }
    long unsigned millis() { return static_cast<long unsigned>(k_uptime_get()); }
}
#elif !defined(ARDUINO) // __ZEPHYR__
#include <thread>
#include <chrono>
namespace {
    long unsigned micros()
    {
        static auto start = std::chrono::steady_clock::now();
        return static_cast<long unsigned>(std::chrono::duration_cast<std::chrono::microseconds>(
            std::chrono::steady_clock::now() - start).count());
    }
    long unsigned millis()
    {
        static auto start = std::chrono::steady_clock::now();
        return static_cast<long unsigned>(std::chrono::duration_cast<std::chrono::milliseconds>(
            std::chrono::steady_clock::now() - start).count());
    }
}
#endif // __ZEPHYR__

#if defined(ARDUINO)
//delay(schedDelay_us / 1000UL);
#elif defined(__ZEPHYR__)
//#ifdef CONFIG_BOARD_NATIVE_POSIX
//    // The native POSIX board emulation does not advance system timeout during code execution,
//    // which is incompatible with the FastScheduler comparing system time to target times.
//    k_sleep(K_USEC(schedDelay_us ? schedDelay_us : 1));
//#else // CONFIG_BOARD_NATIVE_POSIX
//    k_sleep(K_USEC(schedDelay_us));
//#endif // CONFIG_BOARD_NATIVE_POSIX
#else //__ZEPHYR__
std::binary_semaphore delay_scheduler(1);
#endif //__ZEPHYR__

struct schedule : std::suspend_always
{
    ghostl::cancellation_token ct;
    schedule(ghostl::cancellation_token _ct = {}) : ct(_ct) {}

    void await_suspend(std::coroutine_handle<> handle)
    {
        schedule_function([handle]() {
            if (handle && !handle.done()) handle.resume();
            });
        // break delay
#if defined(ARDUINO)
        //delay(schedDelay_us / 1000UL);
#elif defined(__ZEPHYR__)
        //#ifdef CONFIG_BOARD_NATIVE_POSIX
        //    // The native POSIX board emulation does not advance system timeout during code execution,
        //    // which is incompatible with the FastScheduler comparing system time to target times.
        //    k_sleep(K_USEC(schedDelay_us ? schedDelay_us : 1));
        //#else // CONFIG_BOARD_NATIVE_POSIX
        //    k_sleep(K_USEC(schedDelay_us));
        //#endif // CONFIG_BOARD_NATIVE_POSIX
#else //__ZEPHYR__
        delay_scheduler.release();
#endif //__ZEPHYR__
    }
};

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
const auto PRODUCER_THREAD_CNT = std::thread::hardware_concurrency() / 2;
const unsigned MESSAGES = TOTALMESSAGESTARGET / PRODUCER_THREAD_CNT;
circular_queue<std::thread> producer_threads(PRODUCER_THREAD_CNT);
ghostl::async_queue<qitem> queue;
std::vector<unsigned> checks(PRODUCER_THREAD_CNT);
unsigned total_rx{ 0 };

auto run_consumer(ghostl::cancellation_token ct = {}) -> ghostl::task<>
{
    while (total_rx < PRODUCER_THREAD_CNT * MESSAGES)
    {
        //auto now = std::chrono::system_clock::now();
        auto item = co_await queue.pop();
        co_await schedule();
        if (checks[item.id] != item.val)
        {
            PRINTF("thread #%u item mismatch (expected %u): %u\n", item.id, checks[item.id], item.val);
        }
        checks[item.id]++;
        total_rx++;
        //// simulate some load
        //auto start = std::chrono::system_clock::now();
        //while (std::chrono::system_clock::now() - start < 1us) {}
    }
    queue.flush();
    PRINTLN("consumer done");
}

void main_async_queue(ghostl::cancellation_token ct)
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
                //while (std::chrono::system_clock::now() - start < 30us) {}
                //if (0 == c % 10000) while (std::chrono::system_clock::now() - start < 400ms) {}

                if (!queue.push({ i, c }))
                {
                    std::cerr << "queue full" << std::endl;
                    std::this_thread::sleep_for(10us);
                }
            }
            PRINTF("producer thread #%u done\n", i);
            }));
    }

    auto task = run_consumer(ct);
    auto runner = ghostl::run_task(std::move(task));
    runner.continue_with([ct]() {
        while (producer_threads.available())
        {
            auto thread = producer_threads.pop();
            thread.join();
        }

        if (total_rx != PRODUCER_THREAD_CNT * MESSAGES)
        {
            PRINTLN("total rx count != tx count on finish");
            exit(2);
        }
        for (unsigned i = 0; i < PRODUCER_THREAD_CNT; ++i)
        {
            if (checks[i] != MESSAGES)
            {
                PRINTF("thread #%u rx count != thread tx count on finish\n", i);
                exit(2);
            }
        }
        exit(0);
        });
    runner.resume();
}

ghostl::cancellation_token_source cts;

void setup()
{
#if defined(ESP32) || defined(ESP8266)
    Serial.begin(115200);
    while (!Serial) delay(1);
#endif

    auto ct = cts.token();
    main_async_queue(ct);
}

void loop()
{
#ifndef ESP8266
    auto schedDelay_us = get_scheduled_recurrent_delay_us();
#else
    auto schedDelay_us = std::min(get_scheduled_delay_us(), get_scheduled_recurrent_delay_us());
#endif
    //if (schedDelay_us >= 10)
    //{
    //    PRINTF("Delaying scheduler: %lu us (now = %lu s)\n", schedDelay_us, millis() / 1000);
    //}

#if defined(ARDUINO)
    //delay(schedDelay_us / 1000UL);
#elif defined(__ZEPHYR__)
    //#ifdef CONFIG_BOARD_NATIVE_POSIX
    //    // The native POSIX board emulation does not advance system timeout during code execution,
    //    // which is incompatible with the FastScheduler comparing system time to target times.
    //    k_sleep(K_USEC(schedDelay_us ? schedDelay_us : 1));
    //#else // CONFIG_BOARD_NATIVE_POSIX
    //    k_sleep(K_USEC(schedDelay_us));
    //#endif // CONFIG_BOARD_NATIVE_POSIX
#else //__ZEPHYR__
    if (delay_scheduler.try_acquire_for(std::chrono::microseconds(schedDelay_us))) {}
#endif //__ZEPHYR__

#ifndef ESP8266
    run_scheduled_functions();
#endif
}

int main(int argc, char* argv[])
{
    (void)argc;
    (void)argv;
    setup();
    for (;;) loop();
    return 0;
}

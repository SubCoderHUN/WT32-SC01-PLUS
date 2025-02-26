/*
 FastScheduler.cpp - Scheduled functions.
 Copyright (c) 2020 esp8266/Arduino
 Copyright (c) 2023 Dirk O. Kaar
 
 This library is free software; you can redistribute it and/or
 modify it under the terms of the GNU Lesser General Public
 License as published by the Free Software Foundation; either
 version 2.1 of the License, or (at your option) any later version.
 This library is distributed in the hope that it will be useful,
 but WITHOUT ANY WARRANTY; without even the implied warranty of
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 Lesser General Public License for more details.
 You should have received a copy of the GNU Lesser General Public
 License along with this library; if not, write to the Free Software
 Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
*/

#ifndef ESP8266 // This is not suitable for ESP8266 MCUs

#include "FastScheduler.h"
#include <atomic>
#include <cassert>
#include <circular_queue_mp.h>

#ifdef __ZEPHYR__
#include <zephyr/kernel.h>
namespace {
    long unsigned micros() { return static_cast<long unsigned>(k_uptime_get() * 1000UL); }
}
#elif !defined(ARDUINO) // __ZEPHYR__
#include <chrono>
namespace {
    long unsigned micros()
    {
        static auto start = std::chrono::steady_clock::now();
        return static_cast<long unsigned>(std::chrono::duration_cast<std::chrono::microseconds>(
            std::chrono::steady_clock::now() - start).count());
    }
}
#endif // __ZEPHYR__

// As 32 bit unsigned integer, micros() rolls over every 71.6 minutes.
// For unambiguous earlier/later order between two timestamps,
// despite roll over, there is a limit on the maximum duration
// that can be requested, if full expiration must be observable:
// later - earlier >= 0 for both later >= earlier or (rolled over) later <= earlier
// Also, expiration should remain observable for a useful duration of time:
// now - (start + period) >= 0 for now - start >= 0 despite (start + period) >= now
// A well-balanced solution, not breaking on two's compliment signed arithmetic,
// is limiting durations to the maximum signed value of the same word size
// as the original unsigned word.
constexpr decltype(micros()) HALF_MAX_MICROS = ~static_cast<decltype(micros())>(0) >> 1;

typedef std::function<bool(void)> mFuncT;

struct scheduled_fn_t
{
    mFuncT mFunc = nullptr;
    decltype(micros()) callPeriod_us;
    decltype(micros()) callTime_us;
    std::function<bool()> alarm = nullptr;
    scheduled_fn_t() : callPeriod_us(0), callTime_us(micros()) { }
    decltype(micros()) remaining_us() {
        const auto elapsed = micros() - callTime_us;
        return elapsed > callPeriod_us ? 0 : callPeriod_us - elapsed;
    }
    void restart() {
        callTime_us = micros();
    }
};

// anonymous namespace provides compilation-unit internal linkage
namespace {
    static circular_queue_mp<scheduled_fn_t> schedule_queue(FASTSCHEDULER_FN_MAX_COUNT);
    static decltype(micros()) yieldIntvl_us;
    static std::atomic<decltype(micros())> deadline_us;
};

bool IRAM_ATTR schedule_recurrent_function_us(std::function<bool(void)>&& fn, decltype(micros()) repeat_us,
    std::function<bool(void)>&& alarm)
{
    assert(repeat_us <= HALF_MAX_MICROS);

    scheduled_fn_t item;
    item.mFunc = std::move(fn);
    if (repeat_us) item.callPeriod_us = repeat_us;
    item.alarm = std::move(alarm);
    const auto pushed = schedule_queue.push(std::move(item));
    if (pushed)
    {
        auto dl = deadline_us.load();
        for (;;)
        {
            const auto dlRem = dl - micros();
            if (dlRem > HALF_MAX_MICROS || dlRem <= repeat_us ||
                deadline_us.compare_exchange_weak(dl, micros() + repeat_us))
            {
                break;
            }
        }
    }
    return pushed;
}

bool IRAM_ATTR schedule_recurrent_function_us(const std::function<bool(void)>& fn, decltype(micros()) repeat_us,
    const std::function<bool()>& alarm)
{
    return schedule_recurrent_function_us(std::function<bool(void)>(fn), repeat_us, std::function<bool()>(alarm));
}

bool IRAM_ATTR schedule_function(std::function<void(void)>&& fn)
{
    return schedule_recurrent_function_us([fn]() { fn(); return false; }, 0, nullptr);
}

bool IRAM_ATTR schedule_function(const std::function<void(void)>& fn)
{
    return schedule_function(std::function<void(void)>(fn));
}

decltype(micros()) get_scheduled_recurrent_delay_us()
{
    if (!schedule_queue.available()) return HALF_MAX_MICROS;
    // handle already expired deadline_us.
    const auto rem = deadline_us.load() - micros();
    return (rem <= HALF_MAX_MICROS) ? rem : 0;
}

bool run_function(scheduled_fn_t& func)
{
    if (micros() - yieldIntvl_us > 10000)
    {
#ifdef ESP32
        yield();
#endif
        yieldIntvl_us = micros();
    }
    const bool wakeup = func.alarm && func.alarm();
    const bool callNow = !func.remaining_us();

    const auto keep = !(wakeup || callNow) || func.mFunc();
    if (keep)
    {
        if (callNow) func.restart();
        auto dl = deadline_us.load();
        for (;;)
        {
            const auto funcRem = func.remaining_us();
            const auto dlRem = dl - micros();
            if (dlRem > HALF_MAX_MICROS || dlRem <= funcRem ||
                deadline_us.compare_exchange_weak(dl, micros() + funcRem))
            {
                break;
            }
        }
    }
    return keep;
}

void run_scheduled_functions()
{
    // Note to the reader:
    // There is no exposed API to remove a scheduled function:
    // Scheduled functions are removed only from this function, and
    // its purpose is that it is never called from an interrupt.

    static std::atomic<bool> fence(false);
    if (fence.exchange(true)) return;

    yieldIntvl_us = micros();

    deadline_us.store(micros() + HALF_MAX_MICROS);
    schedule_queue.for_each_rev_requeue(run_function);
    fence.store(false);
}

#endif // ESP8266


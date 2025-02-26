/*
 FastScheduler.h - Header file for scheduled functions.
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

#ifndef _FASTSCHEDULER_H
#define _FASTSCHEDULER_H

#include <functional>
#include <cstdint>

#ifndef FASTSCHEDULER_FN_MAX_COUNT
#define FASTSCHEDULER_FN_MAX_COUNT 256
#endif // FASTSCHEDULER_FN_MAX_COUNT

#if defined(ARDUINO)
#include <Arduino.h>
#else
namespace {
    long unsigned micros();
}
#endif

// Scheduled functions called once:
//
// * internal queue is FIFO.
// * Add the given function to a fifo list of functions.
// * There is no mechanism for cancelling scheduled functions.
// * Long running operations are not allowed in the recurrent function.
// * Returns false if the number of scheduled functions exceeds
//   FASTSCHEDULER_FN_MAX_COUNT (or memory shortage).
// * Run the function only once next turn.
// * A scheduled function can itself schedule a function.

bool schedule_function (const std::function<void(void)>& fn);

// Recurrent scheduled function:
//
// * Internal queue is a FIFO.
// * Run the function periodically about every <repeat_us> microseconds until
//   it returns false.
// * Note that it may be more than <repeat_us> microseconds between calls if
//   `yield` is not called frequently, and therefore should not be used for
//   timing critical operations.
// * There is no mechanism for externally cancelling recurrent scheduled
//   functions.  However a user function returning false will cancel itself.
// * Long running operations are not allowed in the recurrent function.
// * Returns false if the number of scheduled functions exceeds
//   FASTSCHEDULER_FN_MAX_COUNT (or memory shortage).
// * If alarm is used, anytime during scheduling when it returns true,
//   any remaining delay from repeat_us is disregarded, and fn is executed.

bool schedule_recurrent_function_us(const std::function<bool(void)>& fn,
    decltype(micros()) repeat_us, const std::function<bool(void)>& alarm = nullptr);

// get_scheduled_recurrent_delay_us() returns the maximum delay
// until the nearest scheduled recurrent function is due.
// This does not include scheduled recurrent functions that were
// scheduled since the most recent run_scheduled_functions() started,
// except if done from a function itself running off the scheduler.

decltype(micros()) get_scheduled_recurrent_delay_us();

// Run all scheduled functions.

void run_scheduled_functions();

#endif //_FASTSCHEDULER_H

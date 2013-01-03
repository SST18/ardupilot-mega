/// -*- tab-width: 4; Mode: C++; c-basic-offset: 4; indent-tabs-mode: nil -*-

#include <AP_HAL.h>
#include <AP_HAL_AVR.h>
#include "Semaphores.h"
#include "Scheduler.h"
using namespace AP_HAL_AVR;

extern const AP_HAL::HAL& hal;

// Constructor
AVRSemaphore::AVRSemaphore() : _taken(false) {}

bool AVRSemaphore::give() {
    if (!_taken) {
        return false;
    } else {
        _taken = false;
        return true;
    }
}

bool AVRSemaphore::take(uint32_t timeout_ms) {
    if (AVRScheduler::_in_timer_proc) {
        hal.scheduler->panic(PSTR("PANIC: AVRSemaphore::take used from "
                    "inside timer process"));
        return false; /* Never reached - panic does not return */
    }
    return _take_from_mainloop(timeout_ms);
}

bool AVRSemaphore::take_nonblocking() {
    if (AVRScheduler::_in_timer_proc) {
        return _take_nonblocking();
    } else {
        return _take_from_mainloop(0);
    }
}

bool AVRSemaphore::_take_from_mainloop(uint32_t timeout_ms) {
    /* Try to take immediately */
    if (_take_nonblocking()) {
        return true;
    } else if (timeout_ms == 0) {
        /* Return immediately if timeout is 0 */
        return false;
    }

    do {
        /* Delay 1ms until we can successfully take, or we timed out */
        hal.scheduler->delay(1);
        timeout_ms--;
        if (_take_nonblocking()) {
            return true;
        }
    } while (timeout_ms > 0);

    return false;
}

bool AVRSemaphore::_take_nonblocking() {
    bool result = false;
    hal.scheduler->begin_atomic();
    if (!_taken) {
        _taken = true;
        result = true;
    }
    hal.scheduler->end_atomic();
    return result;
}


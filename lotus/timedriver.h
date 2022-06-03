#pragma once

#include "types.h"
#include "timer.h"

class timedriver_t {
public:
    virtual ~timedriver_t() = default;

    virtual lotus::timer_t *run_at(uint64_t time_us, timer_callback_t cb) =0;
    virtual lotus::timer_t *run_after(uint64_t us, timer_callback_t cb) =0;
    virtual lotus::timer_t *run_every(uint64_t us, timer_callback_t cb) =0;
};

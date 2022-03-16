#ifndef SIMPLE_PROFILER_H
#define SIMPLE_PROFILER_H

#include "types.h"
#include "platform/platform.h"
#include <stdio.h>

struct tc_Profiler
{
    u64 start_cycle_count[64];
    u64 cycles[64];
    u64 result[64];
};

inline void tc_profiler_init(tc_Profiler *prof, u32 index)
{
    prof->cycles[index] = 0;
}

inline void tc_profiler_begin(tc_Profiler *prof, u32 index)
{
    prof->start_cycle_count[index] = tc_platform_get_cycle_count(); 
}

inline void tc_profiler_end(tc_Profiler *prof, u32 index)
{
    u64 current_cycles = tc_platform_get_cycle_count();
    prof->cycles[index] += current_cycles - prof->start_cycle_count[index];
    prof->start_cycle_count[index] = current_cycles;
}

inline void tc_profiler_clear(tc_Profiler *prof, u32 index)
{
    prof->result[index] = prof->cycles[index];
    prof->cycles[index] = 0;
    prof->start_cycle_count[index] = tc_platform_get_cycle_count(); 
}

#endif // SIMPLE_PROFILER_H

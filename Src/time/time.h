#ifndef TIME_H
#define TIME_H
#include <stdint.h>

typedef uint64_t timeUs_t;
typedef uint32_t timeMs_t;
typedef int32_t timeDelta_t;
void TicksInit(void);
void DelayUs(uint32_t us);
void DelayMs(uint32_t ms);
timeUs_t micros(void);
#endif
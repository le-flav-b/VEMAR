#include "timer.h"

#define TIMER1_WGM_LOW_MASK 0x03
#define TIMER1_WGM_HIGH_MASK 0x18

void TIMER0_init(timer0_mode_t mode, timer0_ps_t prescaler)
{
    if ((0 == TCCR0A) && (0 == TCCR0B))
    {
        TCCR0A = (byte_t)(mode);
        TCCR0B = (byte_t)(prescaler);
    }
}

void TIMER1_init(timer1_mode_t mode, timer1_ps_t prescaler)
{
    if ((0 == TCCR1A) && (0 == TCCR1B))
    {
        TCCR1A = (byte_t)(BIT_read(mode, TIMER1_WGM_LOW_MASK));
        TCCR1B = (byte_t)(BIT_read(mode, TIMER1_WGM_HIGH_MASK) | prescaler);
    }
}

void TIMER2_init(timer2_mode_t mode, timer2_ps_t prescaler)
{
    if ((0 == TCCR2A) && (0 == TCCR2B))
    {
        TCCR2A = (byte_t)(mode);
        TCCR2B = (byte_t)(prescaler);
    }
}

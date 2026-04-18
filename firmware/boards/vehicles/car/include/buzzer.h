#ifndef BUZZER_H
# define BUZZER_H

# include "utils.h"
# include <avr/interrupt.h>

# define BUZZER _PD0

void buzzer_init(void);
void buzzer_off(void);
void buzzer_tone(uint16_t frequency);

#endif

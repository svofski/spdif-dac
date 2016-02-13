#ifndef _TIMER2_H
#define _TIMER2_H

extern volatile void (*timer2_overflow_hook)(void);
extern volatile void (*timer1_overflow_hook)(void);

void timer2_init();
void timer1_init();

void setRedLedDuty(uint8_t duty);


#endif

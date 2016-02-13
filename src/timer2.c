#include <avr/io.h>
#include <avr/interrupt.h>

#include "spdifm.h"

volatile void (*timer2_overflow_hook)(void);
volatile void (*timer1_overflow_hook)(void);

static void default_hook(void);

static volatile uint8_t RED_LED_DUTY;

void timer2_init() {
	timer2_overflow_hook = default_hook;
	TCCR2 = 0x6;		// select clock source = CLKt2s/256
	ASSR = 0x0; 		// ensure synchronous operation
	TIMSK |= _BV(TOIE2);
	RED_LED_DUTY = 0;
}

void timer1_init() {
	timer1_overflow_hook = default_hook;
	//TCCR1A = _BV(COM1A0) | _BV(WGM10);
	//TCCR1B = 0x2;		// clk = clkIO/8
	TCCR1A = 0b10000001;
	TCCR1B = 0b00001000;
	OCR1A = 0;
}

void setRedLedDuty(uint8_t duty) {
	RED_LED_DUTY = duty;
	if (RED_LED_DUTY > 128) {
		OCR1A = RED_LED_DUTY;
		TCCR1B |= 0b001;			// enable Timer 1
		TCCR1A |= 0x80;
	}
}

static void default_hook(void) {
/*
	static uint8_t moo = 0;
	if (moo % 2 == 0) {
		PORTD |= (1<<5);
	} else {
		PORTD &= ~(1<<5);
	}
	moo++;
*/	
}

void SIG_OVERFLOW2( void ) __attribute__ ( ( signal ) );


void SIG_OVERFLOW2( void ) {
	timer2_overflow_hook();
	
	if (RED_LED_DUTY >= 3) {
		OCR1A = RED_LED_DUTY;
		RED_LED_DUTY -= 3;
	}
	if (RED_LED_DUTY < 128) {
		TCCR1B &= 0xf8;			// disable Timer 1
		TCCR1A &= ~0x80;
		PORTB &= ~_BV(1);
	}
}


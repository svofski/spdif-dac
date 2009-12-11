#ifndef _SPDIFM_H
#define _SPDIFM_H

#include <avr/io.h>

#define SPIDDR	DDRB
#define SPIPORT PORTB

#define PORT_SNDRST		PORTB		// port where sound reset line is
#define SNDRST			2			// PORTB2

#define PORT_SS_RECV	PORTB		// SS_RECV is PORTB0
#define SS_RECV	0

#define PORT_SS_DAC		PORTD
#define SS_DAC  		4			// SS_DAC is PORTD4



#endif

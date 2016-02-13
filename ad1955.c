#include "spdifm.h"
#include "ad1955.h"

#include <stdio.h>
#include <avr/delay.h>

static volatile uint8_t dac_muted;

void ad1955_write(uint16_t word) {
	PORT_SS_DAC &= ~(1 << SS_DAC);		// enable AD1955

	// send 16 bits of data, MSB first
	SPDR = (uint8_t) ((word >> 8) & 0xff);		
	spi_wait();
	SPDR = (uint8_t) (word & 0xff);
	spi_wait();
	
	PORT_SS_DAC |= (1 << SS_DAC);
}

void ad1955_configure() {
	ad1955_write(0);
	dac_muted = 0;
}

void ad1955_mute() {
	if (dac_muted == 0) {
		ad1955_write(_BV(AD1955_R0_MUTE));
		_delay_ms(1);
		ad1955_write(_BV(AD1955_R0_MUTE) | _BV(AD1955_R0_POWERDOWN));
		dac_muted = 1;
	}
}

void ad1955_unmute() {
	if (dac_muted == 1) {
		ad1955_write(_BV(AD1955_R0_MUTE));	// power up, but mute
		_delay_ms(1);
		ad1955_write(0);					// unmute after powered up
		dac_muted = 0;
	}
}

uint8_t ad1955_isMuted() {
	return dac_muted;
}


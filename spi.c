#include "spdifm.h"
#include "spi.h"

void spi_init() {
	SPIDDR |= (1 << 3) | (1 << 5);					// MOSI & SCK output
	SPIDDR &= ~(1 << 4);								// MISO input
	SPCR = (uint8_t) (1 << SPE) | (1 << MSTR) | (0 << SPR0);	// Enable SPI, Master, clock rate = fuck/4

	// disable DAC & Receiver
	PORT_SS_RECV |= (1 << SS_RECV);
	PORT_SS_DAC |= (1 << SS_DAC);
}

inline void spi_wait() {
	while (!(SPSR & (1 << SPIF))) {};
}

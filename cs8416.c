#include <stdio.h>

#include "spdifm.h"
#include "cs8416.h"
#include "spi.h"

uint8_t	SPDIF_STATUS_FLAG;

uint8_t spdif_queryStatusIRQ() {
	return (SPDIF_STATUS_FLAG & (1<<SPDIF_INTERRUPT)) != 0;
}

void spdif_clearIRQ() {
	spdif_read(SPDIF_INT1STAT);
	SPDIF_STATUS_FLAG = 0;
}

void spdif_write(uint8_t reg, uint8_t data) {
	PORTB &= ~(1 << SS_RECV);

	SPDR = 0b00100000;					// weird CS8416 chip address, lsb == 0 is for writing
	spi_wait();

	// send MAP (register address)
	SPDR = reg;		
	spi_wait();

	// send data
	SPDR = data;
	spi_wait();

	// raise SS_RECV
	PORTB |= (1 << SS_RECV);
}

uint8_t spdif_read(uint8_t reg) {
	PORTB &= ~(1 << SS_RECV);

	SPDR = 0b00100000; 					// write MAP
	spi_wait();

	SPDR = reg;							// MAP = reg
	spi_wait();
	PORTB |= (1 << SS_RECV);  			// finish partial cycle

	PORTB &= ~(1 << SS_RECV);			// open read cycle
	SPDR = 0b00100001;					// read operation
	spi_wait();
	SPDR = 0;
	spi_wait();

	// raise SS_RECV
	PORTB |= (1 << SS_RECV);
	
	return SPDR;
}

void spdif_dumpRegisters() {
	int i;

	PORTB &= ~(1 << SS_RECV);

	SPDR = 0b00100000; 					// write MAP
	spi_wait();

	SPDR = 0x00;						// MAP = 0
	spi_wait();
	PORTB |= (1 << SS_RECV);  			// finish partial cycle

	PORTB &= ~(1 << SS_RECV);			// open read cycle
	SPDR = 0b00100001;					// read operation
	spi_wait();

	printf("CS8416 Register Dump:");
	for (i = 0; i < 0x80; i++) {
		if (i % 16 == 0) {
			printf("\n%02x: ", i);
		}
		if (i % 8 == 0) {
			printf(" - ");
		}
		SPDR = 0;
		spi_wait();
		printf("%02x ", SPDR);
	}
	printf("\n");
	
	// raise SS_RECV
	PORTB |= (1 << SS_RECV);
}

void spdif_configure() {
	spdif_write(SPDIF_CONTROL0, _BV(SPDIF_PDUR));				// PDUR = 1
	
	//spdif_write(SPDIF_CONTROL1, _BV(SPDIF_SWCLK));

	spdif_write(SPDIF_CONTROL2, (0b0010 << SPDIF_GPO0SEL0) |		// Set GPO0 to be Interrupt Output
								(0b100 << SPDIF_EMPH_CNTL0));		// Automatic de-emphasis filter select
											

																// Transmitter is RXP7
	spdif_write(SPDIF_SADF,
		(1<<SPDIF_SOMS) | (1<<SPDIF_SOSF) | (1<<SPDIF_SODEL) | (1<<SPDIF_SOLRPOL));	// Master mode, OSCLK & OLRCLK are outputs
																						// OSCLK frequency = 128*Fs
																						// I2S mode

	spdif_write(SPDIF_REM, 0x7F);								// Enable all errors
	
	spdif_write(SPDIF_IM, _BV(SPDIF_RERRM) | _BV(SPDIF_FCHM));	// Enable error and format change interrupts
}

static uint8_t CONTROL4_VALUE;

void spdif_selectChannel(uint8_t channel) {
	CONTROL4_VALUE = (1<<SPDIF_RUN) | ((channel & 0x7) << SPDIF_RXSEL0) | (7 << SPDIF_TXSEL0);
	spdif_write(SPDIF_CONTROL4, CONTROL4_VALUE);	// Enable, RXP1 (pin 3) is the input for recv, 
}

uint8_t spdif_readStatus() {
	uint8_t result = 0;
	
	uint8_t dummy8 = spdif_read(SPDIF_FDSTAT);
	if (dummy8 & _BV(SPDIF_SIL)) result |= _BV(SPDIF_STATUS_SILENCE);
	if (dummy8 & _BV(SPDIF_PCM)) result |= _BV(SPDIF_STATUS_PCM);
	
	dummy8 = spdif_read(SPDIF_RERROR);
	if (dummy8 & _BV(SPDIF_CONF)) result |= _BV(SPDIF_CONFIDENCE_ERROR);
	
	return result;
}

void spdif_stop() {
	//CONTROL4_VALUE &= ~(_BV(SPDIF_RUN));				// disable RMCK output
	//spdif_write(SPDIF_CONTROL4, CONTROL4_VALUE);
	//spdif_write(SPDIF_CONTROL1, _BV(SPDIF_SWCLK));
}
void spdif_run() {
	//CONTROL4_VALUE |= _BV(SPDIF_RUN);				// enable RMCK output
	//spdif_write(SPDIF_CONTROL4, CONTROL4_VALUE);
	//  spdif_write(SPDIF_CONTROL1, 0);
}

// CS8416 Interrupt handler
void SIG_INTERRUPT1( void ) __attribute__ ( ( signal ) );  
void SIG_INTERRUPT1( void ) {
	SPDIF_STATUS_FLAG |= _BV(SPDIF_INTERRUPT);
}



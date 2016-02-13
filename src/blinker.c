#include <inttypes.h>
#include <avr/io.h>
#include <avr/delay.h>
#include <avr/interrupt.h>
#include <avr/sleep.h>
#include <stdio.h>
#include <stdlib.h>

#include "spdifm.h"
#include "spi.h"
#include "cs8416.h"
#include "ad1955.h"
#include "usrat.h"
#include "timer2.h"

static const char* idstring = VERSION" build "BUILDNUM;

static volatile uint8_t lastReceiverStatus;


#define REQUEST_MUTE	0
#define REQUEST_UNMUTE	1
#define REQUEST_STOP	2
#define REQUEST_RUN		3

static volatile uint8_t REQUEST_STATUS;			// REQUEST_MUTE, REQUEST_UNMUTE

static void ioinit() {
	DDRD = (uint8_t) 0xff & ~(_BV(3));	// PORTD 3 is input (INT1, RECVINT)
	DDRC = 0xff;	// LCD lines
	DDRB = 0x0f;	// SS_RECV, LCD_BACKLIGHT, SS/SNDRST, MOSI (see spi.c)
	PORTD = _BV(4);
	PORTC = 0x0;
	PORTB = 0x0;
	
	
	set_sleep_mode(SLEEP_MODE_IDLE);

	// set INT0 and INT1 on rising edge
	MCUCR |= _BV(ISC11) | _BV(ISC10) | _BV(ISC01) | _BV(ISC00);
	GICR |= _BV(INT1);		// enable INT1
	
	// Enable GIE
	sei();
}

static void sound_reset() {
	PORT_SNDRST &= ~(1<<SNDRST);
	_delay_ms(100);
	PORT_SS_RECV |= (1 << SS_RECV);	// 
	_delay_ms(100);
	PORT_SNDRST |= (1<<SNDRST);
	_delay_ms(100);
	PORT_SS_RECV &= ~(1 << SS_RECV);	// ensures hi-to-low transition after RST is brought high
	_delay_ms(100);
	PORT_SS_RECV |= (1 << SS_RECV);	// 
}

static void selectInputChannel(uint8_t chan) {
	ad1955_mute();
	spdif_selectChannel(chan);
}

static void decodeAndPrintStatus() {
	uint8_t dummy8;
	
	// get all interesting status info
	printf("Format %s PCM\n", (spdif_read(SPDIF_FDSTAT) & _BV(SPDIF_PCM)) ? "is" : "is not");
	
	dummy8 = spdif_read(SPDIF_RERROR);
	if (dummy8 & _BV(SPDIF_PAR)) printf("Parity ");
	if (dummy8 & _BV(SPDIF_BIP)) printf("Bi-phase ");
	if (dummy8 & _BV(SPDIF_V))   printf("Validity ");
	if (dummy8 & _BV(SPDIF_UNLOCK)) printf("PLL ");
	if (dummy8 & _BV(SPDIF_CCRC)) printf("Channel Status CRC ");
	printf("\n");
	
	dummy8 = spdif_read(SPDIF_RCSTAT);
	printf("Aux data %d bits long\n", (uint8_t)(dummy8 >> 4));
	printf("Data is in %s format\n", (dummy8 & _BV(SPDIF_PRO)) ? "pro" : "consumer");
	printf("Copyright %s asserted, %s\n", (dummy8 & _BV(SPDIF_COPY)) ? "not" : "is", 
												    (dummy8 & _BV(SPDIF_ORIG)) ? "original" : "generation");
	printf("50us/15uS pre-emphasis %s indicated\n", (dummy8 & _BV(SPDIF_EMPH)) ? "not" : "is");
}


#define BUF_SIZE 9

static uint8_t inputBuffer[BUF_SIZE];


static int16_t getparam(uint8_t param_no) {
	uint8_t i;
	uint8_t *where = ((uint8_t *)inputBuffer) + 1;
	for (i = 1; i < BUF_SIZE; i++) {
		if (inputBuffer[i] == (uint8_t)',' || inputBuffer[i] == 0 || inputBuffer[i] == (uint8_t)'\n' || inputBuffer[i] == (uint8_t)'\r') {
			if (param_no == 0) {
				inputBuffer[i] = 0;
				return (int16_t)strtol((char *)where, (char **)NULL, 16);
			} else {
				param_no--;
				where = inputBuffer + i + 1;
			}
		}
	}
	
	return -1;
}


#define STATE_READY		0
#define STATE_DBGINPUT	1
#define STATE_RESPOND	2
#define STATE_POST 		3
#define SPDIF_IRQ		4

#define DBGINPUT_INIT	0
#define DBGINPUT_INPUT	1
#define DBGINPUT_DONE	2
#define DBGINPUT_WAIT	3

int8_t debug_getinput(int8_t initState) {
	static int8_t state = 0;
	
	static uint8_t bufidx;
	uint8_t input;
	
	if (initState != -1) {
		state = initState;
	}
	
	switch (state) {
		case DBGINPUT_INIT:
			bufidx = 0;
			state = DBGINPUT_INPUT;
			break;
		case DBGINPUT_INPUT:
			if (uart_available()) {
				if (bufidx < BUF_SIZE - 1) {
					inputBuffer[bufidx] = input = uart_getc();
					if (input == '\r') {
						state = DBGINPUT_DONE;
					} else {
						(void)putchar(input);
						bufidx++;
					}
					
				} else {
					state = DBGINPUT_DONE;
				}
			}
			break;
		case DBGINPUT_DONE:
			inputBuffer[++bufidx] = 0;
			printf("\n");
			state = DBGINPUT_WAIT;
			break;
		case DBGINPUT_WAIT:
			// endstate
			break;
	}
	
	return state;
}

void debug_respond() {
	uint8_t dummy8;
	uint16_t dummy16;
	uint8_t spdif_reg;
	
	switch (inputBuffer[0]) {
		case 'd':
			spdif_dumpRegisters();
			break;
		case 'i':
			dummy8 = (uint8_t)getparam(0);
			//spdif_selectChannel(dummy8);
			selectInputChannel(dummy8);
			printf("CH#%d\n", dummy8);
			break;
		case 's':
			spdif_reg = (uint8_t)getparam(0);
			dummy8 = (uint8_t)getparam(1);
			spdif_write(spdif_reg, dummy8);
			printf("CS8416:%02x=%02x\n", spdif_reg, dummy8);
			break;
		case 'S':
			decodeAndPrintStatus();
			break;
		case 'r':
			sound_reset();
			break;
		case 'p':
			ad1955_write(AD1955_REG00 | _BV(AD1955_R0_POWERDOWN));
			break;
		case 'P':
			_delay_ms(500);
			ad1955_write(AD1955_REG00 | 0);	// all defaults
			ad1955_write(AD1955_LEFTVOL | 0b1111111111111100);
			ad1955_write(AD1955_RIGHTVOL | 0b1111111111111100);
			
			_delay_ms(500);
			break;
		case 'D':
			dummy16 = getparam(0);
			printf("DAC<-%04x", dummy16);
			ad1955_write(dummy16);
			break;
		default:
			printf("?\n");
			break;
	}

	_delay_ms(500);
}

static int8_t main_state_machine() {
	static int8_t state = STATE_POST;
	static uint8_t skippy = 16;
	
	if (--skippy == 0) {
		lastReceiverStatus = spdif_readStatus();
		skippy = 8;
	} 
	
	switch (state) {
		case STATE_READY:
			if (spdif_queryStatusIRQ()) {
				spdif_clearIRQ();
				//state = SPDIF_IRQ;
				break;
			}
			if (uart_available()) {
				state = STATE_DBGINPUT;
				debug_getinput(DBGINPUT_INIT);
			}
			break;
		case STATE_DBGINPUT:
			if (debug_getinput(-1) == DBGINPUT_WAIT) {
				state = STATE_RESPOND;
			}
			break;
		case STATE_RESPOND:
			debug_respond();
			state = STATE_POST;
			break;
			
		case SPDIF_IRQ:
			//dummy8 = spdif_readStatus();
			//decodeAndPrintStatus();
			state = STATE_POST;
			break;
			
		case STATE_POST:
			printf("\n# ");
			state = STATE_READY;
			break;
	}
	
	return state;
}


void timer2_hook() {
	static uint8_t errorSkip = 0;
	static uint8_t stopped = 0;
	static uint8_t ran = 0;
	
	if (stopped) {
		if (--stopped == 0) {
			REQUEST_STATUS |= _BV(REQUEST_RUN);
			ran = 16;
		}
	} else {
		if ((lastReceiverStatus & _BV(SPDIF_STATUS_SILENCE)) != 0) {
			REQUEST_STATUS |= _BV(REQUEST_MUTE);
			PORTD |= _BV(5);
		} else if ((lastReceiverStatus & _BV(SPDIF_CONFIDENCE_ERROR)) != 0) {
			REQUEST_STATUS |= _BV(REQUEST_MUTE);
			errorSkip = 16;

			if (ran == 0) {
				REQUEST_STATUS |= _BV(REQUEST_STOP);
				stopped = 16;
			} else {
				ran--;
			}
			// blink error led only briefly
			setRedLedDuty(255);
		} else if ((lastReceiverStatus & _BV(SPDIF_STATUS_PCM)) == 0) {
			REQUEST_STATUS |= _BV(REQUEST_MUTE);
			if (ran == 0) {
				REQUEST_STATUS |= _BV(REQUEST_STOP);
				stopped = 16;
			} else {
				ran--;
			}
			PORTD |= _BV(5);
		} else {
			if (errorSkip == 0) {
				REQUEST_STATUS |= _BV(REQUEST_UNMUTE);
				PORTD &= ~(_BV(5));
			} else {
				errorSkip--;
			}
		}
	}
}


int main() {
	ioinit();
	timer2_init();
	timer2_overflow_hook = timer2_hook;
	timer1_init();
	
	_delay_ms(100);
	spi_init();

	sound_reset();
	
	usart_init(1);//	usart_init(23); 	// 9600bps

  	spdif_configure();
	ad1955_configure();
	ad1955_mute();
#define NO_NOISE_MEASURE
#ifdef NOISE_MEASURE
	ad1955_unmute();
	cli();
	set_sleep_mode(SLEEP_MODE_PWR_DOWN);
	for(;;) {
	sleep_mode();
	}
#endif	
	
	spdif_selectChannel(1);
	
   	spdif_dumpRegisters();
	
	printf("\n\nSVODAC V%s\n", idstring);

	for (;;) {
		if (main_state_machine() == STATE_READY) {
			if ((REQUEST_STATUS & _BV(REQUEST_MUTE)) != 0) {
				ad1955_mute();
			} else 	if ((REQUEST_STATUS & _BV(REQUEST_UNMUTE)) != 0) {
				ad1955_unmute();
			} 
			
			if ((REQUEST_STATUS & _BV(REQUEST_STOP)) != 0) {
				//printf("STOP");
				spdif_stop();
			} else if ((REQUEST_STATUS & _BV(REQUEST_RUN)) != 0) {
				//printf("RUN");
				spdif_run();
			}
			REQUEST_STATUS = 0;
			sleep_mode();
		}
	}

	return 0; // make happy
}

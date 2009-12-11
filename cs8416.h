#ifndef _CS8416_H
#define _CS8416_H

#include <inttypes.h>

#define SPDIF_CONTROL0	0x00
#define SPDIF_CONTROL1	0x01
#define SPDIF_CONTROL2	0x02
#define SPDIF_CONTROL3	0x03
#define SPDIF_CONTROL4	0x04
#define SPDIF_SADF		0x05		// Serial Audio Data Format Register (05h)
#define SPDIF_REM		0x06		// Receiver Error Mask (06h)
#define SPDIF_IM		0x07		// Interrupt Mask (07h)
#define SPDIF_IMODE_MSB	0x08		// Interrupt Mode MSB (08h)
#define SPDIF_IMODE_LSB	0x09		// Interrupt Mode LSB (09h)
#define SPDIF_RCSTAT	0x0a		// Receiver Channel Status (0Ah)
#define SPDIF_FDSTAT	0x0b		// Format Detect Status (0Bh)
#define SPDIF_RERROR	0x0c		// Receiver Error (0Ch), see SPDIF_REM
#define SPDIF_INT1STAT	0x0d		// Interrupt 1 Status (0Dh)
#define SPDIF_QCHAN		0x0e		// Q-Channel Subcode spans between 0x0e to 0x17
#define SPDIF_ORMCK_RATIO	0x18	// OMCK/RMCK Ratio (18h)
#define SPDIF_CSR_A0	0x19		// Channel Status Registers for Channel A
#define SPDIF_CSR_B0	0x1e		// Channel Status Registers for Channel B
#define SPDIF_IEC61937BP 	0x23	// IEC61937 Burst Preamble (23h - 26h)
#define SPDIF_VREG		0x7f		// CS8416 I.D. and Version Register (7Fh)

// Bit definitions for SPDIF_CONTROL0

#define SPDIF_FSWCLK	6			// Forces the clock signal on OMCK to be output on RMCK regardless of the SWCLK
#define	SPDIF_PDUR		3			// Normal/High Update Rate phase detector
#define SPDIF_TRUNC		2			// Truncate incoming data

// Bit definitions for SPDIF_CONTROL1

#define	SPDIF_SWCLK		7			// Lets OMCK determine RMCK, OSCLK, OLRCLK when PLL loses lock
#define SPDIF_MUTESAO	6			// Mute control for the serial audio output port
#define SPDIF_INT1		5			// Interrupt output pin control
#define SPDIF_INT0		4			//
#define SPDIF_HOLD1		3			//	
#define SPDIF_HOLD0		2			// How received audio sample is affected if receiver error occurs
#define SPDIF_RMCKF		1			// 0: RMCK = 256*Fs, 1: RMCK = 128*Fs
#define SPDIF_CHS		0			// Receiver Channel Status register decoded from A(0) channel or B(1) channel


// Bit definitions for SPDIF_CONTROL2
#define SPDIF_DETCI		7 			// D to E status transfer inhibit
#define SPDIF_EMPH_CNTL2	6		// De-emphasis control
#define SPDIF_EMPH_CNTL1	5		//
#define SPDIF_EMPH_CNTL0	4		//
#define SPDIF_GPO0SEL3	3			// GPO0 source select
#define SPDIF_GPO0SEL2	2
#define SPDIF_GPO0SEL1	1
#define SPDIF_GPO0SEL0	0

// Bit definitions for SPDIF_CONTROL3
#define SPDIF_GPO1SEL3	7			// GPO1 source select
#define SPDIF_GPO1SEL2	6
#define SPDIF_GPO1SEL1	5
#define SPDIF_GPO1SEL0	4
#define SPDIF_GPO2SEL3	3			// GPO2 source select
#define SPDIF_GPO2SEL2	2
#define SPDIF_GPO2SEL1	1
#define SPDIF_GPO2SEL0	0


// Bit definitions for SPDIF_CONTROL4
#define SPDIF_RUN 		7			// 1 = power on
#define SPDIF_RXD		6
#define SPDIF_RXSEL0	3			// Selects RXP0 to RXP7 for input to the receiver
#define SPDIF_TXSEL0	0			// Selects RXP0 to RXP7 as the input for GPO TX source

// Bit definitions for SPDIF_SADF
#define SPDIF_SOMS		7
#define SPDIF_SOSF		6
#define	SPDIF_SORES1	5
#define SPDIF_SORES0	4
#define SPDIF_SOJUST	3
#define SPDIF_SODEL		2
#define SPDIF_SOSPOL	1
#define SPDIF_SOLRPOL	0

// Bit definitions for SPDIF_REM
// Bit definitions for SPDIF_RERROR
#define SPDIF_QCRC		6			// Q-subcode data CRC error
#define SPDIF_CCRC		5			// Channel Status Block CRC error
#define SPDIF_UNLOCK	4			// PLL out of lock
#define SPDIF_V			3			// Received AES3 Validity bit status (1 = Confidence error)
#define SPDIF_CONF		2			// Condidence error (UNLOCK || BIP)
#define SPDIF_BIP		1			// Bi-phase error bit
#define SPDIF_PAR		0			// Parity error

// Bit definitions for SPDIF_IM
#define SPDIF_PCCHM		6			// PC burst preamble change	
#define SPDIF_OSLIPM	5			// Serial audio output port data slip interrupt
#define SPDIF_DETCM		4			// Indicates the completion of a D to E C-buffer transfer
#define SPDIF_CCHM		3			// Indicates that the current 10 bytes of channel status is different from the previous 10 bytes
#define SPDIF_RERRM		2			// A receiver error has occurred
#define SPDIF_QCHM		1			// A new block of Q-subcode is available for reading
#define SPDIF_FCHM		0			// Format Change

// Bit definitions for SPDIF_IMODE_MSB/SPDIF_IMODE_LSB
#define SPDIF_PCCH1		6			
#define SPDIF_OSLIP1	5
#define SPDIF_DETC1		4
#define SPDIF_CCH1		3
#define SPDIF_RERR1		2
#define SPDIF_QCH1		1
#define SPDIF_FCH1		0

#define SPDIF_PCCH0		6			
#define SPDIF_OSLIP0	5
#define SPDIF_DETC0		4
#define SPDIF_CCH0		3
#define SPDIF_RERR0		2
#define SPDIF_QCH0		1
#define SPDIF_FCH0		0

// Bit definitions for SPDIF_RCSTAT
#define SPDIF_AUX3		7			// Auxilliary data length
#define SPDIF_AUX2		6
#define SPDIF_AUX1		5
#define SPDIF_AUX0		4
#define SPDIF_PRO		3			// Indicates professional format
#define SPDIF_COPY		2			// 1 = Copyright not asserted
#define SPDIF_ORIG		1			// 1 = Receiver data is original
#define SPDIF_EMPH		0			// 0 = 50uS/15uS pre-emphasis indicated

// Bit definitions for SPDIF_FDSTAT
#define SPDIF_PCM		6			// PCM data was detected
#define SPDIF_IEC61937	5			// IEC61937 data was detected
#define SPDIF_DTS_LD	4			// DTS_LD data was detected
#define SPDIF_DTS_CD	3			// DTS_CD data was detected
#define SPDIF_SIL		1			// Digital Silence was detected
#define SPDIF_96KHZ		0			// 0 = input sample rate <= 48kHz

// Bit definitions for SPDIF_INT1STAT
#define SPDIF_PCCH		6			
#define SPDIF_OSLIP		5
#define SPDIF_DETC		4
#define SPDIF_CCH		3
#define SPDIF_RERR		2
#define SPDIF_QCH		1
#define SPDIF_FCH		0


// Bit definitions for spdif_readStatus()
#define SPDIF_STATUS_SILENCE 	0			// digital silence detected
#define SPDIF_STATUS_PCM		1			// input data is in PCM format
#define SPDIF_CONFIDENCE_ERROR	2			// parity+biphase error
#define SPDIF_INTERRUPT			7





void spdif_write(uint8_t reg, uint8_t data);
uint8_t spdif_read(uint8_t reg);
void spdif_dumpRegisters();
void spdif_configure();
void spdif_selectChannel(uint8_t channel);

/// Indicates that an interrupt has occured and status registers need to be read
uint8_t spdif_queryStatusIRQ();			
uint8_t spdif_readStatus();
void spdif_clearIRQ();

void spdif_stop();
void spdif_run();
#endif

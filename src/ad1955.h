#ifndef _AD1955_H
#define _AD1955_H

#include <inttypes.h>
#include "spi.h"

#define AD1955_REG00	0
#define AD1955_REG01	1
#define AD1955_LEFTVOL	2
#define AD1955_RIGHTVOL	3

#define AD1955_R0_POWERDOWN	15
#define AD1955_R0_MUTE		14
#define AD1955_R0_DFORMAT1	13
#define AD1955_R0_DFORMAT0	12
#define AD1955_R0_OFORMAT1	11
#define AD1955_R0_OFORMAT0	10
#define AD1955_R0_PCMRATE1	9
#define AD1955_R0_PCMRATE0	8
#define AD1955_R0_DEEMPH1	7
#define AD1955_R0_DEEMPH0	6
#define AD1955_R0_SDFORMAT1	5
#define AD1955_R0_SDFORMAT0	4
#define AD1955_R0_SDWIDTH1	3
#define AD1955_R0_SDWIDTH0	2

#define AD1955_R1_MCLKMODE1	10
#define AD1955_R1_MCLKMODE0	9
#define AD1955_R1_ZEROPOL	8
#define AD1955_R1_SACDBR	7
#define AD1955_R1_SACDM		6
#define AD1955_R1_SACDPS1	5
#define AD1955_R1_SACDPS0	4
#define AD1955_R1_SACDBI	3
#define AD1955_R1_SACDM2BP	2

void ad1955_write(uint16_t word);
void ad1955_configure(void);
void ad1955_mute();
void ad1955_unmute();
uint8_t ad1955_isMuted();
#endif

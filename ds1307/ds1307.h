#ifndef _DS1307_H
#define _DS1307_H

#ifdef __cplusplus
 extern "C" {
#endif

#include "main.h"
   
#define _ADDR_      0xd0 // I2C Address of DS1307
#define _CAP_       0x40 // Full Capacity of DS1307 - 8 bytes of calendar and 56 bytes of backup
   
#define _CH_        0x80 // Clock Halt
#define _RS0_       0x01 // Rate Select of Squarewave output frequency bit 0
#define _RS1_       0x02 // Rate Select of Squarewave output frequency bit 1
#define _SQWE_      0x10 // Square Wave Enable
#define _OUT_       0x80 // Output Control


bool Init_I2C_DS1307(void);
void DS1307_Init(uint8_t*);

void DS1307_Read(uint8_t, uint8_t, uint8_t*);
void DS1307_Write(uint8_t, uint8_t, uint8_t*);

#ifdef __cplusplus
}
#endif

#endif

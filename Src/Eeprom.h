#ifndef __EEPROM_H
#define __EEPROM_H

#include <stdbool.h>
#include <stdint.h>

//#define   _EEPROM_F1_LOW_DESTINY
#define   _EEPROM_F1_MEDIUM_DESTINY
//#define   _EEPROM_F1_HIGH_DESTINY

#define		_EEPROM_USE_FLASH_PAGE				120 


#define   U_INT8_SIZE     1
#define   U_INT16_SIZE    2
#define   U_INT32_SIZE    4
#define   U_INT64_SIZE    8
#define   FLOAT_SIZE      4
#define   DOUBLE_SIZE     8



//################################################################################################################
bool	EE_Format(void);
bool 	EE_Read(uint16_t VirtualAddress, uint32_t* Data);
bool 	EE_Write(uint16_t VirtualAddress, uint32_t Data);
bool	EE_MultiReads(uint16_t StartVirtualAddress,uint16_t HowMuchToRead,uint32_t* Data);
bool 	EE_MultiWrites(uint16_t StartVirtualAddress,uint16_t HowMuchToWrite,uint32_t* Data);
//################################################################################################################

#endif

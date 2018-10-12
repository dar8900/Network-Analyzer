#include "main.h"
#include "Eeprom.h"

//uint32_t	EEPROMPageBackupInt[_EEPROM_FLASH_PAGE_SIZE/4];

//##########################################################################################################
//ERASE
//##########################################################################################################
bool EE_Format(void)
{
	uint32_t	error;
	HAL_FLASH_Unlock();
	FLASH_EraseInitTypeDef	flashErase;
	flashErase.NbPages=1;
	flashErase.Banks = FLASH_BANK_1;
	flashErase.PageAddress = _EEPROM_FLASH_PAGE_ADDRESS;
	flashErase.TypeErase = FLASH_TYPEERASE_PAGES;
	if(HAL_FLASHEx_Erase(&flashErase,&error)==HAL_OK)
	{
		HAL_FLASH_Lock();
		if(error != 0xFFFFFFFF)
			return false;
		else
			return true;	
	}
	HAL_FLASH_Lock();
	return false;	
}

//##########################################################################################################

// LETTURA

//##########################################################################################################



bool EE_ReadInt(uint16_t VirtualAddress, uint32_t* Data)
{
	if(VirtualAddress >=	(_EEPROM_FLASH_PAGE_SIZE/4))
		return false;
	*Data =  (*(__IO uint32_t*)((VirtualAddress*4)+_EEPROM_FLASH_PAGE_ADDRESS));
	return true;
}


bool EE_MultiReadsInt(uint16_t StartVirtualAddress,uint16_t HowMuchToRead, uint32_t* Data)
{
	if((StartVirtualAddress+HowMuchToRead) >	(_EEPROM_FLASH_PAGE_SIZE/4))
		return false;
	for(uint16_t	i=StartVirtualAddress ; i<HowMuchToRead+StartVirtualAddress ; i++)
	{
		*Data =  (*(__IO uint32_t*)((i*4)+_EEPROM_FLASH_PAGE_ADDRESS));
		Data++;
	}
	return true;
}

//##########################################################################################################

// SCRITTURA

//##########################################################################################################
//bool EE_WriteInt(uint16_t VirtualAddress, uint32_t Data)
//{
//    uint32_t OldValue = 0;
//	if(VirtualAddress >=	(_EEPROM_FLASH_PAGE_SIZE/4))
//		return false;
//    
////	if((*(__IO uint32_t*)((VirtualAddress*4)+_EEPROM_FLASH_PAGE_ADDRESS)) != 0xFFFFFFFF)
////	{
////		
////		if( EE_MultiReadsInt(0,(_EEPROM_FLASH_PAGE_SIZE/4),EEPROMPageBackupInt)==false)
////		{
////			HAL_FLASH_Lock();
////			return false;
////		}
////		EEPROMPageBackupInt[VirtualAddress]=Data;
////		EE_Format();
////		
////		HAL_FLASH_Unlock();
////		for(uint16_t	i=0 ; i<_EEPROM_FLASH_PAGE_SIZE/4 ; i++)
////		{
////            if(HAL_FLASH_Program(FLASH_TYPEPROGRAM_WORD,(i*4)+_EEPROM_FLASH_PAGE_ADDRESS,(uint64_t)EEPROMPageBackupInt[i])!=HAL_OK)
////			{
////				HAL_FLASH_Lock();
////				return false;
////			}			
////		}
////	}	
//    if(EE_Format()==false)
//		return false;
//	HAL_FLASH_Unlock();
//	for(uint16_t i = 0 ; i < (_EEPROM_FLASH_PAGE_SIZE/4); i++)
//	{
//        EE_ReadInt(i, &OldValue);
//        if(Data[i] != OldValue)
//        {
//            if(HAL_FLASH_Program(FLASH_TYPEPROGRAM_WORD,(i*4)+_EEPROM_FLASH_PAGE_ADDRESS,(uint64_t)Data[i])!=HAL_OK)
//            {
//                HAL_FLASH_Lock();
//                return false;
//            }
//        }
//	}
//	HAL_FLASH_Lock();
//	return true;
//    
//}

bool EE_Write(uint32_t Data[])
{
    uint32_t OldValue = 0;
	if(EE_Format()==false)
		return false;    
	HAL_FLASH_Unlock();
	for(uint16_t i = 0 ; i < (_EEPROM_FLASH_PAGE_SIZE/4); i++)
	{
        EE_ReadInt(i, &OldValue);
        if(Data[i] != OldValue)
        {
            if(HAL_FLASH_Program(FLASH_TYPEPROGRAM_WORD,(i*4)+_EEPROM_FLASH_PAGE_ADDRESS,(uint64_t)Data[i])!=HAL_OK)
            {
                HAL_FLASH_Lock();
                return false;
            }
        }
	}
	HAL_FLASH_Lock();
	return true;
}

//##########################################################################################################

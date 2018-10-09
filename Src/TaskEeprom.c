#include "main.h"
#include "TaskEeprom.h"
#include "Eeprom.h"

#ifdef ENABLE_EEPROM



/* TaskEeprom function */
void TaskEeprom(void const * argument)
{
    uint32_t ReadedValue = 10;
    EE_Read(0, &ReadedValue);
    ReadedValue = 11;
    
    /* Infinite loop */
    for(;;)
    {
        osDelay(1000);
    }
    
}

#endif
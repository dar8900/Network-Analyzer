#include "main.h"
#include "SysTime.h"
#include "I2C.h"
#include "TaskRTC.h"
#include "TaskEeprom.h"
#include "Parameters.h"
#include "Menus.h"

#ifdef ENABLE_RTC


TIME_VAR GlobalTime;

bool SecondTick = false;

bool SettingTimeDate = false;

uint8_t DaysPerMonth[] = 
{
    31,
    28,
    31,
    30, 
    31,
    30,
    31,
    31,
    30,
    31,
    30,
    31,
};

typedef enum
{
    SINGLE_READ_NO_REG = 0,
    SINGLE_READ_REG,
    MULTI_READ_NO_REG,
    MULTI_READ_REG
}READ_TYPE_I2C;

typedef enum
{
    SINGLE_WRITE_NO_REG = 0,
    SINGLE_WRITE_REG,
    MULTI_WRITE_NO_REG,
    MULTI_WRITE_REG
}WRITE_TYPE_I2C;

/******************************************************************************
*	@brief	Read bytes from slave with specify register address               *                   
* @param	Register address                                                  *
* @param	Number of data bytes to read from slave                           *
* @param	Pointer to data array byte to store data from slave               *
* @retval	None                                                              *
*******************************************************************************
*/
static void ReadRtcI2C(uint8_t reg, uint8_t* data, uint8_t len, uint8_t WichFunc)
{
    uint8_t address = DS1307_DEVICE_ADDRESS;
    switch(WichFunc)
    {
      case SINGLE_READ_NO_REG:
        HAL_I2C_Master_Receive(&hi2c1, address << 1, data, 1, STM32_HAL_I2C_TIMEOUT);
        break;
      case SINGLE_READ_REG:
        HAL_I2C_Mem_Read(&hi2c1, address << 1, reg, 1, data, 1, STM32_HAL_I2C_TIMEOUT);
        break;
      case MULTI_READ_NO_REG:
        HAL_I2C_Master_Receive(&hi2c1, address << 1, data, len, STM32_HAL_I2C_TIMEOUT);
        break;
      case MULTI_READ_REG:
        HAL_I2C_Mem_Write(&hi2c1, address << 1, reg, 1, data, len, STM32_HAL_I2C_TIMEOUT);
        break;
      default:
        break;        
    }
    return;
}

/**
******************************************************************************
*	@brief	Write bytes to slave with specify register address where to      *
*					start write                                              *                     
* @param	Register address where to start write                            *
* @param	Pointer to data byte array                                       *
* @param	Number of bytes to write                                         *
* @retval	None                                                             *
******************************************************************************
*/
static void WriteRtcI2C(uint8_t reg, uint8_t* data, uint8_t len, uint8_t WichFunc)
{
    uint8_t address = DS1307_DEVICE_ADDRESS;
    switch(WichFunc)
    {
      case SINGLE_WRITE_NO_REG:
        HAL_I2C_Master_Transmit(&hi2c1, address << 1, data, 1, STM32_HAL_I2C_TIMEOUT);
        break;
      case SINGLE_WRITE_REG:
        HAL_I2C_Mem_Write(&hi2c1, address << 1, reg, 1, data, 1, STM32_HAL_I2C_TIMEOUT);
        break;
      case MULTI_WRITE_NO_REG:
        HAL_I2C_Master_Transmit(&hi2c1, address << 1, data, len, STM32_HAL_I2C_TIMEOUT);
        break;
      case MULTI_WRITE_REG:
        HAL_I2C_Mem_Write(&hi2c1, address << 1, reg, 1, data, len, STM32_HAL_I2C_TIMEOUT);
        break;
      default:
        break;        
    }
    return;
}



/*
******************************************************************************
*	@brief	Convert from BCD format to BIN format
* @param	BCD value to be converted
* @retval	BIN value from given BCD
******************************************************************************
*/
static uint8_t bcd2bin(uint8_t bcd)
{
	uint8_t bin = (bcd >> 4) * 10;
	bin += bcd & 0x0F;
	
	return bin;
}

/**
******************************************************************************
*	@brief	Convert from BIN format to BCD format
* @param	BIN value to be converted
* @retval	BCD value from given BIN
******************************************************************************
*/
static uint8_t bin2bcd(uint8_t bin)
{
	uint8_t high = bin / 10;
	uint8_t low = bin - (high *10);
	
	return (high << 4) | low;
}

/**
******************************************************************************
*	@brief	Check min and max from given value
* @param	The value to be checked
* @param	Allowed minimum value
* @param	Allowed maximum value
* @retval	Value between min and max or equal min or max
******************************************************************************
*/
static uint8_t check_min_max(uint8_t val, uint8_t min, uint8_t max)
{
	if (val < min)
		return min;
	else if (val > max)
		return max;
	
	return val;
}

/**
******************************************************************************
*	@brief	Get current seconds value from the register
* @param	None
* @retval	Current seconds value in decimal format
******************************************************************************
*/
uint8_t ds1307_get_seconds()
{
	uint8_t seconds;
    ReadRtcI2C(DS1307_SECONDS, &seconds, 0, SINGLE_READ_REG);
	
	return bcd2bin(seconds);
}

/**
******************************************************************************
*	@brief	Set seconds value to the register
* @param	Seconds value to be set
* @retval	None
******************************************************************************
*/
void ds1307_set_seconds(uint8_t seconds)
{
    uint8_t SetBcd = bin2bcd(check_min_max(seconds, 0, 59));
    WriteRtcI2C(DS1307_SECONDS, &SetBcd, 0, SINGLE_WRITE_REG);
}

/**
******************************************************************************
*	@brief	Get current minutes value from the register
* @param	None
* @retval	Current minutes value in decimal format
******************************************************************************
*/
uint8_t ds1307_get_minutes()
{
	uint8_t minutes;
	ReadRtcI2C(DS1307_MINUTES, &minutes, 0, SINGLE_READ_REG);
	return bcd2bin(minutes);
}

/**
******************************************************************************
*	@brief	Set minutes value to the register
* @param	Minutes value to be set
* @retval	None
******************************************************************************
*/
void ds1307_set_minutes(uint8_t minutes)
{
    uint8_t SetBcd = bin2bcd(check_min_max(minutes, 0, 59));
    WriteRtcI2C(DS1307_MINUTES, &SetBcd, 0, SINGLE_WRITE_REG);
}

/**
******************************************************************************
*	@brief	Get current hour mode (12 or 24)
* @param	None
* @retval	Current hour mode (DS1307_HOUR_12 or DS1307_HOUR_24)
******************************************************************************
*/
uint8_t ds1307_get_hour_mode()
{
	uint8_t hours;
	ReadRtcI2C(DS1307_HOURS, &hours, 0, SINGLE_READ_REG);
	// If hour mode bit is set, then current hour mode is 12, otherwise 24
	return ((hours & (1 << DS1307_HOUR_MODE)) == (1 << DS1307_HOUR_MODE)) ?   DS1307_HOUR_12 : DS1307_HOUR_24;
}

/**
******************************************************************************
*	@brief	Get current am pm (AM or PM)
* @param	None
* @retval	Current am pm (DS1307_AM or DS1307_PM)
******************************************************************************
*/
uint8_t ds1307_get_am_pm()
{
	uint8_t hours;
	i2c_read_with_reg(DS1307_DEVICE_ADDRESS, DS1307_HOURS, &hours);
	ReadRtcI2C(DS1307_HOURS, &hours, 0, SINGLE_READ_REG);
	// If AM PM bit is set, then current AM PM mode is PM, otherwise AM
	return ((hours & (1 << DS1307_AM_PM)) == (1 << DS1307_AM_PM)) ?  DS1307_PM : DS1307_AM;
}

/**
******************************************************************************
*	@brief	Get hours when hour mode is 12 
* @param	None
* @retval	Current hour value in decimal format
******************************************************************************
*/
uint8_t ds1307_get_hours_12()
{
	uint8_t hours;
	ReadRtcI2C(DS1307_HOURS, &hours, 0, SINGLE_READ_REG);
	// Mask hour register value from bit 7 to bit 5 to get hours value
	// in hour mode 12
	return bcd2bin(hours & 0x1F);
}

/**
******************************************************************************
*	@brief	Set hours in hour mode 12
* @param	Hours value
* @param	AM or PM time
* @retval	None
******************************************************************************
*/
void ds1307_set_hours_12(uint8_t hours_12, uint8_t am_pm)
{
	uint8_t hours = 0;
	if (am_pm == DS1307_AM)
	{
		// Hour mode is set, in order to use hour mode 12
		// AM/PM bit is clear, in order to set AM time
		hours = (1 << DS1307_HOUR_MODE) | 
			bin2bcd(check_min_max(hours_12, 1, 12));
	}
	else if (am_pm == DS1307_PM)
	{
		// Hour mode is set, in order to use hour mode 12
		// AM/PM bit is set, in order to set PM time
		hours = (1 << DS1307_HOUR_MODE) | (1 << DS1307_AM_PM) | 
			bin2bcd(check_min_max(hours_12, 1, 12));
	}
    
    WriteRtcI2C(DS1307_HOURS, &hours, 0, SINGLE_WRITE_REG);
}

/**
******************************************************************************
*	@brief	Get hours when hour mode is 24
* @param	None
* @retval	Current hour value in decimal format
******************************************************************************
*/
uint8_t ds1307_get_hours_24()
{
	uint8_t hours;
	ReadRtcI2C(DS1307_HOURS, &hours, 0, SINGLE_READ_REG);
	// Mask hour register value from bit 7 to bit 6 to get hours value
	// in hour mode 24
	return bcd2bin(hours & 0x3F);
}

/**
******************************************************************************
*	@brief	Set hours in hour mode 24
* @param	Hours value
* @retval	None
******************************************************************************
*/
void ds1307_set_hours_24(uint8_t hours_24)
{
    uint8_t SetBcd = bin2bcd(check_min_max(hours_24, 0, 59));
    WriteRtcI2C(DS1307_HOURS, &SetBcd, 0, SINGLE_WRITE_REG);
}

/**
******************************************************************************
*	@brief	Get current day of week value from the register
* @param	None
* @retval	Current day of week value in decimal format
******************************************************************************
*/
uint8_t ds1307_get_day()
{
	uint8_t day;
    ReadRtcI2C(DS1307_DAY, &day, 0, SINGLE_READ_REG);
	return bcd2bin(day);
}

/**
******************************************************************************
*	@brief	Set day of week value to the register
* @param	Day of week value to be set
* @retval	None
******************************************************************************
*/
void ds1307_set_day(uint8_t day)
{
    uint8_t SetBcd = bin2bcd(check_min_max(day, 0, 59));
    WriteRtcI2C(DS1307_DAY, &SetBcd, 0, SINGLE_WRITE_REG);
}

/**
******************************************************************************
*	@brief	Get current date value from the register
* @param	None
* @retval	Current date value in decimal format
******************************************************************************
*/
uint8_t ds1307_get_date()
{
	uint8_t date;
	ReadRtcI2C(DS1307_DATE, &date, 0, SINGLE_READ_REG);
	return bcd2bin(date);
}

/**
******************************************************************************
*	@brief	Set date value to the register
* @param	Date value to be set
* @retval	None
******************************************************************************
*/
void ds1307_set_date(uint8_t date)
{
    uint8_t SetBcd = bin2bcd(check_min_max(date, 0, 59));
    WriteRtcI2C(DS1307_DATE, &SetBcd, 0, SINGLE_WRITE_REG);
}

/**
******************************************************************************
*	@brief	Get current month value from the register
* @param	None
* @retval	Current month value in decimal format
******************************************************************************
*/
uint8_t ds1307_get_month()
{
	uint8_t month;
	ReadRtcI2C(DS1307_MONTH, &month, 0, SINGLE_READ_REG);
	return bcd2bin(month);
}

/**
******************************************************************************
*	@brief	Set month value to the register
* @param	Month value to be set
* @retval	None
******************************************************************************
*/
void ds1307_set_month(uint8_t month)
{
    uint8_t SetBcd = bin2bcd(check_min_max(month, 0, 59));
    WriteRtcI2C(DS1307_MONTH, &SetBcd, 0, SINGLE_WRITE_REG);
}

/**
******************************************************************************
*	@brief	Get current year value from the register
* @param	None
* @retval	Current year value in decimal format
******************************************************************************
*/
uint8_t ds1307_get_year()
{
	uint8_t year;
	ReadRtcI2C(DS1307_YEAR, &year, 0, SINGLE_READ_REG);
	return bcd2bin(year);
}

/**
******************************************************************************
*	@brief	Set year value to the register
* @param	Year value to be set
* @retval	None
******************************************************************************
*/
void ds1307_set_year(uint8_t year)
{
    uint8_t SetBcd = bin2bcd(check_min_max(year, 0, 59));
    WriteRtcI2C(DS1307_YEAR, &SetBcd, 0, SINGLE_WRITE_REG);
}



/**
******************************************************************************
*	@brief	Get hours, minutes, and seconds value from the register in 24 
*					hour mode
* @param	Pointer to variable for store hours value
* @param	Pointer to variable for store minutes value
* @param	Pointer to variable for store seconds value
* @retval	None
******************************************************************************
*/
void ds1307_get_time_24(uint8_t* hours, uint8_t* minutes, uint8_t* seconds)
{
	*hours = ds1307_get_hours_24();
	*minutes = ds1307_get_minutes();
	*seconds = ds1307_get_seconds();
}

/**
******************************************************************************
*	@brief	Set hours, minutes, and seconds value to the register in 24 
*					hour mode
* @param	Hours value to be set
* @param	Minutes value to be set
* @param	Seconds value to be set
* @retval	None
******************************************************************************
*/
void ds1307_set_time_24(uint8_t hours, uint8_t minutes, uint8_t seconds)
{
	ds1307_set_hours_24(hours);
	ds1307_set_minutes(minutes);
	ds1307_set_seconds(seconds);
}


static void GetTimeDate(TIME_VAR *Time)
{
    ds1307_get_time_24(&Time->hours, &Time->minutes, &Time->seconds);
    Time->day   = ds1307_get_date();
    Time->month = ds1307_get_month();
    Time->year  = ds1307_get_year();
}


void SetChangedTime(uint8_t Hour, uint8_t Minute)
{
    ds1307_set_time_24(Hour, Minute ,0); 
}

void SetChangedDate(uint8_t Day, uint8_t Month, uint8_t Year)
{
    ds1307_set_date(Day);
    ds1307_set_month(Month);
    ds1307_set_year(Year);
}

static bool IsRtcRunning()
{
    GetTimeDate(&GlobalTime); 
    if(GlobalTime.hours == 0 && GlobalTime.minutes == 0 && GlobalTime.seconds == 0)
    {
        if(GlobalTime.day == 0 && GlobalTime.month == 0 && GlobalTime.year == 0)
            return false;
    }
    return true;
}

/* TaskRTC function */
void TaskRTC(void const * argument)
{
    if(!IsRtcRunning) 
    {
        ChangeTime();
        ChangeDate();
    }
    TickForSecond = 0;
    
    /* Infinite loop */
    for(;;)
    {   
        GetTimeDate(&GlobalTime); 
        osDelay(500);     
    }
    /* USER CODE END TaskRTC */
}

#endif // ENABLE_RTC
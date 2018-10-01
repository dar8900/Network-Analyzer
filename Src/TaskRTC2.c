#include "main.h"
#include "TaskRTC2.h"

#ifdef ENABLE_RTC

#define DS1307_I2C_TIMEOUT  2000

DATE_TIME_S GlobalTime;

uint8_t SecondCnt;
bool SecondTick = false;
bool HalfSecondTick = false;
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



extern I2C_HandleTypeDef hi2c1;

uint8_t* readTime(void) 
{
    uint8_t *received_data = calloc(TIME_STRUCT_SIZE, sizeof(uint8_t));
    
    received_data = read(0x00, TIME_STRUCT_SIZE, false);
    
    return received_data;
}

void decodeTime(const uint8_t *data, DATE_TIME_S *s_time)
{
    // decode seconds
    uint8_t msd = 0, lsd = 0;
    uint8_t /*am_pm = -1,*/_12h_mode = -1;
    
    lsd = (data[0] & 0x0F);
    msd = (data[0] & 0x70) >> 4;
    s_time->seconds = lsd + 10 * msd;
    
    lsd = (data[1] & 0x0F);
    msd = (data[1] & 0x70) >> 4;
    s_time->minutes = lsd + 10 * msd;
    
    // If 1, then 12-hour mode is enabled, 0 - 24-hour mode
    _12h_mode = (data[2] & 0x40) >> 6;
    
    // When 12-hour mode enabled, PM = 1, AM = 0, otherwise first bit of
    // hour_msd
    if (_12h_mode) 
    {
        //am_pm = (data[2] & 0b00100000) >> 5;
        msd = (data[2] & 0x10) >> 4;
    } 
    else 
    {
        msd = (data[2] & 0x30) >> 4;
    }
    lsd = (data[2] & 0x0F);
    s_time->hours = lsd + 10 * msd;
    
    s_time->day_of_week = (data[3] & 0x07);
    
    lsd = (data[4] & 0x0F);
    msd = (data[4] & 0x30) >> 4;
    s_time->day = lsd + 10 * msd;
    
    lsd = (data[5] & 0x0F);
    msd = (data[5] & 0x10) >> 4;
    s_time->month = lsd + 10 * msd;
    
    lsd = (data[6] & 0x0F);
    msd = (data[6] & 0xF0) >> 4;
    s_time->year = lsd + 10 * msd;
    
    s_time->clock_halt = (data[0] & 0x80) >> 7;
    s_time->out = (data[7] & 0x80) >> 7;
    s_time->sqwe = (data[7] & 0x10) >> 4;
    s_time->rs1 = (data[7] & 0x02) >> 1;
    s_time->rs0 = (data[7] & 0x01);
}

// Each number is represented in BCD format, according to documentation
uint8_t* encodeData(const DATE_TIME_S *s_time)
{
    uint8_t *data = calloc(TIME_STRUCT_SIZE, sizeof(uint8_t));
    uint8_t msd, lsd;
    
    // 0x00 Clock halt and seconds
    msd = s_time->seconds / 10;
    lsd = s_time->seconds - msd * 10;
    data[0] = (s_time->clock_halt << 7) | (msd << 4) | (lsd);
    
    // 0x01 Minutes
    msd = s_time->minutes / 10;
    lsd = s_time->minutes - msd * 10;
    data[1] = (msd << 4) | (lsd);
    
    // 0x02 Hours
    msd = s_time->hours / 10;
    lsd = s_time->hours - msd * 10;
    data[2] = (0 << 6 /*24h mode*/) | (msd << 4) | (lsd);
    
    // 0x03 Day of week
    data[3] = s_time->day_of_week;
    
    // 0x04 Date (day of month)
    msd = s_time->day / 10;
    lsd = s_time->day - msd * 10;
    data[4] = (msd << 4) | (lsd);
    
    // 0x05 Month
    msd = s_time->month / 10;
    lsd = s_time->month - msd * 10;
    data[5] = (msd << 4) | (lsd);
    
    // 0x06 Year
    msd = s_time->year / 10;
    lsd = s_time->year - msd * 10;
    data[6] = (msd << 4) | (lsd);
    
    // 0x07 Control part:
    // OUT, SQWE, RS1 and RS0
    data[7] = (s_time->out << 7) | (s_time->sqwe << 4) | (s_time->rs1 << 1)
        | (s_time->rs0);
    
    return data;
}

void writeTime(const DATE_TIME_S *s_time)
{
    uint8_t *data = encodeData(s_time);
    printRawData(data, TIME_STRUCT_SIZE);
    
    write(0x00, TIME_STRUCT_SIZE, false, data);
    
    free(data);
}

void printTime(const DATE_TIME_S *s_time) 
{
    printf("%2d:%2d:%2d\n\r", s_time->hours, s_time->minutes, s_time->seconds);
    
    switch (s_time->day_of_week) 
    {
      case 1:
        printf("Monday\n\r");
        break;
      case 2:
        printf("Tuesday\n\r");
        break;
      case 3:
        printf("Wednesday\n\r");
        break;
      case 4:
        printf("Thursday\n\r");
        break;
      case 5:
        printf("Friday\n\r");
        break;
      case 6:
        printf("Saturday\n\r");
        break;
      case 7:
        printf("Sunday\n\r");
        break;
        
      default:
        printf("BORKEN\n\r");
        break;
    }
    
    printf("%2d.%2d.%2d\n\r", s_time->day, s_time->month, s_time->year);
    printf("Config: CH: %d OUT: %d SQWE: %d RS1: %d RS0: %d\n\r",
           s_time->clock_halt, s_time->out, s_time->sqwe, s_time->rs1,
           s_time->rs0);
}

void printByte(const uint8_t n) 
{
    unsigned i;
    for (i = 1 << 7; i > 0; i = i / 2)
        (n & i) ? printf("1") : printf("0");
        printf("\n\r");
}

void printRawData(const uint8_t *data, const uint8_t size)
{
    if (data == 0) 
    {
        return;
    }
    
    uint8_t idx;
    
    for (idx = 0; idx < size; ++idx)
    {
        printByte(data[idx]);
    }
}

DATE_TIME_S getTime(void) 
{
    uint8_t* data = readTime();
    DATE_TIME_S s_time;
    decodeTime(data, &s_time);
    free(data);
    return s_time;
}

inline bool checkAddreses(const uint8_t start_address, const uint8_t allocated_bytes)
{
    if ((allocated_bytes <= 0) || (start_address < 0)
        || (start_address + allocated_bytes > RAM_SIZE)) {
            return false;
        }
    return true;
}

uint8_t* read(const uint8_t start_address, const uint8_t allocated_bytes, const bool ram)
{
    if (!checkAddreses(start_address, allocated_bytes)) 
    {
        return 0;
    }
    
    uint8_t *received_data = calloc(allocated_bytes, sizeof(uint8_t));
    
    uint8_t index = start_address;
    
    if(ram)
    {
        index += TIME_STRUCT_SIZE;
        HAL_I2C_Master_Transmit(&hi2c1, DS1307_ADDRESS, &index, 1, DS1307_I2C_TIMEOUT);
    }
    else
    {
        HAL_I2C_Master_Transmit(&hi2c1, DS1307_ADDRESS, &index, 1, DS1307_I2C_TIMEOUT);
    }
                                
    HAL_I2C_Master_Receive(&hi2c1, DS1307_ADDRESS, received_data, allocated_bytes, DS1307_I2C_TIMEOUT);                            
                                
//    I2C_start(I2C1, SLAVE_ADDRESS << 1, I2C_Direction_Transmitter); // start a transmission in Master transmitter mode
//    if (ram) 
//    {
//        I2C_write(I2C1, start_address + TIME_STRUCT_SIZE); // set pointer to read (with offset)
//    } 
//    else 
//    {
//        I2C_write(I2C1, start_address); //
//    }
//    I2C_stop(I2C1); // stop the transmission
//    I2C_start(I2C1, SLAVE_ADDRESS << 1, I2C_Direction_Receiver); // start a transmission in Master receiver mode
//    
//    for (index = 0; index < allocated_bytes; ++index)
//    {
//        if (index + 1 < allocated_bytes) 
//        {
//            received_data[index] = I2C_read_ack(I2C1); // read one byte, send ack and request another byte
//        } 
//        else 
//        {
//            received_data[index] = I2C_read_nack(I2C1); // read one byte and don't request another byte, stop transmission
//        }
//    }
    
    return received_data;
}

void write(const uint8_t start_address, const uint8_t bytes, const bool ram, uint8_t* data) 
{
    uint8_t index = start_address;
    if (!checkAddreses(start_address, bytes)) 
    {
        return;
    }
    if(ram)
    {
        index += TIME_STRUCT_SIZE;
        HAL_I2C_Master_Transmit(&hi2c1, DS1307_ADDRESS, &index, 1, DS1307_I2C_TIMEOUT);
    }
    else
    {
        HAL_I2C_Master_Transmit(&hi2c1, DS1307_ADDRESS, &index, 1, DS1307_I2C_TIMEOUT);
    } 
    
    HAL_I2C_Master_Transmit(&hi2c1, DS1307_ADDRESS, data, bytes, DS1307_I2C_TIMEOUT);
    
//    I2C_start(I2C1, SLAVE_ADDRESS << 1, I2C_Direction_Transmitter); // start a transmission in Master transmitter mode
//    
//    if (ram) 
//    {
//        I2C_write(I2C1, start_address + TIME_STRUCT_SIZE); // set pointer to read (with offset)
//    } 
//    else
//    {
//        I2C_write(I2C1, start_address); //
//    }
//    
    
//    for (index = 0; index <= bytes; index += 1)
//    {
//        I2C_write(I2C1, data[index]); // write one byte to the slave
//    }
//    I2C_stop(I2C1); // stop the transmission
}

static void GetSecondTick()
{
    switch(SecondCnt)
    {
      case 2:
        SecondTick = true;
        SecondCnt++;
        break;
      case 4:
        SecondTick = false;
        SecondCnt = 0;
        break;
      default:
        SecondCnt++;
        break;
    }
    HalfSecondTick = !HalfSecondTick;
}


/* TaskRTC function */
void TaskRTC(void const * argument)
{
    /* USER CODE BEGIN TaskRTC */
     
    /* Infinite loop */
    for(;;)
    {
        GetSecondTick();
        if(SecondTick && HalfSecondTick)
        {
              GlobalTime.seconds++;
              if(GlobalTime.seconds == 60)
              {
                GlobalTime.seconds = 0;
                GlobalTime.minutes++;
                if(GlobalTime.minutes == 60)
                {
                    GlobalTime.minutes = 0;
                    GlobalTime.hours++;
                    if(GlobalTime.hours == 24)
                    {
                        GlobalTime.hours = 0;
                    }
                }
              }
        }
        osDelay(500);
    }
    /* USER CODE END TaskRTC */
}

#endif
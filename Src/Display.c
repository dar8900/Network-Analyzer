#include "main.h"
#include "SysTime.h"
#include "I2C.h"
#include "Display.h"
#include "DisplayDef.h"
#include "TaskRTC.h"
#include "Menus.h"
#include "AlarmMachine.h"


extern bool HalfSecondTick;
extern const char AlarmIconSmall[];

extern const char BatteriaVuota[];
extern const char BatteriaQuarto[];
extern const char BatteriaMeta[];
extern const char Batteria3_4[];
extern const char BatteriaPiena[];
extern const char BatteriaInCarica[];

extern const char CronoIcon[];

extern char *AlarmList[MAX_ALARM_STR];
extern char *AlarmMotivationStr[2];
extern ALARM_CONTROLS AlarmsControls[MAX_ALARM_NUMBER];
extern ALARM_CONTROLS AlarmsControls[MAX_ALARM_NUMBER];

extern uint32_t  PowerOnTime;


extern TIME_VAR GlobalTime;
extern TIME_VAR ActiveTime;
extern CHRONO_VAR Crono;

extern bool SetChrono;

u8g2_t u8g;


typedef enum
{
    BATTERIA_VUOTA = 0,
    BATTERIA_1_4,
    BATTERIA_META,
    BATTERIA_3_4,
    BATTERIA_PIENA,
    BATTERIA_IN_CARICA,
    MAX_BATTERY_ICONS
}BATTERY_ICONS;

const char *BatteryIcons[MAX_BATTERY_ICONS] = 
{
    BatteriaVuota,
    BatteriaQuarto,
    BatteriaMeta,
    Batteria3_4,
    BatteriaPiena,
    BatteriaInCarica,
};

typedef enum
{
    MENU_STR = 0,
    SU_STR,
    GIU_STR,
    BACK_STR,
    OK_STR,
    POS_STR,
    PAGE_STR,
    OK_LONG_STR,
    CONFIRM_OK,
    LEFT_BACK,
    MAX_BOTTOM_BAR_ITEM    
}BOTTOM_BAR_ITEMS;

const char *BarItem[] = 
{
    "Menu",
    "Su",
    "Giu",
    "Back",
    "Ok",   
    "Pos.",
    "Page",
    "Premere ok per confermare",
    "Premere Left per uscire",
};

#ifdef USING_OLED

static uint8_t u8x8_gpio_and_delay_STM32F103(u8x8_t *u8x8, uint8_t msg, uint8_t arg_int, void *arg_ptr)
{
    switch(msg)
    {
      case U8X8_MSG_GPIO_AND_DELAY_INIT:	// called once during init phase of u8g2/u8x8
        break;							// can be used to setup pins
      case U8X8_MSG_DELAY_NANO:			// delay arg_int * 1 nano second
        break;    
      case U8X8_MSG_DELAY_100NANO:		// delay arg_int * 100 nano seconds
        break;
      case U8X8_MSG_DELAY_10MICRO:		// delay arg_int * 10 micro seconds
        break;
      case U8X8_MSG_DELAY_MILLI:			// delay arg_int * 1 milli second
        HAL_Delay(arg_int);
        break;
      case U8X8_MSG_DELAY_I2C:				// arg_int is the I2C speed in 100KHz, e.g. 4 = 400 KHz
        break;							// arg_int=1: delay by 5us, arg_int = 4: delay by 1.25us
      case U8X8_MSG_GPIO_I2C_CLOCK:		// arg_int=0: Output low at I2C clock pin
        break;							// arg_int=1: Input dir with pullup high for I2C clock pin
      case U8X8_MSG_GPIO_I2C_DATA:			// arg_int=0: Output low at I2C data pin
        break;							// arg_int=1: Input dir with pullup high for I2C data pin
      default:
        //        u8x8_SetGPIOResult(u8x8, 1);			// default return value
        break;
    }
    return 1;
}

static uint8_t i2c_STM32F103_HAL(u8x8_t *u8x8, uint8_t msg, uint8_t arg_int, void *arg_ptr)
{
    static uint8_t buffer[32];		/* u8g2/u8x8 will never send more than 32 bytes between START_TRANSFER and END_TRANSFER */
    static uint8_t buf_idx;
    uint8_t *data;
    
    switch(msg)
    {
      case U8X8_MSG_BYTE_START_TRANSFER:
        buf_idx = 0;
        break;
      case U8X8_MSG_BYTE_SEND:
        data = (uint8_t *)arg_ptr;      
        while( arg_int > 0 )
        {
            buffer[buf_idx++] = *data;
            data++;
            arg_int--;
        }     
        break; 
      case U8X8_MSG_BYTE_END_TRANSFER:
        HAL_I2C_Master_Transmit(&hi2c1, SSD1306_I2C_ADDRESS, buffer, buf_idx, STM32_HAL_I2C_TIMEOUT);
        break;
      case U8X8_MSG_BYTE_INIT:
      case U8X8_MSG_BYTE_SET_DC:
        break;
      default:
        return 0;
    }
    return 1;
}
#endif // USING_OLED

#ifdef USING_LCD

static uint8_t u8x8_gpio_and_delay_STM32F103(u8x8_t *u8x8, uint8_t msg, uint8_t arg_int, void *arg_ptr)
{
    switch(msg)
    {
      case U8X8_MSG_GPIO_AND_DELAY_INIT:	// called once during init phase of u8g2/u8x8
        break;							// can be used to setup pins
      case U8X8_MSG_DELAY_NANO:			// delay arg_int * 1 nano second
        DelayUs(1);
        break;    
      case U8X8_MSG_DELAY_100NANO:		// delay arg_int * 100 nano seconds
        DelayUs(1);
        break;
      case U8X8_MSG_DELAY_10MICRO:		// delay arg_int * 10 micro seconds
        DelayUs(arg_int * 10);
        break;
      case U8X8_MSG_DELAY_MILLI:			// delay arg_int * 1 milli second
        DelayMs(arg_int);
        break;
      case U8X8_MSG_DELAY_I2C:				// arg_int is the I2C speed in 100KHz, e.g. 4 = 400 KHz
        break;							// arg_int=1: delay by 5us, arg_int = 4: delay by 1.25us
      case U8X8_MSG_GPIO_I2C_CLOCK:		// arg_int=0: Output low at I2C clock pin
        break;							// arg_int=1: Input dir with pullup high for I2C clock pin
      case U8X8_MSG_GPIO_I2C_DATA:			// arg_int=0: Output low at I2C data pin
        break;							// arg_int=1: Input dir with pullup high for I2C data pin
      default:
        //        u8x8_SetGPIOResult(u8x8, 1);			// default return value
        break;
    }
    return 1;
}


uint8_t u8x8_byte_4wire_STM32F103(u8x8_t *u8x8, uint8_t msg, uint8_t arg_int, void *arg_ptr)
{
    uint8_t *data;
    uint8_t internal_spi_mode; 
    switch(msg) {
      case U8X8_MSG_BYTE_SEND:
        HAL_SPI_Transmit(&hspi1,arg_ptr,arg_int,100);
        break;
      case U8X8_MSG_BYTE_INIT:
        HAL_GPIO_WritePin(ChipSelect_Port, ChipSelect_Pin, HIGH);
        break;
      case U8X8_MSG_BYTE_SET_DC:
        HAL_GPIO_WritePin(RegisterSpiSelect_Port, RegisterSpiSelect_Pin, arg_int);
        break;
      case U8X8_MSG_BYTE_START_TRANSFER:
        HAL_GPIO_WritePin(ChipSelect_Port, ChipSelect_Pin, LOW);
        break;
      case U8X8_MSG_BYTE_END_TRANSFER:      
        HAL_GPIO_WritePin(ChipSelect_Port, ChipSelect_Pin, HIGH);
        break;
      default:
        return 0;
    }  
    return 1;
}


#endif // USING_LCD





#ifdef USING_OLED
//// Oled Init Function
void OledInit()
{
    u8g2_Setup_ssd1306_i2c_128x64_noname_f(&u8g, U8G2_R0, i2c_STM32F103_HAL, u8x8_gpio_and_delay_STM32F103);  // init u8g2 structure
    u8g2_InitDisplay(&u8g); // send init sequence to the display, display is in sleep mode after this,
    u8g2_SetPowerSave(&u8g, 0); // wake up display
}
#endif

#ifdef USING_LCD
void OledInit()
{
    u8g2_Setup_uc1610_ea_dogxl160_f(&u8g, U8G2_R0, u8x8_byte_4wire_STM32F103, u8x8_gpio_and_delay_STM32F103);  // init u8g2 structure
    u8g2_InitDisplay(&u8g); // send init sequence to the display, display is in sleep mode after this,
    u8g2_SetPowerSave(&u8g, 0); // wake up display
}
#endif


void DrawArrow(uint8_t x1, uint8_t y1, uint8_t x2, uint8_t y2, uint8_t Direction)
{
    switch(Direction)
    {
      case UP_DIRECTION:
        u8g2_DrawLine(&u8g, x1, y1, x1, y2);
        u8g2_DrawLine(&u8g, x1 - 3, y1 + 3, x1 + 3, y1 + 3);
        u8g2_DrawLine(&u8g, x1 - 2, y1 + 2, x1 + 2, y1 + 2);
        u8g2_DrawLine(&u8g, x1 - 1, y1 + 1, x1 + 1, y1 + 1);
        break;
      case DOWN_DIRECTION:
        u8g2_DrawLine(&u8g, x1, y1, x1, y2);
        u8g2_DrawLine(&u8g, x1 - 3, y1 + 3, x1 + 3, y1 + 3);
        u8g2_DrawLine(&u8g, x1 - 2, y1 + 4, x1 + 2, y1 + 4);
        u8g2_DrawLine(&u8g, x1 - 1, y1 + 5, x1 + 1, y1 + 5);
        break;
      case LEFT_DIRECTION:
        u8g2_DrawLine(&u8g, x1, y1, x2, y1);
        u8g2_DrawLine(&u8g, x1 + 3, y1 - 3, x1 + 3, y1 + 3);
        u8g2_DrawLine(&u8g, x1 + 4, y1 - 2, x1 + 4, y1 + 2);
        u8g2_DrawLine(&u8g, x1 + 5, y1 - 1, x1 + 5, y1 + 1);
        break;
      case RIGHT_DIRECTION:
        u8g2_DrawLine(&u8g, x1, y1, x2, y1);
        u8g2_DrawLine(&u8g, x1 + 1, y1 - 1, x1 + 1, y1 + 1);
        u8g2_DrawLine(&u8g, x1 + 2, y1 - 2, x1 + 2, y1 + 2);
        u8g2_DrawLine(&u8g, x1 + 3, y1 - 3, x1 + 3, y1 + 3);
        break;
      default:
        break;        
    }
    
}

void ClearScreen()
{
    u8g2_ClearBuffer(&u8g);
    u8g2_SendBuffer(&u8g);     
}

// Per disegnare il logo durante il boot
void DrawStartLogo(const char Logo[])
{
    u8g2_ClearBuffer(&u8g);
    u8g2_DrawXBMP(&u8g, START_LOGO_X_POS, START_LOGO_Y_POS, START_LOGO_LEN, START_LOGO_HIGH, Logo);
    u8g2_SendBuffer(&u8g); 
}



void MessageScreen(char *Message)
{
    u8g2_ClearBuffer(&u8g);
    u8g2_SetFont(&u8g, u8g2_font_7x13_tf);
    if(u8g2_GetStrWidth(&u8g,Message) > SCREEN_MAX_WIDTH)
    {
        u8g2_DrawStr(&u8g, X_CENTER_POS("Stringa non valida"), 32, "Stringa non valida"); 
    }
    else
    {
        u8g2_DrawStr(&u8g, X_CENTER_POS(Message), 32, Message);
    }
    u8g2_SendBuffer(&u8g);
    WDogOsDelay(POPUP_DELAY);
}

void PopUp(char *PopupTitle, char *Body1, char *Body2, char *Body3, char *Body4)
{
    u8g2_ClearBuffer(&u8g);
    
    
    u8g2_SetFont(&u8g, u8g2_font_7x13B_tf);
    u8g2_DrawStr(&u8g, X_CENTER_POS(PopupTitle), GENERAL_STR_Y_POS(3), PopupTitle);
    
    u8g2_SetFont(&u8g, u8g2_font_6x12_tf);
    u8g2_DrawStr(&u8g, X_CENTER_POS(Body1), GENERAL_STR_Y_POS(20) , Body1);
    u8g2_DrawStr(&u8g, X_CENTER_POS(Body2), GENERAL_STR_Y_POS(39) , Body2);
    u8g2_DrawStr(&u8g, X_CENTER_POS(Body3), GENERAL_STR_Y_POS(52) , Body3);
    u8g2_DrawStr(&u8g, X_CENTER_POS(Body4), GENERAL_STR_Y_POS(65) , Body4);
    
    //    u8g2_DrawFrame(&u8g, 0 ,0, 122, 62);
    //    u8g2_DrawFrame(&u8g, 1 ,1, 125, 61);
    u8g2_SendBuffer(&u8g);
}

void DrawMainScreenLoop()
{
    char *MainScreenStr = "Network Analyzer";
    u8g2_ClearBuffer(&u8g);
    DrawTopInfoBar();
    u8g2_SetFont(&u8g, u8g2_font_8x13B_tf);
    u8g2_DrawStr(&u8g, X_CENTER_POS(MainScreenStr), 32, MainScreenStr);
    DrawBottomBarInfo(MAIN_SCREEN_PAGE);
    u8g2_SendBuffer(&u8g);
    
}


void DrawTopInfoBar()
{
    static bool Toggle = false;
    char Time[6];
    char Date[9];
    snprintf(Time, 6, "%02d:%02d", GlobalTime.hours, GlobalTime.minutes);
    snprintf(Date, 9, "%02d/%02d/%02d", GlobalTime.day, GlobalTime.month, GlobalTime.year);
    u8g2_SetFontMode(&u8g, 1);
    u8g2_SetFont(&u8g, u8g_font_4x6);
    u8g2_SetDrawColor(&u8g, 2);
    u8g2_DrawStr(&u8g, X_LEFT_POS, TOP_INFO_BAR_Y_POS, Time);  
    u8g2_DrawStr(&u8g, X_RIGHT_POS(Date), TOP_INFO_BAR_Y_POS, Date);
    
    // Icona allarme attivo e controllato
    if(AlarmsActive())
    {
        u8g2_DrawXBMP(&u8g, ALARM_ICON_SML_X_POS, 0, 12, 6, AlarmIconSmall);
    }
    // Se sono in simulazione
    if(GeneralParams.EnableSimulation)
        u8g2_DrawStr(&u8g, SIMULATION_STR_POS, TOP_INFO_BAR_Y_POS, "Simul.");
    
    // Se il cronometro è attivo
    if(SetChrono)
    {   
        if(Toggle)
        {
            u8g2_DrawXBMP(&u8g, CRONO_ICON_X_POS, 0, 10, 6, CronoIcon);
            Toggle = false;
        }
    }
    // Icona batteria (ancora da gestire)
    u8g2_DrawXBMP(&u8g, BATTERY_ICON_SML_X_POS, 0, 19, 6, BatteryIcons[BATTERIA_1_4]);
    if(HalfSecondTick)
    {     
        HalfSecondTick = false;
        Toggle = true;
    }
}


void DrawBottomBarInfo(uint8_t WichPage)
{
    u8g2_SetFont(&u8g, u8g_font_4x6);
    switch(WichPage)
    {
      case MAIN_SCREEN_PAGE:        
        u8g2_SetDrawColor(&u8g, 2);
        u8g2_DrawStr(&u8g, X_CENTER_POS(BarItem[MENU_STR]), BOTTOM_INFO_BAR_Y_POS, BarItem[MENU_STR]);        
        break;
        
      case SETUP_PAGE:
        u8g2_SetDrawColor(&u8g, 2);
        u8g2_DrawStr(&u8g, X_LEFT_POS, BOTTOM_INFO_BAR_Y_POS, BarItem[BACK_STR]);   
        u8g2_DrawStr(&u8g, OK_STR_X_POS, BOTTOM_INFO_BAR_Y_POS, BarItem[OK_STR]); 
        u8g2_DrawStr(&u8g, UP_STR_X_POS, BOTTOM_INFO_BAR_Y_POS, BarItem[SU_STR]);
        u8g2_DrawStr(&u8g, DOWN_STR_X_POS+8, BOTTOM_INFO_BAR_Y_POS, BarItem[GIU_STR]);
        DrawArrow(LEFT_ARROW_X_POS, LEFT_RIGHT_ARROW_Y_POS, LEFT_ARROW_X_POS + 6, LEFT_RIGHT_ARROW_Y_POS, RIGHT_DIRECTION);
        DrawArrow(UP_ARROW_X_POS, UP_DOWN_ARROW_Y_POS, UP_ARROW_X_POS, UP_DOWN_ARROW_Y_POS + 6, UP_DIRECTION);
        DrawArrow(DOWN_ARROW_X_POS+8, UP_DOWN_ARROW_Y_POS, DOWN_ARROW_X_POS+8, UP_DOWN_ARROW_Y_POS + 6, DOWN_DIRECTION);
        DrawArrow(RIGHT_ARROW_X_POS, LEFT_RIGHT_ARROW_Y_POS, RIGHT_ARROW_X_POS + 6, LEFT_RIGHT_ARROW_Y_POS, LEFT_DIRECTION);
        break;
        
      case TIME_DATE_PAGE:
        u8g2_SetDrawColor(&u8g, 2);
        u8g2_DrawStr(&u8g, X_LEFT_POS, BOTTOM_INFO_BAR_Y_POS, BarItem[BACK_STR]);   
        u8g2_DrawStr(&u8g, BOXPOS_STR_X_POS, BOTTOM_INFO_BAR_Y_POS, BarItem[POS_STR]); 
        u8g2_DrawStr(&u8g, UP_STR_X_POS, BOTTOM_INFO_BAR_Y_POS, BarItem[SU_STR]);
        u8g2_DrawStr(&u8g, DOWN_STR_X_POS, BOTTOM_INFO_BAR_Y_POS, BarItem[GIU_STR]);
        DrawArrow(LEFT_ARROW_X_POS, LEFT_RIGHT_ARROW_Y_POS, LEFT_ARROW_X_POS + 6, LEFT_RIGHT_ARROW_Y_POS, RIGHT_DIRECTION);
        DrawArrow(UP_ARROW_X_POS, UP_DOWN_ARROW_Y_POS, UP_ARROW_X_POS, UP_DOWN_ARROW_Y_POS + 6, UP_DIRECTION);
        DrawArrow(DOWN_ARROW_X_POS, UP_DOWN_ARROW_Y_POS, DOWN_ARROW_X_POS, UP_DOWN_ARROW_Y_POS + 6, DOWN_DIRECTION);
        DrawArrow(RIGHT_ARROW_X_POS - 4, LEFT_RIGHT_ARROW_Y_POS, RIGHT_ARROW_X_POS + 2, LEFT_RIGHT_ARROW_Y_POS, LEFT_DIRECTION);
        break;
        
      case MEASURE_PAGE:
        u8g2_SetDrawColor(&u8g, 2);
        u8g2_DrawStr(&u8g, X_LEFT_POS, BOTTOM_INFO_BAR_Y_POS, BarItem[BACK_STR]);   
        u8g2_DrawStr(&u8g, BOXPOS_STR_X_POS - 2, BOTTOM_INFO_BAR_Y_POS, BarItem[PAGE_STR]); 
        DrawArrow(LEFT_ARROW_X_POS, LEFT_RIGHT_ARROW_Y_POS, LEFT_ARROW_X_POS + 6, LEFT_RIGHT_ARROW_Y_POS, RIGHT_DIRECTION);
        DrawArrow(RIGHT_ARROW_X_POS - 6, LEFT_RIGHT_ARROW_Y_POS, RIGHT_ARROW_X_POS + 2, LEFT_RIGHT_ARROW_Y_POS, LEFT_DIRECTION);
        break;   
        
      case CHANGE_VALUE_PAGE:
        u8g2_SetDrawColor(&u8g, 2);
        u8g2_DrawStr(&u8g, X_LEFT_POS, BOTTOM_INFO_BAR_Y_POS, BarItem[BACK_STR]);   
        u8g2_DrawStr(&u8g, BOXPOS_STR_X_POS, BOTTOM_INFO_BAR_Y_POS, BarItem[POS_STR]); 
        u8g2_DrawStr(&u8g, UP_STR_X_POS, BOTTOM_INFO_BAR_Y_POS, BarItem[SU_STR]);
        u8g2_DrawStr(&u8g, DOWN_STR_X_POS, BOTTOM_INFO_BAR_Y_POS, BarItem[GIU_STR]);
        DrawArrow(LEFT_ARROW_X_POS, LEFT_RIGHT_ARROW_Y_POS, LEFT_ARROW_X_POS + 6, LEFT_RIGHT_ARROW_Y_POS, RIGHT_DIRECTION);
        DrawArrow(UP_ARROW_X_POS, UP_DOWN_ARROW_Y_POS, UP_ARROW_X_POS, UP_DOWN_ARROW_Y_POS + 6, UP_DIRECTION);
        DrawArrow(DOWN_ARROW_X_POS, UP_DOWN_ARROW_Y_POS, DOWN_ARROW_X_POS, UP_DOWN_ARROW_Y_POS + 6, DOWN_DIRECTION);
        DrawArrow(RIGHT_ARROW_X_POS - 4, LEFT_RIGHT_ARROW_Y_POS, RIGHT_ARROW_X_POS + 2, LEFT_RIGHT_ARROW_Y_POS, LEFT_DIRECTION);
        break;
        
      case READ_ONLY_PARAM:
        u8g2_SetDrawColor(&u8g, 2);
        u8g2_DrawStr(&u8g, X_LEFT_POS, BOTTOM_INFO_BAR_Y_POS, BarItem[BACK_STR]);   
        DrawArrow(LEFT_ARROW_X_POS, LEFT_RIGHT_ARROW_Y_POS, LEFT_ARROW_X_POS + 6, LEFT_RIGHT_ARROW_Y_POS, RIGHT_DIRECTION);
        break;
      default:
        break;           
    }
}

void DrawTimeDateChangeLoop(uint8_t BoxPos, uint8_t TypeSetting,uint8_t BoxOneNum, uint8_t BoxTwoNum, uint8_t BoxThreeNum)
{
    uint8_t FontH = 0;
    uint8_t ItemIndx = 0;
    char TimeDateStr[9];
    if(TypeSetting == TIME_TYPE)
        snprintf(TimeDateStr, 6, "%02d:%02d", BoxOneNum, BoxTwoNum);
    else
        snprintf(TimeDateStr, 9, "%02d/%02d/%02d", BoxOneNum, BoxTwoNum, BoxThreeNum);
    
    u8g2_ClearBuffer(&u8g);
    DrawTopInfoBar();
    FontH = u8g2_GetFontAscent(&u8g) - u8g2_GetFontDescent(&u8g);
    u8g2_SetFont(&u8g, u8g_font_6x13);
    u8g2_SetDrawColor(&u8g, 2);
    u8g2_DrawStr(&u8g, X_CENTER_POS(TimeDateStr), TIME_DATE_SETUP_Y_POS + FontH, TimeDateStr);
    
    FontH = u8g2_GetFontAscent(&u8g);
    
    u8g2_DrawFrame(&u8g, (X_CENTER_POS(TimeDateStr) - 2 + (BoxPos * (TIME_DATE_BOX_WIDTH + 6))), TIME_DATE_SETUP_Y_POS - 5, 
                   (TIME_DATE_BOX_WIDTH + 2 ), FontH + 5);
    
    u8g2_SetFont(&u8g, u8g_font_4x6);
    u8g2_DrawStr(&u8g, X_CENTER_POS(BarItem[CONFIRM_OK]), 45, BarItem[CONFIRM_OK]);
    
    DrawBottomBarInfo(TIME_DATE_PAGE);
    u8g2_SendBuffer(&u8g);   
}

void ViewReadOnlyParam(uint32_t ValueToView, uint8_t ParamItem)
{
    char ReadOnlyStr[16];
    switch(ParamItem)
    {
      case ADC_OFFSET:
      case NUMBER_MEMORY_WRITES:       
        snprintf(ReadOnlyStr, 12, "%010d", ValueToView);
        u8g2_ClearBuffer(&u8g);
        DrawTopInfoBar();
        u8g2_SetFont(&u8g, u8g2_font_7x13_tf);
        u8g2_DrawStr(&u8g, X_CENTER_POS(ReadOnlyStr), GENERAL_STR_Y_POS(32), ReadOnlyStr);
        DrawBottomBarInfo(READ_ONLY_PARAM);
        u8g2_SendBuffer(&u8g);
        break;
      case POWER_ON_TIME:
        snprintf(ReadOnlyStr, 16, "%02dd %02dh %02dm %02ds", ActiveTime.day, ActiveTime.hours, ActiveTime.minutes, ActiveTime.seconds);
        u8g2_ClearBuffer(&u8g);
        DrawTopInfoBar();
        u8g2_SetFont(&u8g, u8g2_font_7x13_tf);
        u8g2_DrawStr(&u8g, X_CENTER_POS(ReadOnlyStr), GENERAL_STR_Y_POS(32), ReadOnlyStr);
        DrawBottomBarInfo(READ_ONLY_PARAM);
        u8g2_SendBuffer(&u8g);        
        break;
      default:
        break;
    }
}


void DrawChangeValueLoop(uint8_t BoxPos , uint8_t BoxValues[], const char *Title)
{
    uint8_t FontH = 0;
    uint8_t ItemIndx = 0;
    char ValueStr[6];    
    snprintf(ValueStr, 6, "%d%d%d%d%d", BoxValues[0], BoxValues[1], BoxValues[2], BoxValues[3], BoxValues[4]);
    
    
    u8g2_ClearBuffer(&u8g);
    DrawTopInfoBar();
    
    u8g2_SetFont(&u8g, u8g_font_6x13B);
    u8g2_DrawStr(&u8g, X_CENTER_POS(Title), MENU_TITLE_Y_POS, Title);
    
    
    u8g2_SetFont(&u8g, u8g_font_6x13);
    FontH = u8g2_GetFontAscent(&u8g) - u8g2_GetFontDescent(&u8g);
    u8g2_SetDrawColor(&u8g, 2);
    u8g2_DrawStr(&u8g, X_CENTER_POS(ValueStr), VALUE_BOX_Y_POS + FontH, ValueStr);
    
    FontH = u8g2_GetFontAscent(&u8g);
    
    u8g2_DrawFrame(&u8g, (X_CENTER_POS(ValueStr) - 2 + (BoxPos * VALUE_BOX_WIDTH)), VALUE_BOX_Y_POS , 
                   (VALUE_BOX_WIDTH + 2 ), FontH + 5);
    
    u8g2_SetFont(&u8g, u8g_font_4x6);
    u8g2_DrawStr(&u8g, X_CENTER_POS(BarItem[OK_LONG_STR]), 45, BarItem[OK_LONG_STR]);
    
    DrawBottomBarInfo(CHANGE_VALUE_PAGE);
    u8g2_SendBuffer(&u8g);   
}


void DrawChangeAlarmThrsOrFloatLoop(uint8_t BoxPos , char *StrValue, const char *Title, char FactorChar)
{
    uint8_t FontH = 0;
    char FakeChar;
    char *FactorStr = &FakeChar;
    snprintf(FactorStr, 2, "%c", FactorChar);
    
    u8g2_ClearBuffer(&u8g);
    DrawTopInfoBar();
    
    u8g2_SetFont(&u8g, u8g_font_6x13B);
    u8g2_DrawStr(&u8g, X_CENTER_POS(Title), MENU_TITLE_Y_POS, Title);
    
    
    u8g2_SetFont(&u8g, u8g_font_6x10);
    FontH = u8g2_GetFontAscent(&u8g) - u8g2_GetFontDescent(&u8g);
    u8g2_SetDrawColor(&u8g, 2);
    u8g2_DrawStr(&u8g, X_CENTER_POS(StrValue), VALUE_BOX_Y_POS + FontH, StrValue);
    u8g2_DrawStr(&u8g, 94, VALUE_BOX_Y_POS + FontH, FactorStr);
    
    FontH = u8g2_GetFontAscent(&u8g);
    
    if(BoxPos < 8)
        u8g2_DrawFrame(&u8g, (X_CENTER_POS(StrValue) - 2 + (BoxPos * VALUE_BOX_WIDTH)), VALUE_BOX_Y_POS , 
                       (VALUE_BOX_WIDTH + 2 ), FontH + 5);
    else
        u8g2_DrawFrame(&u8g, 94 - 2 , VALUE_BOX_Y_POS , 
                       (VALUE_BOX_WIDTH + 2 ), FontH + 5);
    
    u8g2_SetFont(&u8g, u8g_font_4x6);
    u8g2_DrawStr(&u8g, X_CENTER_POS(BarItem[OK_LONG_STR]), 45, BarItem[OK_LONG_STR]);
    
    DrawBottomBarInfo(CHANGE_VALUE_PAGE);
    u8g2_SendBuffer(&u8g);   
}

void ShowAlarmStatus(char *PageTitle, uint8_t AlarmItem)
{
    char AlarmTimeDateStr[20], NumberOccurence[3];
    u8g2_ClearBuffer(&u8g);
    DrawTopInfoBar();
    u8g2_SetFont(&u8g, u8g_font_6x13B);
    u8g2_DrawStr(&u8g, X_CENTER_POS(PageTitle), MENU_TITLE_Y_POS, PageTitle);  
    
    u8g2_SetFont(&u8g, u8g2_font_5x8_tf);
    
    WriteTimeDateOccurrenceAlarm(AlarmItem, AlarmTimeDateStr, NumberOccurence);
    u8g2_DrawStr(&u8g, X_CENTER_POS(AlarmMotivationStr[AlarmsControls[AlarmItem].AlarmMotivation]), GENERAL_STR_Y_POS(21) , AlarmMotivationStr[AlarmsControls[AlarmItem].AlarmMotivation]);
    u8g2_DrawStr(&u8g, X_CENTER_POS(AlarmTimeDateStr), GENERAL_STR_Y_POS(32) , AlarmTimeDateStr);
    u8g2_DrawStr(&u8g, X_CENTER_POS(NumberOccurence), GENERAL_STR_Y_POS(45) , NumberOccurence);
    
    DrawBottomBarInfo(READ_ONLY_PARAM);
    u8g2_SendBuffer(&u8g); 
}

void DrawChronometer()
{  
    char ChronoStrStr[16];
    u8g2_ClearBuffer(&u8g);
    DrawTopInfoBar();    
    u8g2_SetFont(&u8g, u8g2_font_6x10_tf);   
    snprintf(ChronoStrStr, 13, "%02d:%02d:%02d.%d%d%d", Crono.hours, Crono.minutes, Crono.seconds, Crono.dec, Crono.cent, Crono.millis);
    u8g2_DrawStr(&u8g, X_CENTER_POS(ChronoStrStr), GENERAL_STR_Y_POS(15), ChronoStrStr);
    u8g2_SetFont(&u8g, u8g_font_4x6);
    u8g2_DrawStr(&u8g, X_CENTER_POS("Ok per start/stop"), GENERAL_STR_Y_POS(25), "Ok per start/stop");
    u8g2_DrawStr(&u8g, X_CENTER_POS("Back per tornare"), GENERAL_STR_Y_POS(33), "Back per tornare");
    u8g2_DrawStr(&u8g, X_RIGHT_POS("Reset"), BOTTOM_INFO_BAR_Y_POS, "Reset");
    DrawBottomBarInfo(READ_ONLY_PARAM);
    u8g2_SendBuffer(&u8g); 
}

void DrawChangeEnumLoop(const char *PageTitle, ENUM_VALUE_ITEM EnumItem[], uint8_t ItemPos, uint8_t HighPosItem, uint8_t MaxMenuItemNum, uint8_t MaxMenuLines)
{
    uint8_t FontH = 0;
    uint8_t ItemIndx = 0;
    char NumElement[6];
    
    snprintf(NumElement, 6, "%d/%d", ItemPos + 1, MaxMenuItemNum);
    u8g2_ClearBuffer(&u8g);
    DrawTopInfoBar();
    // Disegna titolo menu
    u8g2_SetFont(&u8g, u8g_font_6x13B);
    u8g2_DrawStr(&u8g, X_CENTER_POS(PageTitle), MENU_TITLE_Y_POS, PageTitle);  
    
    // Disegna voci menu
    u8g2_SetFont(&u8g, u8g_font_6x10);
    FontH = u8g2_GetFontAscent(&u8g)-u8g2_GetFontDescent(&u8g);
    for(ItemIndx = 0; ItemIndx < MaxMenuLines; ItemIndx++)
    {
        uint8_t ListBuildPos = HighPosItem + ItemIndx;
        if(ListBuildPos >= MaxMenuItemNum)
            break;
        if (ItemPos == ListBuildPos) 
        {
            // Voce menu selezionata
            u8g2_DrawBox(&u8g, 0, (FIRST_MENU_LINES_Y_POS - (FontH - 1) + (ItemIndx * (FontH - 1)) + ((MENU_LINES_DELTA_Y + 1) * ItemIndx) ), SCREEN_MAX_WIDTH, FontH + 1);    
        }
        u8g2_DrawStr(&u8g, X_CENTER_POS(EnumItem[ListBuildPos].ItemName), FIRST_MENU_LINES_Y_POS + (ItemIndx * FontH) + (MENU_LINES_DELTA_Y * ItemIndx), EnumItem[ListBuildPos].ItemName);          
    }
    DrawBottomBarInfo(SETUP_PAGE);
    u8g2_DrawStr(&u8g, 54, BOTTOM_INFO_BAR_Y_POS, NumElement);
    u8g2_SendBuffer(&u8g);
}

void RefreshItemList(const char *PageTitle, const char *ListItem[], uint8_t ItemPos, uint8_t HighPosItem, uint8_t MaxListItemNum, uint8_t MaxMenuLines)
{
    uint8_t FontH = 0;
    uint8_t ItemIndx = 0;
    char NumElement[6];
    
    snprintf(NumElement, 6, "%d/%d", ItemPos + 1, MaxListItemNum);
    
    u8g2_ClearBuffer(&u8g);
    DrawTopInfoBar();
    // Disegna titolo menu
    u8g2_SetFont(&u8g, u8g_font_6x13B);
    FontH = u8g2_GetFontAscent(&u8g) - u8g2_GetFontDescent(&u8g);
    u8g2_SetDrawColor(&u8g, 2);
    u8g2_DrawStr(&u8g, X_CENTER_POS(PageTitle), MENU_TITLE_Y_POS, PageTitle);  
    
    // Disegna voci menu
    u8g2_SetFont(&u8g, u8g_font_6x10);
    FontH = u8g2_GetFontAscent(&u8g)-u8g2_GetFontDescent(&u8g);
    u8g2_SetDrawColor(&u8g, 2);
    for(ItemIndx = 0; ItemIndx < MaxMenuLines; ItemIndx++)
    {
        uint8_t ListBuildPos = HighPosItem + ItemIndx;
        if(ListBuildPos >= MaxListItemNum)
            break;
        if (ItemPos == ListBuildPos) 
        {
            // Voce menu selezionata
            u8g2_DrawBox(&u8g, 0, (FIRST_MENU_LINES_Y_POS - (FontH - 1) + (ItemIndx * (FontH - 1)) + ((MENU_LINES_DELTA_Y + 1) * ItemIndx) ), SCREEN_MAX_WIDTH, FontH + 1);    
        }
        u8g2_DrawStr(&u8g, X_CENTER_POS(ListItem[ListBuildPos]), FIRST_MENU_LINES_Y_POS + (ItemIndx * FontH) + (MENU_LINES_DELTA_Y * ItemIndx), ListItem[ListBuildPos]);          
    }
    DrawBottomBarInfo(SETUP_PAGE);
    // Scrittura elementi in lista
    u8g2_DrawStr(&u8g, 54, BOTTOM_INFO_BAR_Y_POS, NumElement);
    u8g2_SendBuffer(&u8g);
}


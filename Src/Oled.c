#include "main.h"
#include "SysTime.h"
#include "Oled.h"
#include "DisplayDef.h"
#include "TaskRTC.h"
#include "Menus.h"

#define	STM32_HAL_I2C_TIMEOUT	2000

extern I2C_HandleTypeDef hi2c1;

extern TIME_VAR GlobalTime;
extern DATE_VAR GlobalDate;

uint8_t control = 0;

u8g2_t u8g;

typedef enum
{
    MENU_STR = 0,
    SU_STR,
    GIU_STR,
    BACK_STR,
    OK_STR,
    POS_STR,
    PAGE_STR,
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
};

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

//// Oled Init Function
void OledInit()
{
    u8g2_Setup_ssd1306_i2c_128x64_noname_f(&u8g, U8G2_R0, i2c_STM32F103_HAL, u8x8_gpio_and_delay_STM32F103);  // init u8g2 structure
    u8g2_InitDisplay(&u8g); // send init sequence to the display, display is in sleep mode after this,
    u8g2_SetPowerSave(&u8g, 0); // wake up display
}

static const unsigned char u8g_Initial_Logo[] U8X8_PROGMEM =
{
    0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x01,0xfc,0x00,
    0x1f,0xc0,0x00,0x0e,0x03,0x80,0xe0,0x38,0x00,0x10,0x00,0x81,0x00,0x08,0x00,0x20,0xe0,0x42,0x03,0x84,0x00,
    0x20,0xf0,0x42,0x07,0x84,0x00,0x20,0xf0,0xc2,0x07,0x8c,0x00,0x10,0x60,0x81,0x03,0x08,0x00,0x0e,0x07,0x00,
    0xe0,0x70,0x00,0x01,0xf8,0x00,0x1f,0x80,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
    0x00,0x00,0x38,0x00,0x00,0x00,0x00,0x00,0x24,0x00,0x00,0x00,0x00,0x00,0xe3,0x80,0x00,0x00,0x00,0x01,0x00,
    0x80,0x00,0x00,0x00,0x00,0xe3,0x00,0x00,0x00,0x00,0x00,0x24,0x00,0x00,0x00,0x00,0x00,0x38,0x00,0x00,0x00,
    0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x3f,0xff,0xff,
    0xff,0xfc,0x00,0x10,0x20,0x20,0x81,0x08,0x00,0x08,0x20,0x20,0x81,0x30,0x00,0x04,0x20,0x20,0x81,0x20,0x00,
    0x02,0x20,0x20,0x81,0xe0,0x00,0x01,0x20,0x20,0x81,0x00,0x00,0x00,0xe0,0x20,0x83,0x00,0x00,0x00,0x20,0x20,
    0x84,0x00,0x00,0x00,0x18,0x20,0xb8,0x00,0x00,0x00,0x08,0x20,0xa0,0x00,0x00,0x00,0x06,0x20,0xe0,0x00,0x00,
    0x00,0x01,0xa3,0x00,0x00,0x00,0x00,0x00,0xfe,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
    0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
};

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

// Per disegnare il logo durante il boot
void DrawStartLogo()
{
    u8g2_ClearBuffer(&u8g);
    u8g2_DrawXBMP(&u8g, LOGO_X_POS, LOGO_Y_POS, LOGO_LEN, LOGO_HIGH, u8g_Initial_Logo);
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
    osDelay(POPUP_DELAY);
}


void DrawMainScreenLoop()
{
    u8g2_ClearBuffer(&u8g);
    DrawTopInfoBar();
    u8g2_SetFont(&u8g, u8g2_font_9x18B_tf);
    u8g2_DrawStr(&u8g, X_CENTER_POS("Welcome"), 32, "Welcome");
    DrawBottomBarInfo(MAIN_SCREEN_PAGE);
    u8g2_SendBuffer(&u8g);
    
}


void DrawTopInfoBar()
{
    uint8_t FontH = 0;
    char Time[9];
    char Date[9];
    snprintf(Time, 7, "%02d:%02d", GlobalTime.hours, GlobalTime.minutes);
    snprintf(Date, 9, "%02d/%02d/%02d", GlobalDate.day, GlobalDate.month, GlobalDate.year);
    u8g2_SetFontMode(&u8g, 1);
    u8g2_SetFont(&u8g, u8g_font_4x6);
    FontH = u8g2_GetFontAscent(&u8g)-u8g2_GetFontDescent(&u8g);
    u8g2_SetDrawColor(&u8g, 2);
    u8g2_DrawStr(&u8g, X_LEFT_POS, TOP_INFO_BAR_Y_POS, Time);  
    u8g2_DrawStr(&u8g, X_RIGHT_POS(Date), TOP_INFO_BAR_Y_POS, Date);
}


void DrawBottomBarInfo(uint8_t WichPage)
{
    uint8_t FontH = 0;
    
    switch(WichPage)
    {
      case MAIN_SCREEN_PAGE:
        u8g2_SetFont(&u8g, u8g_font_4x6);
        FontH = u8g2_GetFontAscent(&u8g)-u8g2_GetFontDescent(&u8g);
//        u8g2_DrawBox(&u8g, 0, FontH+1, SCREEN_MAX_WIDTH, FontH); 
        u8g2_SetDrawColor(&u8g, 2);
        u8g2_DrawStr(&u8g, X_CENTER_POS(BarItem[MENU_STR]), BOTTOM_INFO_BAR_Y_POS, BarItem[MENU_STR]);        
        break;
      case SETUP_PAGE:
        u8g2_SetFont(&u8g, u8g_font_4x6);
        FontH = u8g2_GetFontAscent(&u8g)-u8g2_GetFontDescent(&u8g);
//        u8g2_DrawBox(&u8g, X_LEFT_POS, FontH+1, SCREEN_MAX_WIDTH, FontH); 
        u8g2_SetDrawColor(&u8g, 2);
        u8g2_DrawStr(&u8g, X_LEFT_POS, BOTTOM_INFO_BAR_Y_POS, BarItem[BACK_STR]);   
        u8g2_DrawStr(&u8g, OK_STR_X_POS, BOTTOM_INFO_BAR_Y_POS, BarItem[OK_STR]); 
        u8g2_DrawStr(&u8g, UP_STR_X_POS, BOTTOM_INFO_BAR_Y_POS, BarItem[SU_STR]);
        u8g2_DrawStr(&u8g, DOWN_STR_X_POS, BOTTOM_INFO_BAR_Y_POS, BarItem[GIU_STR]);
        DrawArrow(LEFT_ARROW_X_POS, LEFT_RIGHT_ARROW_Y_POS, LEFT_ARROW_X_POS + 6, LEFT_RIGHT_ARROW_Y_POS, RIGHT_DIRECTION);
        DrawArrow(UP_ARROW_X_POS, UP_DOWN_ARROW_Y_POS, UP_ARROW_X_POS, UP_DOWN_ARROW_Y_POS + 6, UP_DIRECTION);
        DrawArrow(DOWN_ARROW_X_POS, UP_DOWN_ARROW_Y_POS, DOWN_ARROW_X_POS, UP_DOWN_ARROW_Y_POS + 6, DOWN_DIRECTION);
        DrawArrow(RIGHT_ARROW_X_POS, LEFT_RIGHT_ARROW_Y_POS, RIGHT_ARROW_X_POS + 6, LEFT_RIGHT_ARROW_Y_POS, LEFT_DIRECTION);
        break;
      case TIME_DATE_PAGE:
        u8g2_SetFont(&u8g, u8g_font_4x6);
        FontH = u8g2_GetFontAscent(&u8g)-u8g2_GetFontDescent(&u8g); 
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
        u8g2_SetFont(&u8g, u8g_font_4x6);
        FontH = u8g2_GetFontAscent(&u8g)-u8g2_GetFontDescent(&u8g); 
        u8g2_SetDrawColor(&u8g, 2);
        u8g2_DrawStr(&u8g, X_LEFT_POS, BOTTOM_INFO_BAR_Y_POS, BarItem[BACK_STR]);   
        u8g2_DrawStr(&u8g, BOXPOS_STR_X_POS - 2, BOTTOM_INFO_BAR_Y_POS, BarItem[PAGE_STR]); 
        DrawArrow(LEFT_ARROW_X_POS, LEFT_RIGHT_ARROW_Y_POS, LEFT_ARROW_X_POS + 6, LEFT_RIGHT_ARROW_Y_POS, RIGHT_DIRECTION);
        DrawArrow(RIGHT_ARROW_X_POS - 6, LEFT_RIGHT_ARROW_Y_POS, RIGHT_ARROW_X_POS + 2, LEFT_RIGHT_ARROW_Y_POS, LEFT_DIRECTION);
        break;        
      case CHANGE_VALUE_PAGE:
        u8g2_SetFont(&u8g, u8g_font_4x6);
        FontH = u8g2_GetFontAscent(&u8g)-u8g2_GetFontDescent(&u8g); 
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
    u8g2_DrawStr(&u8g, X_CENTER_POS("Premere ok per confermare"), 45, "Premere ok per confermare");
    
    DrawBottomBarInfo(TIME_DATE_PAGE);
    u8g2_SendBuffer(&u8g);   
}

void DrawChangeValueLoop(uint8_t BoxPos , uint8_t BoxValues[])
{
    uint8_t FontH = 0;
    uint8_t ItemIndx = 0;
    char ValueStr[6];    
    snprintf(ValueStr, 6, "%d%d%d%d%d", BoxValues[0], BoxValues[1], BoxValues[2], BoxValues[3], BoxValues[4]);
    
    
    u8g2_ClearBuffer(&u8g);
    DrawTopInfoBar();
    FontH = u8g2_GetFontAscent(&u8g) - u8g2_GetFontDescent(&u8g);
    u8g2_SetFont(&u8g, u8g_font_6x13);
    u8g2_SetDrawColor(&u8g, 2);
    u8g2_DrawStr(&u8g, X_CENTER_POS(ValueStr), VALUE_BOX_Y_POS + FontH, ValueStr);
    
    FontH = u8g2_GetFontAscent(&u8g);
    
    u8g2_DrawFrame(&u8g, (X_CENTER_POS(ValueStr) - 2 + (BoxPos * VALUE_BOX_WIDTH)), VALUE_BOX_Y_POS - 5, 
                   (VALUE_BOX_WIDTH + 2 ), FontH + 5);
    
    u8g2_SetFont(&u8g, u8g_font_4x6);
    u8g2_DrawStr(&u8g, X_CENTER_POS("Premere ok per confermare"), 45, "Premere ok per confermare");
    
    DrawBottomBarInfo(CHANGE_VALUE_PAGE);
    u8g2_SendBuffer(&u8g);   
}


void DrawMenuLoop(char *PageTitle, MENU_ITEM MenuItem[], uint8_t ItemPos, uint8_t HighPosItem, uint8_t MaxMenuItemNum, uint8_t MaxMenuLines)
{
    uint8_t FontH = 0;
    uint8_t ItemIndx = 0;
    
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
        if(ListBuildPos >= MaxMenuItemNum)
            break;
        if (ItemPos == ListBuildPos) 
        {
            // Voce menu selezionata
            u8g2_DrawBox(&u8g, 0, (FIRST_MENU_LINES_Y_POS - (FontH - 1) + (ItemIndx * (FontH - 1)) + ((MENU_LINES_DELTA_Y + 1) * ItemIndx) ), SCREEN_MAX_WIDTH, FontH + 1);    
        }
        u8g2_DrawStr(&u8g, X_CENTER_POS(MenuItem[ListBuildPos].ItemTitle), FIRST_MENU_LINES_Y_POS + (ItemIndx * FontH) + (MENU_LINES_DELTA_Y * ItemIndx), MenuItem[ListBuildPos].ItemTitle);          
    }
    DrawBottomBarInfo(SETUP_PAGE);
    u8g2_SendBuffer(&u8g);
}

void DrawParamLoop(char *PageTitle, PARAMETER_ITEM MenuItem[], uint8_t ItemPos, uint8_t HighPosItem, uint8_t MaxMenuItemNum, uint8_t MaxMenuLines)
{
    uint8_t FontH = 0;
    uint8_t ItemIndx = 0;
    
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
        if(ListBuildPos >= MaxMenuItemNum)
            break;
        if (ItemPos == ListBuildPos) 
        {
            // Voce menu selezionata
            u8g2_DrawBox(&u8g, 0, (FIRST_MENU_LINES_Y_POS - (FontH - 1) + (ItemIndx * (FontH - 1)) + ((MENU_LINES_DELTA_Y + 1) * ItemIndx) ), SCREEN_MAX_WIDTH, FontH + 1);    
        }
        u8g2_DrawStr(&u8g, X_CENTER_POS(MenuItem[ListBuildPos].ItemTitle), FIRST_MENU_LINES_Y_POS + (ItemIndx * FontH) + (MENU_LINES_DELTA_Y * ItemIndx), MenuItem[ListBuildPos].ItemTitle);          
    }
    DrawBottomBarInfo(SETUP_PAGE);
    u8g2_SendBuffer(&u8g);
}

void DrawListLoop(char *PageTitle, const char *ListItem[], uint8_t ItemPos, uint8_t HighPosItem, uint8_t MaxListItemNum, uint8_t MaxMenuLines)
{
    uint8_t FontH = 0;
    uint8_t ItemIndx = 0;
    
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
    u8g2_SendBuffer(&u8g);
}


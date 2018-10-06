#ifndef OLED_H
#define OLED_H

#include "OLEDLIB2/u8g2.h"

#include "Menus.h"
#include "Parameters.h"

#define SSD1306_I2C_ADDRESS 0x78



typedef enum
{
    UP_DIRECTION = 0,
    DOWN_DIRECTION,
    LEFT_DIRECTION,
    RIGHT_DIRECTION
}ARROW_DIRECTIONS;

typedef enum
{
    MAIN_SCREEN_PAGE = 0,
    SETUP_PAGE,
    TIME_DATE_PAGE,
    MEASURE_PAGE,
    CHANGE_VALUE_PAGE,
    MAX_SCREEN_PAGES
}SCREEN_PAGES;


enum
{
    TIME_TYPE = 0,
    DATE_TYPE
};

void OledInit(void);
void DrawStartLogo(const char Logo[]);
void DrawMainLogo(const char Logo[]);
void MessageScreen(char *Message);
void DrawArrow(uint8_t x1, uint8_t y1, uint8_t x2, uint8_t y2, uint8_t Direction);
void DrawTopInfoBar(void);
void DrawBottomBarInfo(uint8_t WichPage);
void DrawMainScreenLoop(void);
void DrawMenuLoop(char *PageTitle, MENU_ITEM MenuItem[], uint8_t ItemPos, uint8_t HighPosItem, uint8_t MaxMenuItemNum, uint8_t MaxMenuLines);
void DrawParamLoop(char *PageTitle, PARAMETER_ITEM MenuItem[], uint8_t ItemPos, uint8_t HighPosItem, uint8_t MaxMenuItemNum, uint8_t MaxMenuLines);
void DrawTimeDateChangeLoop(uint8_t BoxPos, uint8_t TypeSetting,uint8_t BoxOneNum, uint8_t BoxTwoNum, uint8_t BoxThreeNum);
void DrawListLoop(char *PageTitle, const char *ListItem[], uint8_t ItemPos, uint8_t HighPosItem, uint8_t MaxListItemNum, uint8_t MaxMenuLines);
void DrawChangeValueLoop(uint8_t BoxPos , uint8_t BoxValues[], char *Title);

#endif
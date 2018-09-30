#ifndef DISPLAY_DEF_H
#define DISPLAY_DEF_H

#define SCREEN_MAX_WIDTH    u8g2_GetDisplayWidth(&u8g)

#define LOGO_X_POS      0
#define LOGO_Y_POS      0
#define LOGO_LEN        8
#define LOGO_HIGH       8

#define TOP_INFO_BAR_Y_POS      (0 + (u8g2_GetAscent(&u8g)))
#define BOTTOM_INFO_BAR_Y_POS   (64 - (u8g2_GetAscent(&u8g)))
#define UP_STR_X_POS            32
#define DOWN_STR_X_POS          96

#define LEFT_ARROW_X_POS        18
#define RIGHT_ARROW_X_POS      118
#define UP_ARROW_X_POS          42
#define DOWN_ARROW_X_POS        88

#define LEFT_RIGHT_ARROW_Y_POS  60
#define UP_DOWN_ARROW_Y_POS     57

 
#define MENU_TITLE_Y_POS         8
#define FIRST_MENU_LINES_Y_POS  22
#define MENU_LINES_DELTA_Y       2

#define TIME_DATE_SETUP_Y_POS   20
#define TIME_DATE_FONT_WIDTH     6
#define TIME_DATE_BOX_WIDTH     12

// Posizione x centrale dello schermo
#define X_LEFT_POS               0
#define X_RIGHT_POS(Str)      (SCREEN_MAX_WIDTH - u8g2_GetStrWidth(&u8g,Str))       
#define X_CENTER_POS(Str)     ((SCREEN_MAX_WIDTH - u8g2_GetStrWidth(&u8g,Str)) / 2)

#endif
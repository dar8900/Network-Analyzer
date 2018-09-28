#ifndef TASK_KEYBOARD_H
#define TASK_KEYBOARD_H

#define LONG_PRESS_TIME     300

typedef enum
{
    BUTTON_UP = 0,
    BUTTON_DOWN,
    BUTTON_LEFT,
    BUTTON_RIGHT,
    BUTTON_OK,
    BUTTON_UP_DOWN,
    BUTTON_LEFT_RIGHT,
    BUTTON_UP_LEFT,
    BUTTON_UP_RIGHT,
    BUTTON_DOWN_LEFT,
    BUTTON_DOWN_RIGHT,
    BUTTON_UP_LONG,
    BUTTON_DOWN_LONG,
    BUTTON_LEFT_LONG,
    BUTTON_RIGHT_LONG,
    BUTTON_OK_LONG,
    NO_PRESS
}KEY_PRESSED;

typedef struct 
{
    bool PressedYet : 1;
    
}FLAG_KEYBOARD;

extern uint8_t LastButtonPressed;


void TaskKeyboard(void const * argument);
#endif
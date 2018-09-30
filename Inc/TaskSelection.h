#ifndef TASK_SELECTION_H
#define TASK_SELECTION_H

#undef UNSELECT_ALL

#ifndef UNSELECT_ALL

#undef ENABLE_MEASURE    // Abilita il TaskMeasure

#define ENABLE_KEYBOARD   // Abilita il TaskKeyboard

#define ENABLE_OLED       // Abilita il TaskOled

#define ENABLE_RTC        // Abilita il TaskRTC

#define ENABLE_LED        // Abilita il TaskLed

#endif // UNSELECT_ALL

#endif
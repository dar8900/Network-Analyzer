#include "main.h"
#include "Menus.h"
#include "Parameters.h"
#include "StringsAnalyzer.h"
#include "AlarmMachine.h"

#ifdef ITALIANO

const char *BarItem[] = 
{
    "Menu",
    "Su",
    "Giu",
    "Prec.",
    "Ok",   
    "Pos.",
    "Pag.",
    "Premere Ok per confermare",
    "Premere Prec. per uscire",
};

const char *MainSetupMenuStr[MAX_SETUP_ITEM] = 
{
    "Grafici"            ,
    "Misure"             ,
    "Stato allarmi"      ,
    "Imposta allarmi"    ,
    "Setup generale"     ,
    "Cronometro"         ,
    "Imposta orario"     ,
    "Reset"              ,
};

const char *TimeSettingStr[MAX_TIME_DATE_ITEM] = 
{
    "Cambia orario",
    "Cambia data"  ,
};

const  char *GraphicsMenuStr[MAX_GRAPHIC_ITEM] = 
{
    "Forma d'onda I"     ,
};

const char *ResetList[MAX_RESET_ITEM] = 
{
    "Reset energia",
    "Reset max/min",
    "Riavvia sistema",
    "Reset a default",
};

const char *ParametersMenuStr[MAX_PARAMETER_ITEM] = 
{
    "Abilitare misura"       ,
    "Abilitare simulaz."     ,
    "Corrente sim.(A)"       ,
    "Frequenza sim.(Hz)"     ,
    "Tensione misura (V)"    ,
    "Abilitare log"          ,
    "Periodo log mis.(s)"    ,
    "ADC Offset"             ,
    "Usare screensaver"      ,
    "Tipo screensaver"       ,
    "Timer screensaver(s)"   ,
    "Configura i led "       ,
    "Tempo in attività"      ,
    "Scritture in memoria"   ,
};

const char *AlarmThrMenuStr[MAX_ALARM_SETUP_ITEM] = 
{
    "Soglie corrente "  ,
    "Soglie potenza  "  ,
    "Soglia energia  "  ,
};

const char *AlarmMotivationStr[3] = 
{
    "Sovra-soglia",
    "Sotto-soglia",
    "Nessun allarme",
};


const char *AlarmList[MAX_ALARM_STR] = 
{
    "Allarme corrente",
    "Allarme potenza",
    "Allarme energia",
};
#endif

#ifdef INGLESE

const char *BarItem[] = 
{
    "Menu",
    "Up",
    "Down",
    "Back",
    "Ok",   
    "Pos.",
    "Pag.",
    "Press Ok to confirm",
    "Press Back to exit",
};

const char *MainSetupMenuStr[MAX_SETUP_ITEM] = 
{
    "Graphics"            ,
    "Measure"             ,
    "Alarm status"      ,
    "Set alarms"    ,
    "General setup"     ,
    "Cronometer"         ,
    "Set time"     ,
    "Reset"              ,
};

const char *TimeSettingStr[MAX_TIME_DATE_ITEM] = 
{
    "Change time",
    "Change date"  ,
};

const  char *GraphicsMenuStr[MAX_GRAPHIC_ITEM] = 
{
    "I waveform"     ,
};

const char *ResetList[MAX_RESET_ITEM] = 
{
    "Reset energy",
    "Reset max/min",
    "System restart",
    "Reset to default",
};

const char *ParametersMenuStr[MAX_PARAMETER_ITEM] = 
{
    "Enable measure"       ,
    "Enable simul."     ,
    "Current sim.(A)"       ,
    "Frequency sim.(Hz)"     ,
    "Voltage measure (V)"    ,
    "Enable log"          ,
    "Log mis. period(s)"    ,
    "ADC Offset"             ,
    "Enable screensaver"      ,
    "Screensaver type"       ,
    "Screensaver timer(s)"   ,
    "Led configuration"       ,
    "Power on time"      ,
    "Memory writes num."   ,
};

const char *AlarmThrMenuStr[MAX_ALARM_SETUP_ITEM] = 
{
    "Current thr."  ,
    "Power thr."  ,
    "Energy thr."  ,
};

const char *AlarmMotivationStr[3] = 
{
    "Over thr.",
    "Under thr.",
    "No alarm",
};


const char *AlarmList[MAX_ALARM_STR] = 
{
    "Current alarm",
    "Power alarm",
    "Energy alarm",
};

#endif

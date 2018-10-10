#include "main.h"
#include "AlarmMachine.h"
#include "TaskMeasure.h"
#include "TaskRTC.h"

extern MEASURES GeneralMeasures;
extern TIME_VAR GlobalTime;
extern DATE_VAR GlobalDate;

char *AlarmMotivationStr[2] = 
{
    "Sovra-soglia",
    "Sotto-soglia",
};


char *AlarmList[MAX_ALARM_STR] = 
{
    "Allarme corrente",
    "Allarme potenza",
    "Allarme energia",
};

ALARM_PARAMS AlarmsParameters[MAX_ALARM_NUMBER] = 
{
    {&GeneralMeasures.MeanCurrentRMS, 10.0            , -1.0           },
    {&GeneralMeasures.Power         , 4000.0          , -1.0           },
    {&GeneralMeasures.MeanEnergy    , 200000.0        , -1.0           },
    
};
ALARM_CONTROLS AlarmsControls[MAX_ALARM_NUMBER];




void CheckAlarm()
{
    uint8_t AlarmIndex = 0;
    for(AlarmIndex = 0; AlarmIndex < MAX_ALARM_NUMBER; AlarmIndex++)
    {
        if(*AlarmsParameters[AlarmIndex].AlarmMeasure < AlarmsParameters[AlarmIndex].UnderThreshold || *AlarmsParameters[AlarmIndex].AlarmMeasure > AlarmsParameters[AlarmIndex].OverThreshold)
        {
            if(!AlarmsControls[AlarmIndex].Active)
            {
                AlarmsControls[AlarmIndex].NumbOccurrence++;
                AlarmsControls[AlarmIndex].AlarmActivationTime = GlobalTime;
                AlarmsControls[AlarmIndex].AlarmActivationDate = GlobalDate;
                AlarmsControls[AlarmIndex].Cheked = false;
                AlarmsControls[AlarmIndex].RePorted = true;
                
            }
            
            AlarmsControls[AlarmIndex].Active = true;            
            if(*AlarmsParameters[AlarmIndex].AlarmMeasure < AlarmsParameters[AlarmIndex].UnderThreshold)
            {
                AlarmsControls[AlarmIndex].AlarmMotivation = UNDER_THR;
            }
            else if(*AlarmsParameters[AlarmIndex].AlarmMeasure > AlarmsParameters[AlarmIndex].OverThreshold)
            {
                AlarmsControls[AlarmIndex].AlarmMotivation = OVER_THR; 
            }

        }
        else
        {
            AlarmsControls[AlarmIndex].Active = false;
            AlarmsControls[AlarmIndex].RePorted = false;
            AlarmsControls[AlarmIndex].Cheked = true;
            AlarmsControls[AlarmIndex].AlarmMotivation = NO_ALARM;
            AlarmsControls[AlarmIndex].AlarmActivationTime.hours = 0;
            AlarmsControls[AlarmIndex].AlarmActivationTime.minutes = 0;
            AlarmsControls[AlarmIndex].AlarmActivationTime.seconds = 0;
            AlarmsControls[AlarmIndex].AlarmActivationDate.day = 0;
            AlarmsControls[AlarmIndex].AlarmActivationDate.month = 0;
            AlarmsControls[AlarmIndex].AlarmActivationDate.year = 0;
        }
        if(AlarmsControls[AlarmIndex].Active && AlarmsControls[AlarmIndex].Cheked && AlarmsControls[AlarmIndex].RePorted)
            AlarmsControls[AlarmIndex].RePorted = false;
        
    }

}

bool AlarmsReported()
{
    uint8_t AlarmIndx = 0;
    for(AlarmIndx = 0; AlarmIndx < MAX_ALARM_NUMBER; AlarmIndx++)
    {
        if(AlarmsControls[AlarmIndx].RePorted)
            return true;
    }
    return false;
}

bool AlarmsActive()
{
    uint8_t AlarmIndx = 0;
    for(AlarmIndx = 0; AlarmIndx < MAX_ALARM_NUMBER; AlarmIndx++)
    {
        if(AlarmsControls[AlarmIndx].Active)
            return true;
    }
    return false;
}

bool AlarmsChecked()
{
    uint8_t AlarmIndx = 0;
    for(AlarmIndx = 0; AlarmIndx < MAX_ALARM_NUMBER; AlarmIndx++)
    {
        if(!AlarmsControls[AlarmIndx].Cheked)
            return false;
    }
    return true;
}

uint8_t LastReportedAlarm()
{
    uint8_t AlarmIndx = 0;
    for(AlarmIndx = MAX_ALARM_NUMBER - 1; AlarmIndx >= 0; AlarmIndx--)
    {
        if(AlarmsControls[AlarmIndx].RePorted)
            return AlarmIndx;
    }
    return MAX_ALARM_NUMBER;
}
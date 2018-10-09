#ifndef TASK_MEASURE_H
#define TASK_MEASURE_H

#define ADC_MAX_VALUE               4095
#define ADC_HALF_MAX_VALUE          2048



#define NUM_TEST_SAMPLE       126

#define PIGRECO               3.141592653
#define TO_GRADES(Angle)     (Angle * (180.0 / PIGRECO))
#define TO_RADIANTS(Angle)   (Angle * (PIGRECO / 180.0))

#define VOLTAGE_VALUE         220.0


#define TOVOLT(Value)        (Value*(3.3/4096.0))
#define CURR_SENSOR_SENSITIVITY  185.0

typedef struct
{
    float MeanCurrentRMS;
    float Power;
    float MeanEnergy;
}MEASURES;

extern MEASURES GeneralMeasures;

void TaskMeasure(void const * argument);
#endif
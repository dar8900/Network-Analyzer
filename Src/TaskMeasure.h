#ifndef TASK_MEASURE_H
#define TASK_MEASURE_H

#define ADC_MAX_VALUE               4095
#define ADC_HALF_MAX_VALUE          2048



#define NUM_TEST_SAMPLE       126

#define PIGRECO               3.141592653
#define TO_GRADES(Angle)     (Angle * (180.0 / PIGRECO))
#define TO_RADIANTS(Angle)   (Angle * (PIGRECO / 180.0))

#define VOLTAGE_VALUE_DFLT    220


#define TOVOLT(Value)            (Value*(3.3/4096.0))
#define CURR_SENSOR_CAPACITY     5  
#define CURR_SENSOR_SENSITIVITY  (((3300*(CURR_SENSOR_CAPACITY * 185))/5000)/CURR_SENSOR_CAPACITY) // real 185.0 

typedef struct
{
    float MeanCurrentRMS;
    float Power;
    float MeanEnergy;
    float MaxCurrent;
    float MinCurrent;
    float MaxPower;
    float MinPower;
}MEASURES;

extern MEASURES GeneralMeasures;

void TaskMeasure(void const * argument);
#endif
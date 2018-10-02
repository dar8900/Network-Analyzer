#ifndef TASK_MEASURE_H
#define TASK_MEASURE_H

#define NUM_TEST_SAMPLE       126

#define PIGRECO               3.141592653
#define TO_GRADES(Angle)     (Angle * (180.0 / PIGRECO))
#define TO_RADIANTS(Angle)   (Angle * (PIGRECO / 180.0))

#define VOLTAGE_VALUE         220.0

void TaskMeasure(void const * argument);
#endif
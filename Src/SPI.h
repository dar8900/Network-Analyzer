#ifndef SPI_H
#define SPI_H

#include "main.h"

extern SPI_HandleTypeDef hspi1;


extern void _Error_Handler(char *, int);

void MX_SPI1_Init(void);


#endif
#ifndef __PWM_H
#define __PWM_H
#include "sys.h"
//#include "stm32f4xx.h"

void TIM8_PWM_Init(uint32_t arr,uint32_t psc);

u16 anglee(float ang);

#endif

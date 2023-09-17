#ifndef __CAN_H
#define __CAN_H

#include "sys.h"
#include "PID.h"

#define speed 0
#define position 1

void CAN1_mode_init(uint8_t tsjw,uint8_t tbs2,uint8_t tbs1,uint16_t brp);
void CAN1_RX0_IRQHandler(void);
void CAN1_CMD_TRANSMIT(PidTypeDef *pid_speed,PidTypeDef *pid_ecd, float set_motor_speed,float set_motor_ecd,uint16_t now_mode);
void PID_OVER_ZERO(float *set,float *get);

typedef struct
{
    int16_t ecd;
    int16_t speed_rpm;
    int16_t given_current;
    uint8_t temperate;
    int16_t last_ecd;
	int16_t round;
} motor_measure_t;


#endif


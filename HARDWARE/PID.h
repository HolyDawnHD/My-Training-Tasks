#ifndef __PID_H
#define __PID_H

#include "sys.h"

typedef float fp32;


enum PID_MODE
{
    PID_POSITION = 0,
    PID_DELTA
};



typedef struct
{
    uint8_t mode;
    //PID ������
    fp32 Kp;
    fp32 Ki;
    fp32 Kd;

    fp32 max_out;  //������
    fp32 max_iout; //���������

    fp32 set;//�趨ת��
    fp32 now_speed;//��ǰת��

    fp32 out;
    fp32 Pout;
    fp32 Iout;
    fp32 Dout;
    fp32 Dbuf[3];  //΢���� 0���� 1��һ�� 2���ϴ�
    fp32 error[3]; //����� 0���� 1��һ�� 2���ϴ�

} PidTypeDef;


void PID_Init(PidTypeDef *pid, uint8_t mode, const fp32 kp,const fp32 ki,const fp32 kd, fp32 max_out, fp32 max_iout);
extern fp32 PID_Calc(PidTypeDef *pid, fp32 ref, fp32 set);
extern void PID_clear(PidTypeDef *pid);



#endif

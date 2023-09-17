#include "CAN.h"
#include "sys.h"
#include "PID.h"
#include "usart.h"


motor_measure_t motor;
static CanTxMsg TxMessage;




#define CAN_MOTOR_ID 0x205

const motor_measure_t *Motor_Measure_Point(void)
{
    return &motor;
}

void CAN1_mode_init(u8 tsjw,u8 tbs2,u8 tbs1,u16 brp)
{
	GPIO_InitTypeDef GPIO_InitStructure;
    CAN_InitTypeDef CAN_InitStructure;
    CAN_FilterInitTypeDef CAN_FilterInitStructure;
    NVIC_InitTypeDef NVIC_InitStructure;
	
	//gpiob 8 9 can1 rx tx
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOB, ENABLE);
	
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_CAN1, ENABLE);
	
//	RCC_APB1PeriphResetCmd(RCC_APB1Periph_CAN1, ENABLE);//复位使相应寄存器为0,
//    RCC_APB1PeriphResetCmd(RCC_APB1Periph_CAN1, DISABLE);//取消复位，使寄存器能够正常使用
	
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_8 | GPIO_Pin_9;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
    GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;
    GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
    GPIO_Init(GPIOB, &GPIO_InitStructure);
	
	GPIO_PinAFConfig(GPIOB, GPIO_PinSource8, GPIO_AF_CAN1);
    GPIO_PinAFConfig(GPIOB, GPIO_PinSource9, GPIO_AF_CAN1);
	
	CAN_InitStructure.CAN_TTCM = DISABLE;
    CAN_InitStructure.CAN_ABOM = DISABLE;
    CAN_InitStructure.CAN_AWUM = DISABLE;
    CAN_InitStructure.CAN_NART = ENABLE;
    CAN_InitStructure.CAN_RFLM = DISABLE;
    CAN_InitStructure.CAN_TXFP = DISABLE;
    CAN_InitStructure.CAN_Mode = CAN_Mode_Normal;
    CAN_InitStructure.CAN_SJW = tsjw;
    CAN_InitStructure.CAN_BS1 = tbs1;
    CAN_InitStructure.CAN_BS2 = tbs2;
    CAN_InitStructure.CAN_Prescaler = brp;
    CAN_Init(CAN1, &CAN_InitStructure);
	
	CAN_FilterInitStructure.CAN_FilterNumber = 0;
    CAN_FilterInitStructure.CAN_FilterMode = CAN_FilterMode_IdMask;
    CAN_FilterInitStructure.CAN_FilterScale = CAN_FilterScale_32bit;
    CAN_FilterInitStructure.CAN_FilterIdHigh = 0x0000;
    CAN_FilterInitStructure.CAN_FilterIdLow = 0x0000;
    CAN_FilterInitStructure.CAN_FilterMaskIdHigh = 0x0000;
    CAN_FilterInitStructure.CAN_FilterMaskIdLow = 0x0000;
    CAN_FilterInitStructure.CAN_FilterFIFOAssignment = CAN_Filter_FIFO0;
    CAN_FilterInitStructure.CAN_FilterActivation = ENABLE;
    CAN_FilterInit(&CAN_FilterInitStructure);
	
	CAN_ITConfig(CAN1, CAN_IT_FMP0, ENABLE);

    NVIC_InitStructure.NVIC_IRQChannel = CAN1_RX0_IRQn;
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&NVIC_InitStructure);
	
	
}


#define get_motor_measure(ptr, rx_message)                                                     \
{                                                                                              \
        (ptr)->last_ecd = (ptr)->ecd;                                                          \
        (ptr)->ecd = (uint16_t)((rx_message)->Data[0] << 8 | (rx_message)->Data[1]);           \
        (ptr)->speed_rpm = (uint16_t)((rx_message)->Data[2] << 8 | (rx_message)->Data[3]);     \
        (ptr)->given_current = (uint16_t)((rx_message)->Data[4] << 8 | (rx_message)->Data[5]); \
        (ptr)->temperate = (rx_message)->Data[6];                                              \
}



void CAN1_RX0_IRQHandler(void)
{
    static CanRxMsg rx1_message;

//    if (CAN_GetITStatus(CAN1, CAN_IT_FMP0) != RESET)
//    {
        CAN_ClearITPendingBit(CAN1, CAN_IT_FMP0);
        CAN_Receive(CAN1, CAN_FIFO0, &rx1_message);
		get_motor_measure(&motor, &rx1_message);
//    }
}


u16 flag=0;
float yaw0;
int16_t yaw1,yaw;
fp32 get_motor_ecd_degree;


//这是恒定转速到指定位置的方案1
void PID_OVER_ZERO (float *set,float *get)
{
	if(*set - *get > 180.0f)
	{
		*get += 360.0f;
		
	}
	else if (*set - *get < -180.0f)
	{
		*get -= 360.0f;
	}
	
	
}


//void PID_OVER_ZERO (float *set,float *get)
//{
//	if(motor.ecd-motor.last_ecd<-4096)
//	{
//		*get += 360.0f;
//		
//	}
//	else if (motor.ecd-motor.last_ecd>4096)
//	{
//		*get -= 360.0f;
//	}
//	
//	
//}

float real_ecd_degree;
void CAN1_CMD_TRANSMIT(PidTypeDef *pid_speed,PidTypeDef *pid_ecd, float set_motor_speed,float set_motor_ecd,u16 now_mode)
{
	
    TxMessage.StdId =0x200 ; //0x1FF  0x200
	
    TxMessage.IDE = 0x00000000;
    TxMessage.RTR = 0x00000000;
    TxMessage.DLC = 0x08;
	
	if(now_mode==speed)
	{
		yaw=(int32_t)PID_Calc(pid_speed,motor.speed_rpm,set_motor_speed);
		
	}
	else if(now_mode==position)
	{
		get_motor_ecd_degree=(fp32)motor.ecd/8191.0f*360.0f;
		//PID_OVER_ZERO(&set_motor_ecd,&get_motor_ecd_degree);
		
		if(motor.ecd-motor.last_ecd<-4000)
		{
			motor.round++;
		}
		if(motor.ecd-motor.last_ecd>4000)
		{
			motor.round--;
		}
		real_ecd_degree = get_motor_ecd_degree+360*motor.round;
		
		yaw0=PID_Calc(pid_ecd,real_ecd_degree,set_motor_ecd);
		yaw=(int16_t)PID_Calc(pid_speed,motor.speed_rpm,yaw0);
	}
	

	
	//u16 yaw=-1000;
	
    TxMessage.Data[0] = (yaw >> 8);
    TxMessage.Data[1] = yaw;
//    TxMessage.Data[2] = (pitch >> 8);
//    TxMessage.Data[3] = pitch;
//    TxMessage.Data[4] = (shoot >> 8);
//    TxMessage.Data[5] = shoot;
//    TxMessage.Data[6] = (rev >> 8);
//    TxMessage.Data[7] = rev;

    CAN_Transmit( CAN1,  &TxMessage);

}



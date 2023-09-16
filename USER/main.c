#include "stm32f4xx.h"
#include "sys.h"
#include "delay.h"
#include "LED.h"
#include "usart.h"
#include "PWM.h"
#include "CAN.h"
#include "PID.h"

/*

//红绿灯交替点亮，1s交替一次
int main()
{
	
	delay_init(168);
	LED_Init();
	//u16 flag=0;
	while(1)
	{
		GPIO_ResetBits(GPIOC,GPIO_Pin_2);
		GPIO_SetBits(GPIOC,GPIO_Pin_3);
		delay_ms(500);
		GPIO_SetBits(GPIOC,GPIO_Pin_2);
		delay_ms(500);
		GPIO_ResetBits(GPIOC,GPIO_Pin_2);
		GPIO_ResetBits(GPIOC,GPIO_Pin_3);
		delay_ms(500);
		GPIO_SetBits(GPIOC,GPIO_Pin_2);
		delay_ms(500);
	}
}

*/


/*

//实现流水灯效果 舵机转动指定角度
int main(void)
{
	u16 led0pwmval=0;
	u8 dir=1;
	delay_init(168); 
	
	
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);
	LED_Init();
	
//	GPIO_ResetBits(GPIOC,GPIO_Pin_9);
//	delay_ms(300);
//	GPIO_SetBits(GPIOC,GPIO_Pin_9);
//	
	
	uart_init(115200);
	TIM8_PWM_Init(84-1,499); //(72*20000)/72000000=0.02
	//LED_G=0;
	while(1)
	{
//		delay_ms(500);
//		if(dir)
//		{
//			LED_G=1;
//			led0pwmval++;
//			LED_R=0;
//		}
//		else 
//		{
//			LED_R=1;
//			led0pwmval--; 
//			LED_G=0;
//			
//		}
//		
//		if(led0pwmval>62)
//			dir=0;
//		if(led0pwmval==12)
//			dir=1; 
//		TIM_SetCompare4(TIM3,led0pwmval);	
//		
		
		delay_ms(3000);
		TIM_SetCompare4(TIM3,12); //ccr从12.5—62.5
		

	}
	
	
}

*/

/*
//这个是串口通讯实验

int main(void)
{
	u8 t,len;
	u16 times=0;
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);
	delay_init(168); //延时初始化
	uart_init(115200); //串口初始化波特率为 115200
	LED_Init();
	//GPIO_ResetBits(GPIOC,GPIO_Pin_2);
	LED_R=0; 
	printf("实验 开始 \r\n");
	delay_ms(500);//程序启动红灯闪烁500ms
	LED_R=!LED_R;
	while(1)
	{
		USART_GetFlagStatus(UART5, USART_RX_STA);
		//if(USART_ReceiveData(UART5))
		if(USART_RX_STA&0x8000)
		{
			printf("收到data\r\n");//收到数据黄灯闪烁300ms
			LED_R=0;
			LED_G=0;
			delay_ms(300);
			LED_R=1;
			LED_G=1;
			delay_ms(300);
			USART_RX_STA=0;
		}
		else
		{
			
			times++;
			if(times%5000==0)
			{ 
				printf("\r\n 这是串口通信实验 \r\n");
			}
			if(times%200==0)
				printf("请输入数据,以回车键结束\r\n");
			if(times%30==0)
				LED_G=!LED_G;//闪烁 LED,提示系统正在运行.
			delay_ms(10);
		}
	}
	
	
}
*/


fp32 pid[3];

PidTypeDef  Pid_Speed,Pid_Ecd;


#define speed 0
#define ecd 1
/**
3508canshu
	fp32 kp_speed=15;
	fp32 ki_speed=0.003;
	fp32 kd_speed=1;
	fp32 kp_ecd=10;
	fp32 ki_ecd=0;
	fp32 kd_ecd=0;
*/

//gm6020canshu
	fp32 kp_speed=40;
	fp32 ki_speed=20;
	fp32 kd_speed=0;
	fp32 kp_ecd=0;
	fp32 ki_ecd=0;
	fp32 kd_ecd=0;

	

float ecd_degree,speed_rpm,delta=0.0f;;
int main()
{
	delay_init(168);
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);
	
	
	speed_rpm=150.0f;         //转速
	ecd_degree=2000.0f;           //位置，单位degree
	uart_init(115200);
	LED_Init();
	LED_R=0;
	
	/*
	0.6 0 2
	5 0.5 2
	*/
	

	PID_Init(&Pid_Speed,PID_POSITION,kp_speed,ki_speed,kd_speed,30000.0f,5000.0f);
	PID_Init(&Pid_Ecd,PID_POSITION,kp_ecd,ki_ecd,kd_ecd,8192.0f,8192.0f);
	
	CAN1_mode_init(CAN_SJW_1tq, CAN_BS2_7tq, CAN_BS1_6tq, 3);
	
	u32 t=0,if_show_position=0,time_forward=0,time_backward=0;//表演模式 1开启 0关闭
	u32 if_position_control=0;//1 位置控制 0 速度控制
	
	while(1)
	{
		delay_ms(1);
		t++;
		if(t==1000)
		{
			t=0;
////			ecd_degree+=90.0f;
////			
////			if(ecd_degree>360.0f)
////			{
////				ecd_degree-=360.0f;
////			}
////			
			LED_R=!LED_R;
			
		}
		//CAN1_CMD_TRANSMIT(&Pid_Speed,&Pid_Ecd,speed_rpm,ecd_degree,position);
		
		if(if_show_position&&if_position_control)
		{
			if(time_backward<=6)
			{
				CAN1_CMD_TRANSMIT(&Pid_Speed,&Pid_Ecd,speed_rpm,delta,position);
				delta-=1.0f;
				if(delta < -360.0f) 
				{
					delta+=360.0f;
					time_backward++;
				}
			}
			else if(time_forward<=3)
			{
				CAN1_CMD_TRANSMIT(&Pid_Speed,&Pid_Ecd,speed_rpm,delta,position);
				delta+=1.0f;
				if(delta>360.0f) 
				{
					delta-=360.0f;
					time_forward++;
				}
			}
			else
			{
				CAN1_CMD_TRANSMIT(&Pid_Speed,&Pid_Ecd,speed_rpm,0.0f,position);
			}
		}
		else if(if_position_control)
		{
			if(ecd_degree < -360.0f)
			{
				CAN1_CMD_TRANSMIT(&Pid_Speed,&Pid_Ecd,speed_rpm,delta,position);
				delta-=1.0f;
				if(delta < -360.0f) 
				{
					delta+=360.0f;
					ecd_degree+=360.0f;
				}
			}
			else if(delta>=ecd_degree&&ecd_degree<0)
			{
				
				CAN1_CMD_TRANSMIT(&Pid_Speed,&Pid_Ecd,speed_rpm,delta,position);
				delta-=1.0f;
			}
			else if(ecd_degree>360.0f)
			{
				CAN1_CMD_TRANSMIT(&Pid_Speed,&Pid_Ecd,speed_rpm,delta,position);
				delta+=1.0f;
				if(delta>360.0f) 
				{
					delta-=360.0f;
					ecd_degree-=360.0f;
				}
			}
			else if(delta<=ecd_degree&&ecd_degree>0)
			{
				
				CAN1_CMD_TRANSMIT(&Pid_Speed,&Pid_Ecd,speed_rpm,delta,position);
				delta+=1.0f;
			}
			else
			{
				CAN1_CMD_TRANSMIT(&Pid_Speed,&Pid_Ecd,speed_rpm,ecd_degree,position);
			}
		}
		else
		{
			CAN1_CMD_TRANSMIT(&Pid_Speed,&Pid_Ecd,speed_rpm,ecd_degree,speed);
		}
		
	}
	
}



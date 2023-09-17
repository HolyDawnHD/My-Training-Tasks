#include "stm32f4xx.h"
#include "sys.h"
#include "delay.h"
#include "LED.h"
#include "usart.h"
#include "PWM.h"
#include "CAN.h"
#include "PID.h"
#include "DMA.h"

/*
// ******************09/06*****************************
//红绿灯交替点亮，1s绿灯交替一次，2s红灯交替一次
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

// **********************09/08*********************

//实现流水灯效果 舵机转动指定角度/转速

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
//	TIM8_PWM_Init(499,83); //LED流水灯(500*84)/42000000=0.001
	TIM8_PWM_Init(20000-1,42-1);//舵机（20000*42）/42000000=0.02
	LED_G=0;
	while(1)
	{
		delay_ms(500);
		if(dir)
		{
			LED_G=1;
			led0pwmval++;
			LED_R=0;
		}
		else 
		{
			LED_R=1;
			led0pwmval--; 
			LED_G=0;
			
		}
		
		if(led0pwmval>450)    //LED流水灯
			dir=0;
		if(led0pwmval==0)
			dir=1; 
		TIM_SetCompare4(TIM3,led0pwmval);	
		
		
//		if(led0pwmval>2500)    //舵机角度控制角度换算函数anglee
//			dir=0;
//		if(led0pwmval==500)
//			dir=1; 
//		TIM_SetCompare4(TIM3,led0pwmval);	
		
//		TIM_SetCompare4(TIM3,12); //ccr从12.5—62.5
	}
	
	
}
*/


/*

// ********************09/08********************

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



/*
// ***************************09/17补上dma作业***********************

//这一版根据我的理解写的，但是收到的除了第一组数据后面都是框框，有空再改

int main(void)
{
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);
	delay_init(168); //延时初始化
	uart_init(115200); //串口初始化波特率为 115200
	u32 i;
	dma_Init();
	
	LED_Init();
	LED_R=0; 
	LED_G=1;
	delay_ms(500);  //程序启动红灯闪烁500ms
	LED_R=!LED_R;
	
	while(1)
	{
		delay_ms(100);
		i++;
		if(i%5==0)
		{
			LED_G=!LED_G;
		}
		SendData();
	}
}

*/


// ***************************09/17dma作业能用*******************
//这一版能用了，跟着教程来的

#define SEND_BUF_SIZE 13
u8 SendBuff[SEND_BUF_SIZE]; //发送数据缓冲区
const u8 TEXT_TO_SEND[]={"zyhy nb!\n\r"};
 
int main(void)
{ 
	u16 i;
	u8 t=0,j,mask=0;

	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);
	delay_init(168); //初始化延时函数
	uart_init(115200); 
	LED_Init(); 

	//DMA1,STEAM7,CH4,外设为串口5,存储器为 SendBuff,长度为:SEND_BUF_SIZE.
	MYDMA_Config(DMA1_Stream7,DMA_Channel_4,(u32)&UART5->DR,(u32)SendBuff,SEND_BUF_SIZE);
	
	j=sizeof(TEXT_TO_SEND); 
	for(i=0;i<SEND_BUF_SIZE;i++)
	{
		if(t>=j)
		{
			if(mask)
			{
				SendBuff[i]=0x0a;
				t=0;
			}
			else 
			{
				SendBuff[i]=0x0d;
				mask++;
			}
		}
		else
		{
			mask=0;
			SendBuff[i]=TEXT_TO_SEND[t];
			t++;
		} 
	}
	LED_R=0;
	delay_ms(500);
	LED_R=1;
	i=0;
	while(1)
	{
		delay_ms(1000);

		USART_DMACmd(UART5,USART_DMAReq_Tx,ENABLE); 
		MYDMA_Enable(DMA1_Stream7,SEND_BUF_SIZE); 
		while(1)
		{
			if(DMA_GetFlagStatus(DMA1_Stream7,DMA_FLAG_TCIF7)!=RESET)
			{ 
				DMA_ClearFlag(DMA1_Stream7,DMA_FLAG_TCIF7);//清传输完成标志
				break; 
			}
				
		} 
		i++;
		if(i==5)
		{
			LED_G=!LED_G;//提示系统正在运行
			i=0;
		} 
	} 
	
}




/*

// ***********************09/14 初版****************

//can通讯 pid控制3508电机
//@brief 当前位置环在到达指定位置时恒定转速到达位置，下一步打算重新处理过零点问题，给get+8191使用pid在输入角度与当前角度之前进行位置控制

fp32 pid[3];

PidTypeDef  Pid_Speed,Pid_Ecd;

#define speed 0
#define ecd 1

u32 t=0,if_show_position=0,time_forward=0,time_backward=1;//表演模式 1开启 0关闭
u32 if_position_control=1;//1 位置控制 0 速度控制
float ecd_degree,speed_rpm,delta=0.0f;

//3508canshu
	fp32 kp_speed=15;
	fp32 ki_speed=0.003;
	fp32 kd_speed=1;
	fp32 kp_ecd=10;
	fp32 ki_ecd=0;
	fp32 kd_ecd=0;


//gm6020canshu
//	fp32 kp_speed=40;
//	fp32 ki_speed=20;
//	fp32 kd_speed=0;
//	fp32 kp_ecd=0;
//	fp32 ki_ecd=0;
//	fp32 kd_ecd=0;



int main()
{
	delay_init(168);
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);
	
	
	speed_rpm=150.0f;         //转速
	ecd_degree=2000.0f;           //位置，单位degree
	uart_init(115200);
	LED_Init();
	LED_R=0;
	

//	0.6 0 2
//	5 0.5 2

	

	PID_Init(&Pid_Speed,PID_POSITION,kp_speed,ki_speed,kd_speed,30000.0f,5000.0f);
	PID_Init(&Pid_Ecd,PID_POSITION,kp_ecd,ki_ecd,kd_ecd,8192.0f,8192.0f);
	
	CAN1_mode_init(CAN_SJW_1tq, CAN_BS2_7tq, CAN_BS1_6tq, 3);

	while(1)
	{
		delay_ms(1);
		t++;
		if(t==1000)
		{
			t=0;
////			ecd_degree+=90.0f;//1s自动转90
////			
////			if(ecd_degree>360.0f)
////			{
////				ecd_degree-=360.0f;
////			}		

			LED_R=!LED_R;
			
		}
		//CAN1_CMD_TRANSMIT(&Pid_Speed,&Pid_Ecd,speed_rpm,ecd_degree,position);
		
		if(if_show_position&&if_position_control)//表演模式 反转6圈正转3圈停在0°位置
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
		else if(if_position_control)//位置控制，可以超过360°
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
		else//速度控制，制定转速
		{
			CAN1_CMD_TRANSMIT(&Pid_Speed,&Pid_Ecd,speed_rpm,ecd_degree,speed);
		}
	}
}

*/

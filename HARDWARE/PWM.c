#include"PWM.h"
#include "sys.h"

u16 anglee(float ang)
{
	return ang/180*50+12;
	
}



void TIM8_PWM_Init(u32 arr,u32 psc)
{
	//pc9   tim3_ch4
	
	GPIO_InitTypeDef GPIO_InitStructure;
	TIM_TimeBaseInitTypeDef TIM_TimeBaseStructure;
	TIM_OCInitTypeDef TIM_OCInitStructure;
	
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3,ENABLE);
	RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOC, ENABLE);
	//RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOF, ENABLE);

	GPIO_PinAFConfig(GPIOC,GPIO_PinSource9,GPIO_AF_TIM3);//pc9fuyongweiTIM3
	
	GPIO_InitStructure.GPIO_Pin=GPIO_Pin_9;
	GPIO_InitStructure.GPIO_OType=GPIO_OType_PP;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF; 		 //复用
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;	
	GPIO_InitStructure.GPIO_PuPd=GPIO_PuPd_UP;
	GPIO_Init(GPIOC, &GPIO_InitStructure);	
	
//	GPIO_InitStructure.GPIO_Pin=GPIO_Pin_9;
//	GPIO_InitStructure.GPIO_OType=GPIO_OType_PP;
//	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF; 		 
//	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz;	
//	GPIO_InitStructure.GPIO_PuPd=GPIO_PuPd_UP;
//	GPIO_Init(GPIOF, &GPIO_InitStructure);	
	
	
	TIM_TimeBaseStructure.TIM_Prescaler=psc; 
	TIM_TimeBaseStructure.TIM_Period=arr; 
	TIM_TimeBaseStructure.TIM_ClockDivision=TIM_CKD_DIV1; 
	TIM_TimeBaseStructure.TIM_CounterMode=TIM_CounterMode_Up; 
	TIM_TimeBaseInit(TIM3,&TIM_TimeBaseStructure);
	
	

	TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM1; 
	TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable; 
	TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_Low; 
	TIM_OC4Init(TIM3, &TIM_OCInitStructure); 
	TIM_OC4PreloadConfig(TIM3, TIM_OCPreload_Enable); 
	
	TIM_ARRPreloadConfig(TIM3,ENABLE);
	TIM_Cmd(TIM3, ENABLE); 
	
	TIM_CtrlPWMOutputs(TIM3,ENABLE);
}

/*
void TIM14_PWM_Init(u32 arr,u32 psc)
{
GPIO_InitTypeDef GPIO_InitStructure;
TIM_TimeBaseInitTypeDef TIM_TimeBaseStructure;
TIM_OCInitTypeDef TIM_OCInitStructure;
RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM14,ENABLE);//TIM14 时钟使能
RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOF, ENABLE); //使能 PORTF 时钟
	
GPIO_PinAFConfig(GPIOF,GPIO_PinSource9,GPIO_AF_TIM14); //GF9 复用为 TIM14
GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9; //GPIOF9
GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF; //复用功能
GPIO_InitStructure.GPIO_Speed = GPIO_Speed_100MHz; //速度 50MHz
GPIO_InitStructure.GPIO_OType = GPIO_OType_PP; //推挽复用输出
GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP; //上拉
GPIO_Init(GPIOF,&GPIO_InitStructure); //初始化 PF9

TIM_TimeBaseStructure.TIM_Prescaler=psc; //定时器分频
TIM_TimeBaseStructure.TIM_CounterMode=TIM_CounterMode_Up; //向上计数模式
TIM_TimeBaseStructure.TIM_Period=arr; //自动重装载值
TIM_TimeBaseStructure.TIM_ClockDivision=TIM_CKD_DIV1;
	
	
	TIM_TimeBaseInit(TIM14,&TIM_TimeBaseStructure);//初始化定时器 14
//初始化 TIM14 Channel1 PWM 模式
TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_PWM1; //PWM 调制模式 1
TIM_OCInitStructure.TIM_OutputState = TIM_OutputState_Enable; //比较输出使能
TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_Low; //输出极性低
TIM_OC1Init(TIM14, &TIM_OCInitStructure); //初始化外设 TIM1 4OC1
TIM_OC1PreloadConfig(TIM14, TIM_OCPreload_Enable); //使能预装载寄存器
 TIM_ARRPreloadConfig(TIM14,ENABLE);//ARPE 使能
TIM_Cmd(TIM14, ENABLE); //使能 TIM14
}
*/


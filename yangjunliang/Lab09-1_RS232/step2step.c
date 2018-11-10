#include <msp430f6638.h>
#include <stdint.h>
#include <stdio.h>
#include "dr_tft.h"

void GPIO_init(void)
void TimerA_Init(void);		//定时器TA初始化函数
void initClock(void);

void initialization(void);


void main(void)
{
	WDTCTL = WDTPW + WDTHOLD;//关闭看门狗

	_DINT();
	initialization();
	etft_AreaSet(0,0,319,239,0);

	_EINT();
	

	while(1)
	{
		etft_AreaSet(0,0,39,239,0);
		etft_AreaSet(40,0,79,239,31);
		etft_AreaSet(80,0,119,239,2016);
		etft_AreaSet(120,0,159,239,63488);
		etft_AreaSet(160,0,199,239,2047);
		etft_AreaSet(200,0,239,239,63519);
		etft_AreaSet(240,0,279,239,65504);
		etft_AreaSet(280,0,319,239,65535);
		__delay_cycles(MCLK_FREQ*3);
		etft_AreaSet(0,0,319,239,0);
		__delay_cycles(MCLK_FREQ);
		etft_DisplayString("TI MSP430F6638 EVM",100,80,65535,0);
		etft_DisplayString("TI UNIVERSITY PROGRAM",0,150,63488,0);
		etft_DisplayString("- TSINGHUA UNIVERSITY",100,180,65504,0);
		__delay_cycles(MCLK_FREQ*3);

		// background
	}
}

void initialization(void)
{
	GPIO_init();
	TimerA_Init();
	initClock();
  	initTFT();
}
void GPIO_init(void)
{
	P1DIR |= BIT5;//控制蜂鸣器输出
	P4DIR |= BIT5;//控制LED输出
}
	

void TimerA_Init(void)		//定时器TA初始化函数
{
	TA0CTL |= MC_1 + TASSEL_2 + TACLR;	//时钟为SMCLK,比较模式，开始时清零计数器
	TA0CCTL0 = CCIE;					//比较器中断使能
	TA0CCR0  = 50000;					//比较值设为50000，相当于50ms的时间间隔
}


void initClock(void)
{
	while(BAKCTL & LOCKIO) // Unlock XT1 pins for operation
	BAKCTL &= ~(LOCKIO);
	UCSCTL6 &= ~XT1OFF; //启动XT1
	P7SEL |= BIT2 + BIT3; //XT2引脚功能选择
	UCSCTL6 &= ~XT2OFF; //启动XT2
	while(SFRIFG1 & OFIFG) { //等待XT1、XT2与DCO稳定
	UCSCTL7 &= ~(DCOFFG+XT1LFOFFG+XT2OFFG);
	SFRIFG1 &= ~OFIFG;
	}

	UCSCTL4 = SELA__XT1CLK + SELS__XT2CLK + SELM__XT2CLK; //避免DCO调整中跑飞

	UCSCTL1 = DCORSEL_5; //6000kHz~23.7MHz
	UCSCTL2 = 20000000 / (4000000 / 16); //XT2频率较高，分频后作为基准可获得更高的精度
	UCSCTL3 = SELREF__XT2CLK + FLLREFDIV__16; //XT2进行16分频后作为基准
	while (SFRIFG1 & OFIFG) { //等待XT1、XT2与DCO稳定
	UCSCTL7 &= ~(DCOFFG+XT1LFOFFG+XT2OFFG);
	SFRIFG1 &= ~OFIFG;
	}
	UCSCTL5 = DIVA__1 + DIVS__1 + DIVM__1; //设定几个CLK的分频
	UCSCTL4 = SELA__XT1CLK + SELS__DCOCLK + SELM__DCOCLK; //设定几个CLK的时钟源
}


//foreground
//TimerA
#pragma vector = TIMER0_A0_VECTOR
__interrupt void Timer_A (void)
{
	P1OUT ^= BIT5;//形成鸣叫效果
	P4OUT ^= BIT5;//形成闪灯效果
}
//UART
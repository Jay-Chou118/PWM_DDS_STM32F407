/**
 ****************************************************************************************************
 * @file        dac9833.c
 * @author      科一电子
 * @version     V1.0
 * @date        2020-04-21
 * @brief       AD9833驱动程序
 
 		 5V			-->			VCC
		 GND		-->			GND
		 PA7		-->			SDA
		 PA5		-->			SCLK
		 PE2		-->			CS1
		 PE3		-->			CS2

 ****************************************************************************************************
 */

#include "ad9833.h"
#include "delay.h"

/**
 * @brief       AD9833初始化
 * @param       无
 * @retval      无
 */
void AD9833_Init(void)
{
	GPIO_InitTypeDef gpio_init_struct;
	__HAL_RCC_GPIOA_CLK_ENABLE();					  /* 开启GPIOE时钟使能 */
	__HAL_RCC_GPIOE_CLK_ENABLE();					  /* 开启GPIOE时钟使能 */
	
	gpio_init_struct.Pin = GPIO_PIN_3 | GPIO_PIN_2;
	gpio_init_struct.Mode = GPIO_MODE_OUTPUT_PP;      /* 推挽输出 */
	gpio_init_struct.Pull = GPIO_PULLUP;              /* 上拉 */
	gpio_init_struct.Speed = GPIO_SPEED_FREQ_HIGH;    /* 高速 */
	HAL_GPIO_Init(GPIOE, &gpio_init_struct);       	  /* 初始化引脚 */
	
	gpio_init_struct.Pin = GPIO_PIN_5 | GPIO_PIN_7;
	gpio_init_struct.Mode = GPIO_MODE_OUTPUT_PP;      /* 推挽输出 */
	gpio_init_struct.Pull = GPIO_PULLUP;              /* 上拉 */
	gpio_init_struct.Speed = GPIO_SPEED_FREQ_HIGH;    /* 高速 */
	HAL_GPIO_Init(GPIOA, &gpio_init_struct);       	  /* 初始化引脚 */
	
	delay_ms(100);
	AD9833_Set_Register(AD9833_ALL, AD9833_Reg_Cmd | AD9833_Reset);
	delay_ms(10);
}

/**
 * @brief       SPI传输一个16位的数据
 * @param       data：要传输的值
 * @retval      无
 */
void SPI_Write_16bit(uint16_t data)
{
	uint8_t i;
	uint16_t Send_Data = data;
	
	for(i=0;i<16;i++)
	{
		if(Send_Data&0x8000)
			AD9833_SDA_H;
		else
			AD9833_SDA_L;
		AD9833_SCLK_L;
		Send_Data<<=1;
		AD9833_SCLK_H;
	}
}

/**
 * @brief       设置AD9833寄存器的值
 * @param       ch：要设置的通道，reg：要设置寄存器的值
 * @retval      无
 */
void AD9833_Set_Register(uint16_t ch, uint16_t reg)
{
	if(ch == AD9833_ALL)
	{
		AD9833_CS1_L;	 
		AD9833_CS2_L;
		SPI_Write_16bit(reg);
		AD9833_CS1_H;
		AD9833_CS2_H;
	}
	else if(ch == AD9833_CH1)
	{
		AD9833_CS1_L;	 
		SPI_Write_16bit(reg);
		AD9833_CS1_H;
	}
	else if(ch == AD9833_CH2)
	{
		AD9833_CS2_L;	 
		SPI_Write_16bit(reg);
		AD9833_CS2_H;
	}
}

/**
 * @brief       设置AD9833的频率寄存器
 * @param       ch：要设置的通道，Type：波形类型 Frequency：频率的值
 * @retval      无
 */
void AD9833_Set_Frequency(uint16_t ch, uint16_t Type, uint32_t Frequency)
{
	uint16_t  Fre_H = AD9833_Reg_Freq0;
	uint16_t  Fre_L = AD9833_Reg_Freq0;
	
	Fre_H |= (Frequency & 0xFFFC000) >> 14 ;
	Fre_L |= (Frequency & 0x3FFF);

	AD9833_Set_Register(ch, AD9833_B28);
	AD9833_Set_Register(ch, Fre_L);
	AD9833_Set_Register(ch, Fre_H);
	AD9833_Set_Register(ch, AD9833_Reg_Cmd | AD9833_B28 | Type);
}

/**
 * @brief       设置AD9833的相位
 * @param       type：波形类型，Frequency：频率的值，Phase1：通道1的相位值，Phase2：通道2的相位值
 * @retval      无
 */
void AD9833_Set_Phase(uint16_t Type, uint32_t Frequency, float Phase1, float Phase2)
{
	uint16_t  Fre_H = AD9833_Reg_Freq0;
	uint16_t  Fre_L = AD9833_Reg_Freq0;
	uint16_t  Value1 = AD9833_Reg_Phase0;
	uint16_t  Value2 = AD9833_Reg_Phase0;
	uint16_t  P1 = 0;
	uint16_t  P2 = 0;
	
	if(Phase1 >= 360 || Phase1 == 0)
		P1 = 0;
	else
		P1 = (uint16_t)((float)Phase1/360.0*4095.0);
	
	if(Phase2 >= 360 || Phase2 == 0)
		P2 = 0;
	else
		P2 = (uint16_t)((float)Phase2/360.0*4095.0);
	
	Fre_H |= (Frequency & 0xFFFC000) >> 14 ;
	Fre_L |= (Frequency & 0x3FFF);
	Value1 |= P1 & 0x0FFF;
	Value2 |= P2 & 0x0FFF;

	AD9833_Set_Register(AD9833_ALL, AD9833_B28 | AD9833_Reset);
	AD9833_Set_Register(AD9833_ALL, Fre_L);
	AD9833_Set_Register(AD9833_ALL, Fre_H);
	AD9833_Set_Register(AD9833_CH1, Value1);
	AD9833_Set_Register(AD9833_CH2, Value2);
	AD9833_Set_Register(AD9833_ALL, AD9833_Reg_Cmd | AD9833_B28 | Type);
	
}

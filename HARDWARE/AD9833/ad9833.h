/**
 ****************************************************************************************************
 * @file        ad9833.h
 * @author      科一电子
 * @version     V1.0
 * @date        2020-04-21
 * @brief       AD9833驱动程序
 ****************************************************************************************************
 */
#ifndef _AD9833_H_
#define _AD9833_H_
#include "sys.h"

/*********************************引脚声明*********************************************/
#define AD9833_SDA_L      	HAL_GPIO_WritePin(GPIOA, GPIO_PIN_7, GPIO_PIN_RESET)
#define AD9833_SDA_H     	HAL_GPIO_WritePin(GPIOA, GPIO_PIN_7, GPIO_PIN_SET)
#define AD9833_SCLK_L      	HAL_GPIO_WritePin(GPIOA, GPIO_PIN_5, GPIO_PIN_RESET)
#define AD9833_SCLK_H     	HAL_GPIO_WritePin(GPIOA, GPIO_PIN_5, GPIO_PIN_SET)
#define AD9833_CS1_L      	HAL_GPIO_WritePin(GPIOE, GPIO_PIN_2, GPIO_PIN_RESET)
#define AD9833_CS1_H     	HAL_GPIO_WritePin(GPIOE, GPIO_PIN_2, GPIO_PIN_SET)
#define AD9833_CS2_L      	HAL_GPIO_WritePin(GPIOE, GPIO_PIN_3, GPIO_PIN_RESET)
#define AD9833_CS2_H     	HAL_GPIO_WritePin(GPIOE, GPIO_PIN_3, GPIO_PIN_SET)

/*********************************AD9833通道选择*********************************************/
#define AD9833_ALL			0
#define AD9833_CH1			1
#define AD9833_CH2			2

/*********************************AD9833寄存器*********************************************/
#define AD9833_Reg_Cmd		(0 << 14)
#define AD9833_Reg_Freq0	(1 << 14)
#define AD9833_Reg_Freq1	(2 << 14)
#define AD9833_Reg_Phase0	(6 << 13)
#define AD9833_Reg_Phase1	(7 << 13)

/*********************************AD9833命令控制位*********************************************/
#define AD9833_B28			(1 << 13)
#define AD9833_HlB			(1 << 12)
#define AD9833_fSel0		(0 << 11)
#define AD9833_fSel1		(1 << 11)
#define AD9833_pSel0		(0 << 10)
#define AD9833_pSel1		(1 << 10)
#define AD9833_Pin_SW		(1 << 9)
#define AD9833_Reset		(1 << 8)
#define AD9833_Sleep1		(1 << 7)
#define AD9833_Sleep2		(1 << 6)
#define AD9833_Opbiten		(1 << 5)
#define AD9833_Sign_Pib		(1 << 4)
#define AD9833_Div2			(1 << 3)
#define AD9833_Mode			(1 << 1)

#define AD9833_Out_Sinus	((0 << 5) | (0 << 1) | (0 << 3))
#define AD9833_Out_Triangle	((0 << 5) | (1 << 1) | (0 << 3))
#define AD9833_Out_Msb		((1 << 5) | (0 << 1) | (1 << 3))
#define AD9833_Out_Msb2		((1 << 5) | (0 << 1) | (0 << 3))

/*******************************外部函数声明*******************************************/
extern void AD9833_Init(void);
extern void SPI_Write_16bit(uint16_t data);
extern void AD9833_Set_Register(uint16_t ch, uint16_t reg);
extern void AD9833_Set_Frequency(uint16_t ch, uint16_t Type, uint32_t Frequency);
extern void AD9833_Set_Phase(uint16_t Type, uint32_t Frequency, float Phase1, float Phase2);


#endif

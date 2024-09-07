/**
 ******************************************************************************
 * @author: Nark
 * @date:   21/08/2024
 ******************************************************************************
**/

#include "io_cfg.h"

#include "misc.h"
#include "stm32l1xx_gpio.h"
#include "stm32l1xx_rcc.h"
#include "stm32l1xx_usart.h"
#include "stm32l1xx_spi.h"
#include "stm32l1xx_tim.h"
#include "stm32l1xx_exti.h"
#include "stm32l1xx_i2c.h"
#include "stm32l1xx_flash.h"
#include "stm32l1xx_dma.h"
#include "stm32l1xx_syscfg.h"

#include "xprintf.h"
#include "sys_cfg.h"
#include "platform.h"

/******************************************************************************
* led life function
*******************************************************************************/
void led_life_init() {
	GPIO_InitTypeDef        GPIO_InitStructure;

	RCC_AHBPeriphClockCmd(LED_LIFE_IO_CLOCK, ENABLE);

	GPIO_InitStructure.GPIO_Pin = LED_LIFE_IO_PIN;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_2MHz;
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
	GPIO_Init(LED_LIFE_IO_PORT, &GPIO_InitStructure);
}

void led_life_on() {
	GPIO_ResetBits(LED_LIFE_IO_PORT, LED_LIFE_IO_PIN);
}

void led_life_off() {
	GPIO_SetBits(LED_LIFE_IO_PORT, LED_LIFE_IO_PIN);
}

/******************************************************************************
* usart1 function
*******************************************************************************/
void usart1_init(uint32_t baudrate) {
	USART_InitTypeDef USART_InitStructure;
	GPIO_InitTypeDef GPIO_InitStructure;
	NVIC_InitTypeDef NVIC_InitStructure;

	/* enable clocks */
	RCC_AHBPeriphClockCmd(CONSOLE_GPIO_CLOCK, ENABLE);
	RCC_APB2PeriphClockCmd(CONSOLE_USART_CLOCK, ENABLE);

	/* pin af config */
	GPIO_PinAFConfig(CONSOLE_GPIO_PORT, CONSOLE_TX_PIN_SOURCE, CONSOLE_GPIO_AF);
	GPIO_PinAFConfig(CONSOLE_GPIO_PORT, CONSOLE_RX_PIN_SOURCE, CONSOLE_GPIO_AF);

	/* gpio config */
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_2MHz;
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;
	GPIO_InitStructure.GPIO_Pin = CONSOLE_TX_PIN;
	GPIO_Init(CONSOLE_GPIO_PORT, &GPIO_InitStructure);
	GPIO_InitStructure.GPIO_Pin = CONSOLE_RX_PIN;
	GPIO_Init(CONSOLE_GPIO_PORT, &GPIO_InitStructure);

	/* usart1 config */
	USART_InitStructure.USART_BaudRate = baudrate;
	USART_InitStructure.USART_WordLength = USART_WordLength_8b;
	USART_InitStructure.USART_StopBits = USART_StopBits_1;
	USART_InitStructure.USART_Parity = USART_Parity_No;
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
	USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;
	USART_Init(CONSOLE_USART, &USART_InitStructure);

	/* enable the usart interrupt */
	NVIC_InitStructure.NVIC_IRQChannel = CONSOLE_INTERRUPT;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 2;
	NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStructure);

	USART_ClearITPendingBit(CONSOLE_USART, USART_IT_RXNE | USART_IT_TXE);
	USART_ITConfig(CONSOLE_USART, USART_IT_RXNE, ENABLE);
	USART_ITConfig(CONSOLE_USART, USART_IT_TXE, DISABLE);

	/* enable usart */
	USART_Cmd(CONSOLE_USART, ENABLE);
}

void usart1_put_char(uint8_t c) {
	/* wait last transmission completed */
	while (USART_GetFlagStatus(CONSOLE_USART, USART_FLAG_TXE) == RESET);

	/* put transmission data */
	USART_SendData(CONSOLE_USART, (uint8_t)c);

	/* wait transmission completed */
	while (USART_GetFlagStatus(CONSOLE_USART, USART_FLAG_TC) == RESET);
}

uint8_t usart1_get_char() {
	volatile uint8_t c;

	if (USART_GetITStatus(USART1, USART_IT_RXNE) == SET) {
		c = (uint8_t)USART_ReceiveData(USART1);
		USART_ClearITPendingBit(USART1, USART_IT_RXNE);
	}

	return c;
}

/******************************************************************************
* internal flash function
*******************************************************************************/
void internal_flash_unlock() {
	FLASH_Unlock();
	FLASH_ClearFlag(FLASH_FLAG_EOP|FLASH_FLAG_WRPERR | FLASH_FLAG_PGAERR |
					FLASH_FLAG_SIZERR | FLASH_FLAG_OPTVERR | FLASH_FLAG_OPTVERRUSR);
}

void internal_flash_lock() {
	FLASH_Lock();
}

void internal_flash_erase_pages(uint32_t address, uint32_t len) {
	uint32_t page_number;
	uint32_t index;

	page_number = len / 256;

	if ((page_number * 256) < len) {
		page_number++;
	}

	for (index = 0; index < page_number; index++) {
		FLASH_ErasePage(address + (index * 256));
	}
}

uint8_t internal_flash_write(uint32_t address, uint8_t* data, uint32_t len) {
	uint32_t temp;
	uint32_t index = 0;
	FLASH_Status flash_status = FLASH_BUSY;

	while (index < len) {
		temp = 0;

		memcpy(&temp, &data[index], (len - index) >= sizeof(uint32_t) ? sizeof(uint32_t) : (len - index));

		flash_status = FLASH_FastProgramWord(address + index, temp);

		if(flash_status == FLASH_COMPLETE) {
			index += sizeof(uint32_t);
		}
		else {
			FLASH_ClearFlag(FLASH_FLAG_EOP|FLASH_FLAG_WRPERR | FLASH_FLAG_PGAERR |
							FLASH_FLAG_SIZERR | FLASH_FLAG_OPTVERR | FLASH_FLAG_OPTVERRUSR);
		}
	}

	return 1;
}

/******************************************************************************
* spi1 function
*******************************************************************************/
void spi_init() {
	GPIO_InitTypeDef GPIO_InitStructure;
	SPI_InitTypeDef SPI_InitStructure;

	/* enable clocks */
	RCC_AHBPeriphClockCmd(RCC_AHBPeriph_GPIOA, ENABLE);
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_SPI1, ENABLE);

	/* gpio config */
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_5;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_40MHz;
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStructure.GPIO_PuPd  = GPIO_PuPd_UP;
	GPIO_Init(GPIOA, &GPIO_InitStructure);

	/* MISO pin config */
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_6;
	GPIO_Init(GPIOA, &GPIO_InitStructure);

	/* MOSI pin config */
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_7;
	GPIO_Init(GPIOA, &GPIO_InitStructure);

	/* pin af config */
	GPIO_PinAFConfig(GPIOA, GPIO_PinSource5, GPIO_AF_SPI1);
	GPIO_PinAFConfig(GPIOA, GPIO_PinSource6, GPIO_AF_SPI1);
	GPIO_PinAFConfig(GPIOA, GPIO_PinSource7, GPIO_AF_SPI1);

	/* spi config */
	SPI_DeInit(SPI1);
	SPI_InitStructure.SPI_Direction = SPI_Direction_2Lines_FullDuplex;
	SPI_InitStructure.SPI_Mode = SPI_Mode_Master;
	SPI_InitStructure.SPI_DataSize = SPI_DataSize_8b;
	SPI_InitStructure.SPI_CPOL = SPI_CPOL_Low;
	SPI_InitStructure.SPI_CPHA = SPI_CPHA_1Edge;
	SPI_InitStructure.SPI_NSS = SPI_NSS_Soft;
	SPI_InitStructure.SPI_FirstBit = SPI_FirstBit_MSB;
	SPI_InitStructure.SPI_BaudRatePrescaler = SPI_BaudRatePrescaler_2;
	SPI_InitStructure.SPI_CRCPolynomial = 7;
	SPI_Init(SPI1, &SPI_InitStructure);

	SPI_I2S_ITConfig(SPI1, SPI_I2S_IT_RXNE, ENABLE);
	SPI_I2S_ITConfig(SPI1, SPI_I2S_IT_TXE, ENABLE);

	/* spi enable */
	SPI_Cmd(SPI1, ENABLE);
}

/******************************************************************************
* external flash io config
*******************************************************************************/
void flash_io_ctrl_init() {
	GPIO_InitTypeDef GPIO_InitStructure;

	RCC_AHBPeriphClockCmd(FLASH_CE_IO_CLOCK, ENABLE);

	GPIO_InitStructure.GPIO_Pin = FLASH_CE_IO_PIN;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_2MHz;
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
	GPIO_Init(FLASH_CE_IO_PORT, &GPIO_InitStructure);
	GPIO_SetBits(FLASH_CE_IO_PORT, FLASH_CE_IO_PIN);
}

void flash_cs_low() {
	GPIO_ResetBits(FLASH_CE_IO_PORT, FLASH_CE_IO_PIN);
}

void flash_cs_high() {
	GPIO_SetBits(FLASH_CE_IO_PORT, FLASH_CE_IO_PIN);
}

uint8_t flash_transfer(uint8_t data) {
	unsigned long transfer_data = data;

	/* waiting send idle then send data */
	while (SPI_I2S_GetFlagStatus(SPI1, SPI_I2S_FLAG_TXE) == RESET);
	SPI_I2S_SendData(SPI1, (uint8_t)transfer_data);

	/* waiting conplete rev data */
	while (SPI_I2S_GetFlagStatus(SPI1, SPI_I2S_FLAG_RXNE) == RESET);
	transfer_data = (uint8_t)SPI_I2S_ReceiveData(SPI1);

	return (uint8_t)transfer_data;
}

/******************************************************************************
* w5500 ethernet io config
*******************************************************************************/
uint8_t w5500_spi_transfer(uint8_t data) {
	unsigned long transfer_data = data;
	uint32_t counter;

	/* waiting send idle then send data */
	counter = system_info.cpu_clock / 1000;
	while (SPI_I2S_GetFlagStatus(SPI1, SPI_I2S_FLAG_TXE) == RESET) {
		if (counter-- == 0) {
			FATAL("spi", 0x01);
		}
	}

	SPI_I2S_SendData(SPI1, (uint8_t)transfer_data);

	/* waiting conplete rev data */
	counter = system_info.cpu_clock / 1000;
	while (SPI_I2S_GetFlagStatus(SPI1, SPI_I2S_FLAG_RXNE) == RESET) {
		if (counter-- == 0) {
			FATAL("spi", 0x02);
		}
	}

	transfer_data = (uint8_t)SPI_I2S_ReceiveData(SPI1);

	return (uint8_t)transfer_data;
}

void w5500_io_ctrl_init() {
	GPIO_InitTypeDef GPIO_InitStructure;

	RCC_AHBPeriphClockCmd(RCC_AHBPeriph_GPIOB, ENABLE);

	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_15;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_40MHz;
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL;
	GPIO_Init(GPIOB, &GPIO_InitStructure);

	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9;
	GPIO_Init(GPIOB, &GPIO_InitStructure);

	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_14;
	GPIO_Init(GPIOB, &GPIO_InitStructure);

	GPIO_SetBits(GPIOB, GPIO_Pin_9);
	GPIO_SetBits(GPIOB, GPIO_Pin_14);
	GPIO_SetBits(GPIOB, GPIO_Pin_15);
}

void w5500_cs_high() {
	GPIO_SetBits(GPIOB, GPIO_Pin_15);
}

void w5500_cs_low() {
	GPIO_ResetBits(GPIOB, GPIO_Pin_15);
}

uint8_t w5500_spi_read_byte() {
	uint32_t counter;
	uint16_t ret;

	/* waiting send idle then send data */
	counter = system_info.cpu_clock / 1000;
	while (SPI_I2S_GetFlagStatus(SPI1, SPI_I2S_FLAG_TXE) == RESET) {
		if (counter-- == 0) {
			FATAL("spi", 0x01);
		}
	}

	/* send dummy byte */
	SPI_I2S_SendData(SPI1, 0);

	/* waiting complete rev data */
	counter = system_info.cpu_clock / 1000;
	while (SPI_I2S_GetFlagStatus(SPI1, SPI_I2S_FLAG_RXNE) == RESET) {
		if (counter-- == 0) {
			FATAL("spi", 0x02);
		}
	}

	ret = (uint8_t)SPI_I2S_ReceiveData(SPI1);

	return ((uint8_t)ret);
}

void w5500_spi_write_byte(uint8_t data) {
	uint16_t send = data;
	uint32_t counter;

	/* waiting send idle then send data */
	counter = system_info.cpu_clock / 1000;
	while (SPI_I2S_GetFlagStatus(SPI1, SPI_I2S_FLAG_TXE) == RESET) {
		if (counter-- == 0) {
			FATAL("spi", 0x01);
		}
	}

	/* send byte */
	SPI_I2S_SendData(SPI1, (uint8_t)send);
	
	/* waiting complete rev data */
	counter = system_info.cpu_clock / 1000;
	while (SPI_I2S_GetFlagStatus(SPI1, SPI_I2S_FLAG_RXNE) == RESET) {
		if (counter-- == 0) {
			FATAL("spi", 0x02);
		}
	}
	SPI_I2S_ReceiveData(SPI1);
}

void io_init() {
	/* led life init */
	led_life_init();

	/* spi init */
	spi_init();

	/* w5500 cs pin init */
	w5500_io_ctrl_init();

	/* flash ce pin init */
	flash_io_ctrl_init();
}
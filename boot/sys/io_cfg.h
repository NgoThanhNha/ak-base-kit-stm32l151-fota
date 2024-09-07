/**
 ******************************************************************************
 * @author: Nark
 * @date:   26/08/2024
 ******************************************************************************
**/

#ifndef __IO_CFG_H__
#define __IO_CFG_H__

#ifdef __cplusplus
 extern "C" {
#endif

#include <string.h>
#include <stdint.h>

/******************************************************************************
* led life io pin map
*******************************************************************************/
#define LED_LIFE_IO_PIN                 (GPIO_Pin_8)
#define LED_LIFE_IO_PORT                (GPIOB)
#define LED_LIFE_IO_CLOCK               (RCC_AHBPeriph_GPIOB)

/*****************************************************************************
* external flash - W25Q256 io ctrl pin map
******************************************************************************/
#define FLASH_CE_IO_PIN					(GPIO_Pin_14)
#define FLASH_CE_IO_PORT				(GPIOB)
#define FLASH_CE_IO_CLOCK				(RCC_AHBPeriph_GPIOB)

/******************************************************************************
* usart console io pin map
*******************************************************************************/
#define CONSOLE_GPIO_PORT               (GPIOA)
#define CONSOLE_GPIO_CLOCK              (RCC_AHBPeriph_GPIOA)
#define CONSOLE_USART_CLOCK             (RCC_APB2Periph_USART1)

#define CONSOLE_TX_PIN                  (GPIO_Pin_9)
#define CONSOLE_RX_PIN                  (GPIO_Pin_10)
#define CONSOLE_TX_PIN_SOURCE           (GPIO_PinSource9)
#define CONSOLE_RX_PIN_SOURCE           (GPIO_PinSource10)

#define CONSOLE_USART                   (USART1)
#define CONSOLE_GPIO_AF                 (GPIO_AF_USART1)
#define CONSOLE_SPEED                   (115200)
#define CONSOLE_INTERRUPT               (USART1_IRQn)

/* led life functions */
extern void led_life_on();
extern void led_life_off();

/* usart functions */
extern void usart1_init(uint32_t baudrate);
extern void usart1_put_char(uint8_t c);
extern uint8_t usart1_get_char();

/* external flash functions */
extern void flash_cs_low();
extern void flash_cs_high();
extern uint8_t flash_transfer(uint8_t);

/* internal flash functions */
extern void internal_flash_unlock();
extern void internal_flash_lock();
extern void internal_flash_erase_pages(uint32_t address, uint32_t len);
extern uint8_t internal_flash_write(uint32_t address, uint8_t* data, uint32_t len);

/* io initialize */
extern void io_init();

#ifdef __cplusplus
}
#endif

#endif /* __IO_CFG_H__ */
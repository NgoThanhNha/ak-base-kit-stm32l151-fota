/**
 ******************************************************************************
 * @author: Nark
 * @date:   26/08/2024
 ******************************************************************************
**/

#include "sys_startup.h"

#include "system_stm32l1xx.h"
#include "stm32l1xx.h"
#include "stm32l1xx_conf.h"
#include "core_cm3.h"

#include "app.h"
#include "io_cfg.h"

#include "sys_cfg.h"
#include "sys_irq.h"

#include "led.h"

/* define the addresses of data and bss sections */
extern uint32_t _sidata, _sdata, _edata, _sbss, _ebss, _estack;
extern void (*__preinit_array_start[])();
extern void (*__preinit_array_end[])();
extern void (*__init_array_start[])();
extern void (*__init_array_end[])();

/* reset handler */
void reset_handler(void) {
	volatile uint32_t *src = &_sidata;
	volatile uint32_t *dest = &_sdata;

	/* clear flag */
	RCC_ClearFlag();

	/* jump to application requested */
	if (sys_ctrl_jump_to_app_req == SYS_CTRL_JUMP_TO_APP_REQ) {
		sys_ctrl_jump_to_app();
	}

    /* call the system init function */
    SystemInit();

	/* copy the data segment initializers from flash to SRAM */
    while (dest < &_edata) {
        *dest++ = *src++;
    }

    /* zero fill to .bss section */
    dest = &_sbss;
    while (dest < &_ebss) {
        *dest++ = 0;
    }

    /*****************************************************************************/
    /* system config
    ******************************************************************************/
    sys_cfg_clock();
    sys_cfg_tick();
    sys_cfg_console();
    
	/* usart1 init */
	usart1_init(CONSOLE_BAUDRATE);

	/* system info update */
	sys_ctrl_update_info();

    /* go to the main application */
    boot_main();
}

/* STM32L151 Cortex-M3 processor exception vector table */
__attribute__((section(".isr_vector"))) void (*const g_pfnVectors[])(void) = {
	/* cpu interrupts */
	((void (*)())(uint32_t)&_estack),		/* the initial stack pointer */
	reset_handler,							/* the reset handler */
	nmi_handler,							/* the NMI handler */
	hard_fault_handler,						/* the hard fault handler */
	mem_manage_handler,						/* the MPU fault handler */
	bus_fault_handler,						/* the bus fault handler */
	usage_fault_handler,					/* the usage fault handler */
	0,										/* reserved */
	0,										/* reserved */
	0,										/* reserved */
	0,										/* reserved */
	svc_handler,							/* SVCall handler */
	dg_monitor_handler,						/* debug monitor handler */
	0,										/* reserved */
	pendsv_handler,							/* the PendSV handler */
	system_tick_handler,					/* the SysTick handler */

	/* external interrupts */
	default_handler,						/* window watchdog */
	default_handler,						/* PVD through EXTI line detect */
	default_handler,						/* tamper and time stamp */
	default_handler,						/* RTC wakeup */
	default_handler,						/* flash */
	default_handler,						/* RCC */
	default_handler,						/* EXTI line 0 */
	default_handler,						/* EXTI line 1 */
	default_handler,						/* EXTI line 2 */
	default_handler,						/* EXTI line 3 */
	default_handler,						/* EXTI line 4 */
	default_handler,						/* DMA1 channel 1 */
	default_handler,						/* DMA1 channel 2 */
	default_handler,						/* DMA1 channel 3 */
	default_handler,						/* DMA1 channel 4 */
	default_handler,						/* DMA1 channel 5 */
	default_handler,						/* DMA1 channel 6 */
	default_handler,						/* DMA1 channel 7 */
	default_handler,						/* ADC1 */
	default_handler,						/* USB high priority */
	default_handler,						/* USB low priority */
	default_handler,						/* DAC */
	default_handler,						/* COMP through EXTI line */
	default_handler,						/* EXTI line 9..5 */
	default_handler,						/* LCD */
	default_handler,						/* TIM9 */
	default_handler,						/* TIM10 */
	default_handler,						/* TIM11 */
	default_handler,						/* TIM2 */
	default_handler,						/* TIM3 */
	default_handler,						/* TIM4 */
	default_handler,						/* I2C1 event */
	default_handler,						/* I2C1 error */
	default_handler,						/* I2C2 event */
	default_handler,						/* I2C2 error */
	default_handler,						/* SPI1 */
	default_handler,						/* SPI2 */
	default_handler,						/* USART1 */
	default_handler,					    /* USART2 */
	default_handler,						/* USART3 */
	default_handler,						/* EXTI line 15..10 */
	default_handler,						/* RTC alarm through EXTI line */
	default_handler,						/* USB FS wakeup from suspend */
	default_handler,						/* TIM6 */
	default_handler,					    /* TIM7 */
};
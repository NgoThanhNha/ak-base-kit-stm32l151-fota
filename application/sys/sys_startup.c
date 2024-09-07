/**
 ******************************************************************************
 * @author: Nark
 * @date:   21/08/2024
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
#include "platform.h"

#include "led.h"

/* define the addresses of data and bss sections */
extern uint32_t _sidata, _sdata, _edata, _sbss, _ebss, _estack, _isr_vector;
extern void (*__preinit_array_start[])();
extern void (*__preinit_array_end[])();
extern void (*__init_array_start[])();
extern void (*__init_array_end[])();

/* reset handler */
void reset_handler(void) {
	/* MUST BE disable interrupt */
	__disable_irq();

    volatile uint32_t *src = &_sidata;
	volatile uint32_t *dest = &_sdata;
	volatile unsigned i, cnt;

	/* system init */
    SystemInit();

	/* copy the data segment initializers from flash to SRAM */
    while (dest < &_edata) {
        *dest++ = *src++;
    }

    /* zero fill to .bss section */
    dest = &_sbss;
    while (dest < &_ebss) {
        *dest++ = 0UL;
    }

    /*****************************************************************************/
    /* re-assign vector interrupt table address
    ******************************************************************************/
   	/* in this app - vector interrput table address is 0x8003000 */
    SCB->VTOR = (uint32_t)&_isr_vector;

	/* fill stack section */
    sys_stack_fill();

	ENTRY_CRITICAL();

    /*****************************************************************************/
    /* system config
    ******************************************************************************/
	sys_cfg_clock(); /* init system clock */
	sys_cfg_svc(); /* setting svc exception priority */
	sys_cfg_pendsv(); /* setting psv exception priority */
	sys_cfg_tick(); /* system tick */
	sys_cfg_console(); /* system console */
    
	/* invoke all static constructors */
	cnt = __preinit_array_end - __preinit_array_start;
	for (i = 0; i < cnt; i++)
		__preinit_array_start[i]();

	cnt = __init_array_end - __init_array_start;
	for (i = 0; i < cnt; i++)
		__init_array_start[i]();
    
	sys_ctrl_delay_ms(1);

	/* usart1 init */
	usart1_init(CONSOLE_BAUDRATE);

    /* io init */
    io_init();

    /* system update info */
    sys_ctrl_show_info();

    /* go to the main application */
    main_app();
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
	usart1_irq_handler,						/* USART1 */
	default_handler,					    /* USART2 */
	default_handler,						/* USART3 */
	default_handler,						/* EXTI line 15..10 */
	default_handler,						/* RTC alarm through EXTI line */
	default_handler,						/* USB FS wakeup from suspend */
	default_handler,						/* TIM6 */
	default_handler,					    /* TIM7 */
};
/**
 ******************************************************************************
 * @author: Nark
 * @date:   26/08/2024
 ******************************************************************************
**/

#include "sys_cfg.h"

#include "stm32l1xx.h"
#include "stm32l1xx_conf.h"
#include "system_stm32l1xx.h"
#include "core_cm3.h"
#include "core_cmFunc.h"

#include "sys_irq.h"
#include "io_cfg.h"

/*****************************************************************************/
/* linker variable
******************************************************************************/
extern uint32_t _start_flash;
extern uint32_t _end_flash;
extern uint32_t _start_ram;
extern uint32_t _end_ram;
extern uint32_t _sdata;
extern uint32_t _edata;
extern uint32_t _sbss;
extern uint32_t _ebss;
extern uint32_t __heap_start__;
extern uint32_t __heap_end__;
extern uint32_t _sstack;
extern uint32_t _estack;

/******************************************************************************
* system configurating functions
*******************************************************************************/
void sys_cfg_clock() {
    /* enable HSE (High Speed External) clock */
    RCC_HSEConfig(RCC_HSE_ON);
    
    /* wait till HSE is ready */
    while (RCC_GetFlagStatus(RCC_FLAG_HSERDY) == RESET);

    /* configure the PLL (Phase Locked Loop) */
    /* HSE = 8MHz, PLL = 32MHz (8 * 12 / 3) */
    RCC_PLLConfig(RCC_PLLSource_HSE, RCC_PLLMul_12, RCC_PLLDiv_3); 
    RCC_PLLCmd(ENABLE);

    /* wait till PLL is ready */
    while (RCC_GetFlagStatus(RCC_FLAG_PLLRDY) == RESET);

    /* select PLL as system clock source */
    RCC_SYSCLKConfig(RCC_SYSCLKSource_PLLCLK);

    /* wait till PLL is used as system clock source */
    while (RCC_GetSYSCLKSource() != 0x0C);

    /* configure the HCLK, PCLK1, and PCLK2 clocks dividers */
    RCC_HCLKConfig(RCC_SYSCLK_Div1);   /* HCLK = SYSCLK = 32 MHz */
    RCC_PCLK1Config(RCC_HCLK_Div2);    /* PCLK1 = HCLK/2 = 16 MHz */
    RCC_PCLK2Config(RCC_HCLK_Div1);    /* PCLK2 = HCLK = 32 MHz */

    /* enable the Clock Security System (CSS) */
    RCC_ClockSecuritySystemCmd(ENABLE);
}

void sys_cfg_tick() {
    SystemCoreClockUpdate();
    if (SysTick_Config(SystemCoreClock / 1000)) { 
        while (1);
    }
    SysTick->CTRL |= SysTick_CTRL_CLKSOURCE_Msk;
}

void sys_cfg_console() {
    xfunc_output = (void(*)(int))usart1_put_char;
}

/******************************************************************************
* system info function
*******************************************************************************/
system_info_t system_info;

void sys_ctrl_update_info() {

	RCC_ClocksTypeDef RCC_Clocks;

	RCC_GetClocksFreq(&RCC_Clocks);

	system_info.cpu_clock = RCC_Clocks.HCLK_Frequency;
	system_info.tick      = 1;
	system_info.console_baudrate = CONSOLE_BAUDRATE;
	system_info.flash_used = ((uint32_t)&_end_flash - (uint32_t)&_start_flash) + ((uint32_t)&_edata - (uint32_t)&_sdata);
	system_info.ram_used = (uint32_t)&_estack - (uint32_t)&_start_ram;
	system_info.data_init_size = (uint32_t)&_edata - (uint32_t)&_sdata;
	system_info.data_non_init_size = (uint32_t)&_ebss - (uint32_t)&_sbss;
	system_info.stack_avail = (uint32_t)&_estack - (uint32_t)&_end_ram;
	system_info.heap_avail = (uint32_t)&__heap_end__ - (uint32_t)&__heap_start__;
	system_info.ram_other = system_info.ram_used - (system_info.heap_avail + system_info.stack_avail + system_info.data_non_init_size + system_info.data_init_size);
}

/******************************************************************************
* system utility functions
*******************************************************************************/
uint32_t sys_ctrl_jump_to_app_req;

void sys_ctrl_reset() {
	NVIC_SystemReset();
}

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wdeprecated"
void sys_ctrl_jump_to_app() {
	volatile uint32_t normal_stack_pointer = (uint32_t) *(volatile uint32_t*)(APP_START_ADDR);
	volatile uint32_t app_address = (uint32_t) *(volatile uint32_t*)(APP_START_ADDR + 4);
    sys_ctrl_jump_to_app_req = 0x1A2B3C4D;

	p_jump_func jump_to_app = (p_jump_func)app_address;
 
	/* disable interrupt */
	__disable_irq();

	/* set stack pointer */
	__DMB();
	__asm volatile ("MSR msp, %0\n" : : "r" (normal_stack_pointer) : "sp");
	__DSB();

	/* jump to application */
	jump_to_app();
}
#pragma GCC diagnostic pop

void sys_ctrl_delay_ms(volatile uint32_t count) {
    volatile uint32_t current_ticks = SysTick->VAL;

	/* number of ticks per millisecond */
	const uint32_t tick_per_ms = SysTick->LOAD + 1;

	/* number of ticks need to count */
	const uint32_t number_of_tick = ((count - ((count > 0) ? 1 : 0)) * tick_per_ms);

	/* number of elapsed ticks */
	uint32_t elapsed_tick = 0;
	volatile uint32_t prev_tick = current_ticks;
	while (number_of_tick > elapsed_tick) {
		current_ticks = SysTick->VAL;
		elapsed_tick += (prev_tick < current_ticks) ? (tick_per_ms + prev_tick - current_ticks) : (prev_tick - current_ticks);
		prev_tick = current_ticks;
	}
}
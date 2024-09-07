/**
 ******************************************************************************
 * @author: Nark
 * @date:   26/08/2024
 ******************************************************************************
**/

#include "stm32l1xx.h"
#include "stm32l1xx_tim.h"
#include "stm32l1xx_exti.h"
#include "stm32l1xx_usart.h"

#include "io_cfg.h"
#include "sys_irq.h"

#include "app.h"

/* counter of milliseconds since the application run */
volatile uint32_t millis_current;

void default_handler() {
	
}

void nmi_handler() {
	
}

void hard_fault_handler() {
	
}

void mem_manage_handler() {
	
}

void bus_fault_handler() {
	
}

void usage_fault_handler() {
	
}

void dg_monitor_handler() {
    default_handler();
}

void pendsv_handler() {
    default_handler();
}

void svc_handler() {
    default_handler();
}

void system_tick_handler() {
	static uint32_t div_counter = 0;

	/* increasing millis counter */
	millis_current++;

	if (div_counter == 10) {
		div_counter = 0;
	}

	switch (div_counter) {
	case 0:
		sys_irq_timer_10ms();
		break;

	default:
		break;
	}

	div_counter++;
}
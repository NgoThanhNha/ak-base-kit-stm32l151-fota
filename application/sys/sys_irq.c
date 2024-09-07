/**
 ******************************************************************************
 * @author: Nark
 * @date:   21/08/2024
 ******************************************************************************
**/

#include "stm32l1xx.h"
#include "stm32l1xx_tim.h"
#include "stm32l1xx_exti.h"
#include "stm32l1xx_usart.h"

#include "task.h"
#include "message.h"
#include "timer.h"

#include "io_cfg.h"
#include "sys_irq.h"
#include "platform.h"

#include "app.h"
#include "task_shell.h"

#include "mqtt_port.h"

/* counter of milliseconds since the application run */
volatile uint32_t millis_current;

void default_handler() {
	FATAL("SYSTEM", 0x01);
}

void nmi_handler() {
	FATAL("SYSTEM", 0x02);
}

void hard_fault_handler() {
	FATAL("SYSTEM", 0x03);
}

void mem_manage_handler() {
	FATAL("SYSTEM", 0x04);
}

void bus_fault_handler() {
	FATAL("SYSTEM", 0x05);
}

void usage_fault_handler() {
	FATAL("SYSTEM", 0x06);
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

	/* kernel tick */
	timer_tick(1);

	/* mqtt tick */
	mqtt_tick();

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

uint32_t sys_ctrl_millis() {
	volatile uint32_t ret;
	ENTRY_CRITICAL();
	ret = millis_current;
	EXIT_CRITICAL();
	return ret;
}

void usart1_irq_handler() {
	sys_irq_shell();
}
/**
 ******************************************************************************
 * @author: Nark
 * @date:   02/05/2024
 ******************************************************************************
**/

#include "platform.h"

#include "stm32l1xx.h"
#include "core_cm3.h"
#include "core_cmFunc.h"

#include "io_cfg.h"
#include "sys_cfg.h"

void enable_interrupts() {
	__enable_irq();
}

void disable_interrupts() {
	__disable_irq();
}

void fatal_error(const char* fatal_info, uint8_t fatal_id) {
    #if defined (RELEASE_EN)
        sys_ctrl_reset();
    #else
        disable_interrupts();
        SYS_PRINT("\n");
        SYS_PRINT("************************ FATAL_ERROR ************************\n");
        SYS_PRINT("[FATAL_TYPE]: %s -> [FATAL_ID]: 0x%02X\n", fatal_info, fatal_id);
        SYS_PRINT("\n");
        while (1) {
            led_life_on();
            sys_ctrl_delay_ms(50);
            led_life_off();
            sys_ctrl_delay_ms(50); 
        }
    #endif
}
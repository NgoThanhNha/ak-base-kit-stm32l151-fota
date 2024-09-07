/**
 ******************************************************************************
 * @author: Nark
 * @date:   26/08/2024
 ******************************************************************************
**/

#ifndef __SYS_CFG_H__
#define __SYS_CFG_H__

#ifdef __cplusplus
 extern "C" {
#endif 

#include <stdint.h>
#include "xprintf.h"

typedef void (*p_jump_func)(void);

#define APP_START_ADDR 						(0x08005000)
#define SYS_CTRL_JUMP_TO_APP_REQ			((uint32_t)0xBEEBC0DE)

#define CONSOLE_BAUDRATE                    (115200)
#define SYS_PRINT(fmt, ...)                 xprintf((const char*)fmt, ##__VA_ARGS__)

typedef struct {
	uint32_t cpu_clock;
	uint32_t tick;
	uint32_t console_baudrate;
	uint32_t flash_used;
	uint32_t ram_used;
	uint32_t data_init_size;
	uint32_t data_non_init_size;
	uint32_t stack_avail;
	uint32_t heap_avail;
	uint32_t ram_other;
} system_info_t;

extern system_info_t system_info;

/******************************************************************************
* system boot functions
*******************************************************************************/
extern uint32_t sys_ctrl_jump_to_app_req;
extern void sys_ctrl_jump_to_app();

/******************************************************************************
* system configure functions
*******************************************************************************/
extern void sys_cfg_clock();
extern void sys_cfg_tick();
extern void sys_cfg_console();

/******************************************************************************
* system info function
*******************************************************************************/
extern void sys_ctrl_update_info();

/******************************************************************************
* system utility functions
*******************************************************************************/
extern void sys_ctrl_delay_ms(volatile uint32_t count);
extern void sys_ctrl_reset();

#ifdef __cplusplus
}
#endif

#endif /* __SYS_CFG_H__ */
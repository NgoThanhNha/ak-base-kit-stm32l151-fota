/**
 ******************************************************************************
 * @author: Nark
 * @date:   26/07/2024
 ******************************************************************************
**/

#ifndef __TASK_SHELL_H__
#define __TASK_SHELL_H__

#ifdef __cplusplus
 extern "C" {
#endif 

#include <stdint.h>
#include "task.h"

#include "ring_buffer.h"
#include "shell.h"

/* for get char from usart irq */
extern uint8_t buffer_console_rev[BUFFER_CONSOLE_REV_SIZE];
extern ring_buffer_char_t ring_buffer_console_rev;

extern void sys_irq_shell();
extern void task_polling_console();
extern void task_shell_handler(stk_msg_t* msg);

#ifdef __cplusplus
}
#endif

#endif /* __TASK_SHELL_H__ */
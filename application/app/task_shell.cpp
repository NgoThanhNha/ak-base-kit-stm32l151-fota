/**
 ******************************************************************************
 * @author: Nark
 * @date:   26/07/2024
 ******************************************************************************
**/

#include "task_shell.h"

#include "io_cfg.h"

#include "platform.h"
#include "cmd_line.h"

#include "app.h"
#include "app_dbg.h"
#include "task_list.h"

/* for get char */
uint8_t buffer_console_rev[BUFFER_CONSOLE_REV_SIZE];
ring_buffer_char_t ring_buffer_console_rev;

/* for parser cmd */
uint8_t shell_data_parser[SHELL_BUFFER_LENGTH];
volatile shell_t shell;

void sys_irq_shell() {
	volatile uint8_t c = 0;

	c = usart1_get_char();

	ENTRY_CRITICAL();
	ring_buffer_char_put(&ring_buffer_console_rev, c);
	EXIT_CRITICAL();
}

void task_shell_handler(stk_msg_t* msg) {

    uint8_t first_char = shell_data_parser[0];
    
    switch (msg->sig) {
    case AC_SHELL_CMD_PARSER:
        break;

    default:
        break;
    }

    switch (cmd_line_parser((cmd_line_t*)shell_table, (uint8_t*)&shell_data_parser[0])) {
    case CMD_SUCCESS:
        break;

    case CMD_NOT_FOUND:
        if (first_char != '\r' && first_char != '\n') {
            SHELL_LOG("cmd unknown\n");
        }
        break;

    case CMD_TOO_LONG:
        SHELL_LOG("cmd too long\n");
        break;

    case CMD_TABLE_NOT_FOUND:
        SHELL_LOG("cmd table not found\n");
        break;

    default:
        SHELL_LOG("cmd error\n");
        break;
    }
    SHELL_LOG("#");
}

void task_polling_console() {
    volatile uint8_t c = 0;

    while (ring_buffer_char_is_empty(&ring_buffer_console_rev) == false) {

        ENTRY_CRITICAL();
        c = ring_buffer_char_get(&ring_buffer_console_rev);
        EXIT_CRITICAL();
        if (shell.index < SHELL_BUFFER_LENGTH - 1) {

            if (c == '\r' || c == '\n') { /* linefeed */

                xputc('\r');
                xputc('\n');

                shell.data[shell.index] = c;
                shell.data[shell.index + 1] = 0;
                memcpy((uint8_t*)&shell_data_parser[0], (uint8_t*)&shell.data[0], shell.index + 2);
                task_post_pure_msg(TASK_SHELL_ID, AC_SHELL_CMD_PARSER);

                shell.index = 0;
            }
            else {

                xputc(c);

                if (c == 8 && shell.index) { /* backspace */
                    shell.index--;
                }
                else {
                    shell.data[shell.index++] = c;
                }
            }
        }
        else {
            SHELL_LOG("\nerror: cmd too long, cmd size: %d, try again !\n", SHELL_BUFFER_LENGTH);
            shell.index = 0;
        }
    }
}
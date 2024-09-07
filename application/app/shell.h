/**
 ******************************************************************************
 * @author: Nark
 * @date:   26/07/2024
 ******************************************************************************
**/

#ifndef __SHELL_H__
#define __SHELL_H__

#ifdef __cplusplus
 extern "C" {
#endif 

#include <stdint.h>

#include "cmd_line.h"
#include "app_dbg.h"

#define SHELL_BUFFER_LENGTH				(32)
#define BUFFER_CONSOLE_REV_SIZE			(256)

#if defined (CONSOLE_DBG_EN)
    #define SHELL_LOG(fmt, ...)         xprintf(fmt, ##__VA_ARGS__)
#endif

typedef struct {
	uint8_t index;
	uint8_t data[SHELL_BUFFER_LENGTH];
} shell_t;

/* table contain commands */
extern cmd_line_t shell_table[];

#ifdef __cplusplus
}
#endif

#endif /* __SHELL_H__ */
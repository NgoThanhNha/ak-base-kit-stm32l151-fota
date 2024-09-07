/**
 ******************************************************************************
 * @author: Nark
 * @date:   21/08/2024
 ******************************************************************************
**/

#ifndef __APP_DBG_H__
#define __APP_DBG_H__

#ifdef __cplusplus
 extern "C" {
#endif 

#include <stdint.h>

#include "io_cfg.h"
#include "xprintf.h"

#define CONSOLE_DBG_EN

#if defined (CONSOLE_DBG_EN)
#define APP_PRINT(fmt, ...)         xprintf("[APP] " fmt, ##__VA_ARGS__)
#define APP_DBG(fmt, ...)           xprintf("[DBG] " fmt, ##__VA_ARGS__)
#define APP_DBG_SIG(fmt, ...)       xprintf("-SIG-> " fmt, ##__VA_ARGS__)
#else
#define APP_PRINT(fmt, ...)              
#define APP_DBG(fmt, ...)         
#define APP_DBG_SIG(fmt, ...)  
#endif

#ifdef __cplusplus
}
#endif

#endif /* __APP_DBG_H__ */
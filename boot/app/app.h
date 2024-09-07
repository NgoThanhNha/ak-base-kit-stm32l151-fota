/**
 ******************************************************************************
 * @author: Nark
 * @date:   26/08/2024
 ******************************************************************************
**/

#ifndef __APP_H__
#define __APP_H__

#ifdef __cplusplus
 extern "C" {
#endif

#include <stdio.h>

#define BOOT_VER                                "1.0"
#define APP_START_ADDR_EXTERNAL_FLASH           (0x00000000)
#define PACKET_SIZE_COPY                        (128)

/* main application */
extern int boot_main();

/* polling with system tick */
extern void sys_irq_timer_10ms();

#ifdef __cplusplus
}
#endif

#endif /* __APP_H__ */
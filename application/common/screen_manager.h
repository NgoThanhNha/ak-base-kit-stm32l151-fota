/**
 ******************************************************************************
 * @author: Nark
 * @date:   02/04/2024
 ******************************************************************************
**/

#ifndef __SCREEN_MANAGER_H__
#define __SCREEN_MANAGER_H__

#ifdef __cplusplus
 extern "C" {
#endif 

#include <stdint.h>
#include "message.h"

#define SCREEN_ENTRY                    (0xFE)
#define SCREEN_MANAGER_NULL             ((scr_mng_t*)0)

#define SCREEN_TRANS(screen_target)     scr_mng_trans(screen_target)

typedef void (*pf_screen_handler)(stk_msg_t*);

typedef struct {
    pf_screen_handler current_screen_display;
} scr_mng_t;

extern void scr_mng_init(scr_mng_t* screen, pf_screen_handler init_screen);
extern void screen_mng_dispatch(stk_msg_t* msg);
extern void scr_mng_trans(pf_screen_handler target_screen);

#ifdef __cplusplus
}
#endif

#endif /* __SCREEN_MANAGER_H__ */
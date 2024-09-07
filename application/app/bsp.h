/**
 ******************************************************************************
 * @author: Nark
 * @date:   21/08/2024
 ******************************************************************************
**/

#ifndef __BSP_H__
#define __BSP_H__

#ifdef __cplusplus
 extern "C" {
#endif 

#include <stdint.h>
#include "button.h"

extern button_t button_mode;
extern button_t button_up;
extern button_t button_down;

extern void button_mode_callback(void* _button);
extern void button_up_callback(void* _button);
extern void button_down_callback(void* _button);

#ifdef __cplusplus
}
#endif

#endif /* __BSP_H__ */
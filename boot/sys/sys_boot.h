/**
 ******************************************************************************
 * @author: Nark
 * @date:   28/08/2024
 ******************************************************************************
**/

#ifndef __SYS_BOOT_H__
#define __SYS_BOOT_H__

#ifdef __cplusplus
 extern "C" {
#endif

#include <string.h>
#include <stdint.h>
#include <stdio.h>

#include "stm32l1xx_flash.h"

#define SYS_BOOT_OK							(0x00)
#define SYS_BOOT_NG							(0x01)

#define FIRMWARE_PSK						(0xBEEBC0DE) /* pre-shared key, this is the magic number */
#define SYS_FW_CMD_NONE						(0x01)
#define SYS_FW_CMD_UPDATE_REQ				(0x02)

#define SYS_FW_CONTAINER_NONE				(0x01)
#define SYS_FW_CONTAINER_DIRECTLY			(0x02)
#define SYS_FW_CONTAINER_EXTERNAL_FLASH		(0x03)
#define SYS_FW_CONTAINER_INTERNAL_FLASH		(0x04)
#define SYS_FW_CONTAINER_EPPROM				(0x05)
#define SYS_FW_CONTAINER_SD_CARD			(0x06)

#define SYS_FW_IO_DRIVER_NONE				(0x01)
#define SYS_FW_IO_DRIVER_UART				(0x02)
#define SYS_FW_IO_DRIVER_SPI				(0x03)

typedef struct {
	uint32_t psk;
	uint32_t bin_len;
	uint32_t checksum;
} firmware_header_t;

typedef struct {
	uint8_t cmd; /* none, update request */
	uint8_t container;
	uint8_t io_driver;
} firmware_cmd_t;

typedef struct {
    /* current firmwre header, that field will be update when system start */
    firmware_header_t current_fw_boot_header;
	firmware_header_t current_fw_app_header;

    /* new firmwre header, that field contain new firmware header */
    firmware_header_t update_fw_boot_header;
	firmware_header_t update_fw_app_header;

	/* firmware's boot command */
	firmware_cmd_t fw_boot_cmd;

	/* firmware's app command */
	firmware_cmd_t fw_app_cmd;
} sys_boot_t;

extern void sys_boot_init();
extern void sys_boot_get(sys_boot_t*);
extern uint8_t sys_boot_set(sys_boot_t*);

#ifdef __cplusplus
}
#endif

#endif /* __SYS_BOOT_H__ */
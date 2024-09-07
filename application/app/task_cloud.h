/**
 ******************************************************************************
 * @author: Nark
 * @date:   23/08/2024
 ******************************************************************************
**/

#ifndef __TASK_CLOUD_H__
#define __TASK_CLOUD_H__

#ifdef __cplusplus
 extern "C" {
#endif

#include <stdint.h>
#include <stdio.h>

#include "message.h"
#include "xprintf.h"

#define CLOUD_LOG_EN

#if defined (CLOUD_LOG_EN)
#define CLOUD_LOG(fmt, ...)                 xprintf((const char*)fmt, ##__VA_ARGS__)
#else
#define CLOUD_LOG(fmt, ...)
#endif

#define MQTT_NG                             (0x00)
#define MQTT_OK                             (0x01)

#define MQTT_BROKER_DOMAIN                  "demo.thingsboard.io"

/* topic for publish sample data */
#define MQTT_PUBLISH_PAYLOAD                "{temperature:25}"
#define MQTT_PUBLISH_TOPIC                  "v1/devices/me/telemetry"

/* topic for request fota attributes */
#define MQTT_FOTA_PUB_TOPIC_ATTR_REQ        "v1/devices/me/attributes/request/1"
#define MQTT_FOTA_SUB_TOPIC_ATTR_RES        "v1/devices/me/attributes/response/+"
#define MQTT_FOTA_ATTR_REQ_MSG              "{\"sharedKeys\":\"fw_title,fw_version,fw_size,fw_checksum\"}"

/* topic for receive payload fota */
#define MQTT_FOTA_SUB_TOPIC_REV_PAYLOAD     "v2/fw/response/+/chunk/+"

#define CHUNK_SIZE                          (256) /* 256 bytes for each packet download */
#define CHUNK_REQ_COUNTER_MAX               (3)

typedef struct {
    uint32_t bin_len; /* firmware size */
    uint32_t check_sum; /* check sum with crc32 algorithm */
    uint16_t chunk_id; /* packet id firmware download */
    uint8_t chunk_retry_req_counter; /* retry for download packet */
    uint32_t index_write_in_flash; /* index for writing fw to external flash */
    uint32_t fw_remain; /* firmware remain after writen to flash */
    uint16_t fw_packet_len; /* packet len for writing to external flash */
    uint8_t payload[256]; /* payload reiceived after chunk request */
} fota_frame_t;

extern fota_frame_t fota_frame_rev;
extern void network_mqtt_fota_print_progress(uint8_t percent);
extern void task_polling_mqtt();
extern void task_cloud_handler(stk_msg_t* msg);

#ifdef __cplusplus
}
#endif

#endif /* __TASK_CLOUD_H__ */
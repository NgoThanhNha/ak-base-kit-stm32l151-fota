/**
 ******************************************************************************
 * @author: Nark
 * @date:   23/08/2024
 ******************************************************************************
**/

#include "task_cloud.h"

#include "task.h"
#include "message.h"
#include "timer.h"

#include "io_cfg.h"
#include "led.h"
#include "utils.h"

#include "app.h"
#include "app_dbg.h"
#include "task_list.h"

/* network includes */
#include "w5500.h"
#include "w5500_port.h"
#include "MQTTClient.h"
#include "mqtt_port.h"
#include "dns.h"
#include "dhcp.h"

/* libraries includes */
#include "ArduinoJson.h"

/* net objects */
Network network;
MQTTClient client;

/* net attr */
wiz_NetInfo net_info = {.mac = {0x00, 0x08, 0xED, 0x01, 0x02, 0x03}};
static uint8_t network_ret;         
static uint8_t ethernet_buffer[2048];
static uint8_t dhcp_status = 0;

/* mqtt attr */
uint8_t target_ip[4] = {0, 0, 0, 0};
unsigned int target_port = 1883;
unsigned char mqtt_send_buffer[2048];
unsigned char mqtt_rev_buffer[2048];
MQTTMessage mqtt_pub_msg;
StaticJsonDocument<200> json_publish_to_mqtt;
char json_buffer[512];

/* network functions */
static void network_hw_init();
static void network_set_net_info(wiz_NetInfo net_info);
static void network_get_net_info();
static void network_dhcp_init();
static void network_dhcp_assign();
static void network_dhcp_conflict();

/* mqtt functions */
static uint8_t network_mqtt_init_status = MQTT_NG;
static uint8_t network_mqtt_init();
static void network_mqtt_publish();

/* fota functions */
fota_frame_t fota_frame_rev;
static void network_mqtt_subcribe_fota_rev_attr(MessageData *msg_rev);
static void network_mqtt_subcribe_fota_rev_payload(MessageData *msg_rev);
static void network_mqtt_fota_request_attr();
static int8_t network_mqtt_fota_request_chunk(uint32_t chunk_id);

void task_cloud_handler(stk_msg_t* msg) {
    switch (msg->sig) {
    case AC_NET_HW_INIT:
        APP_PRINT("[task_cloud] AC_NET_INIT\n");
        network_hw_init();
        network_dhcp_init();
        task_post_pure_msg(TASK_CLOUD_ID, AC_NET_DHCP_INIT);
        break;
    
    case AC_NET_DHCP_INIT:
        dhcp_status = DHCP_run();
        if (dhcp_status == DHCP_IP_LEASED) {
            timer_remove(TASK_CLOUD_ID, AC_NET_DHCP_INIT);
        }
        else {
            timer_set(TASK_CLOUD_ID, AC_NET_DHCP_INIT, 500, TIMER_PERIODIC);
        }
        break;

    case AC_NET_GET_INFO:
        APP_PRINT("[task_cloud] AC_NET_GET_INFO\n");
        network_get_net_info();
        task_post_pure_msg(TASK_CLOUD_ID, AC_MQTT_INIT);
        break;

    case AC_MQTT_INIT:
        APP_PRINT("[task_cloud] AC_MQTT_INIT\n");
        network_ret = network_mqtt_init();
        if (network_ret == MQTT_OK) {
            network_mqtt_init_status = MQTT_OK;
        }
        else {
            network_mqtt_init_status = MQTT_NG;
        }
        break;

    case AC_MQTT_PUB:
        APP_PRINT("[task_cloud] AC_MQTT_PUB\n");
        network_mqtt_publish();
        break;

    case AC_MQTT_FOTA_REQ_ATTR:
        APP_PRINT("[task_cloud] AC_MQTT_FOTA_REQ\n");
        network_mqtt_fota_request_attr();
        break;

    case AC_MQTT_FOTA_REQ_CHUNK:
        if (network_mqtt_fota_request_chunk(fota_frame_rev.chunk_id) == MQTT_OK) {
            /* TODO */
        }
        else {
            /* TODO */
        }
        break;

    default:
        break;
    }
}

/*****************************************************************************/
/* network functions 
******************************************************************************/
void network_hw_init() {
    uint8_t tx_sizes[_WIZCHIP_SOCK_NUM_] = {2, 2, 2, 2}; /* example transmit buffer sizes */
    uint8_t rx_sizes[_WIZCHIP_SOCK_NUM_] = {2, 2, 2, 2}; /* example receive buffer sizes */

    /* w5500 hardware init */
    reg_wizchip_cs_cbfunc(w5500_cs_low, w5500_cs_high);
    reg_wizchip_spi_cbfunc(w5500_spi_read_byte, w5500_spi_write_byte);
    wizchip_init(tx_sizes, rx_sizes);
}

void network_get_net_info() {
    wiz_NetInfo get_net_info;

    /* get the current network information from the W5500 module */
    wizchip_getnetinfo(&get_net_info);

    CLOUD_LOG("\n");
    CLOUD_LOG("====================\n");
    CLOUD_LOG(" network info\n");
    CLOUD_LOG("====================\n");
    CLOUD_LOG("[mac address]: %02X:%02X:%02X:%02X:%02X:%02X\r\n"
            "[ip address]: %d.%d.%d.%d\r\n"
            "[subnet mask]: %d.%d.%d.%d\r\n"
            "[gateway]: %d.%d.%d.%d\r\n"
            "[dns server]: %d.%d.%d.%d\r\n",
            get_net_info.mac[0], get_net_info.mac[1], get_net_info.mac[2],
            get_net_info.mac[3], get_net_info.mac[4], get_net_info.mac[5],
            get_net_info.ip[0], get_net_info.ip[1], get_net_info.ip[2], get_net_info.ip[3],
            get_net_info.sn[0], get_net_info.sn[1], get_net_info.sn[2], get_net_info.sn[3],
            get_net_info.gw[0], get_net_info.gw[1], get_net_info.gw[2], get_net_info.gw[3],
            get_net_info.dns[0], get_net_info.dns[1], get_net_info.dns[2], get_net_info.dns[3]);
    CLOUD_LOG("\n");
}

void network_set_net_info(wiz_NetInfo net_info) {
    wizchip_setnetinfo(&net_info);
}

void network_dhcp_init() {
    CLOUD_LOG("DHCP client running !\n");
    /* dhcp init */
    DHCP_init(0, ethernet_buffer);

    /* init dhcp control block */
    reg_dhcp_cbfunc(network_dhcp_assign, network_dhcp_assign, network_dhcp_conflict);
}

void network_dhcp_assign() {
    getIPfromDHCP(net_info.ip);
    getGWfromDHCP(net_info.gw);
    getSNfromDHCP(net_info.sn);
    getDNSfromDHCP(net_info.dns);

    net_info.dhcp = NETINFO_DHCP;

    /* network initialize */
    network_set_net_info(net_info);
    CLOUD_LOG("DHCP configure successfully !\n");
    CLOUD_LOG("DHCP leased time: %ld seconds\n", getDHCPLeasetime());
    task_post_pure_msg(TASK_CLOUD_ID, AC_NET_GET_INFO);
}

void network_dhcp_conflict() {
    CLOUD_LOG("DHCP conflict !\n");
    CLOUD_LOG("network is not initialized !\n");
}

/*****************************************************************************/
/* mqtt functions 
******************************************************************************/
uint8_t network_mqtt_init() {
	network.my_socket = 1;
	dns_init(1, ethernet_buffer);
	if (dns_run(net_info.dns, (uint8_t*)MQTT_BROKER_DOMAIN, (uint8_t*)&target_ip) == 0) {
        APP_PRINT("[task_cloud] DNS failed !\n");
        return MQTT_NG;
    }
    APP_PRINT("[task_cloud] IP received from DNS server: %d.%d.%d.%d\n", target_ip[0], target_ip[1], target_ip[2], target_ip[3]);
    APP_PRINT("[task_cloud] connecting to network...\n");

    /* open socket */
    mqtt_new_network(&network, 0);
    int result = mqtt_connect_network(&network, (uint8_t*)&target_ip, 1883);
    if (result != 1) {
        APP_PRINT("[task_cloud] failed to connect to network: %d\n", result);
        return MQTT_NG;
    }
    else {
        APP_PRINT("[task_cloud] connected to the network !\n");
    }

    /* mqtt init */
	mqtt_client_init(&client ,&network, 5000, mqtt_send_buffer, 2048, mqtt_rev_buffer, 2048);
    
    /* set mqtt attributes */
    APP_PRINT("[task_cloud] connecting to the mqtt broker...\n");
    MQTTPacket_connectData mqtt_attr = MQTTPacket_connectData_initializer;
	mqtt_attr.willFlag = 0;
	mqtt_attr.MQTTVersion = 4;
    mqtt_attr.cleansession = 1;
	mqtt_attr.clientID.cstring = (char*)&"a";
	mqtt_attr.username.cstring = (char*)&"0QZU9y1nujkVRVEAxCwQ"; /* access token */
	mqtt_attr.password.cstring = (char*)&" ";
	mqtt_attr.keepAliveInterval = 60;

    /* connect to the mqtt broker */
    int rc = 0;
    rc = mqtt_connect(&client, &mqtt_attr);
	if (rc == 0) {
        APP_PRINT("[task_cloud] connected to the mqtt broker !\n");
    }
    else {
        APP_PRINT("[task_cloud] connect to the mqtt broker failed !\n");
        return MQTT_NG;
    }

    /* configure publish message */
    mqtt_pub_msg.qos = QOS1;
    mqtt_pub_msg.retained = 0;
    mqtt_pub_msg.dup = 0;
    mqtt_pub_msg.payload = (uint8_t*)MQTT_PUBLISH_PAYLOAD;
    mqtt_pub_msg.payloadlen = strlen(MQTT_PUBLISH_PAYLOAD);

    /* configure subcribe */
    int32_t retval;

    /* subcribe to topic contain response firmware attributes */
    retval = mqtt_subscribe(&client, MQTT_FOTA_SUB_TOPIC_ATTR_RES, QOS1, network_mqtt_subcribe_fota_rev_attr);

    if (retval < 0) {
        APP_PRINT("[task_cloud] mqtt subcribe attr failed: %d\n", retval);
        return MQTT_NG;
    }
    else {
        APP_PRINT("[task_cloud] mqtt subcribe attr successfully !\n");
    }

    /* subcribe to topic contain response payload */
    retval = mqtt_subscribe(&client, MQTT_FOTA_SUB_TOPIC_REV_PAYLOAD, QOS1, network_mqtt_subcribe_fota_rev_payload);

    if (retval < 0) {
        APP_PRINT("[task_cloud] mqtt subcribe fota failed: %d\n", retval);
        return MQTT_NG;
    }
    else {
        APP_PRINT("[task_cloud] mqtt subcribe fota successfully !\n");
    }

    /* mqtt polling start */
    task_polling_set_ability(TASK_POLLING_MQTT_ID, STK_ENABLE);

    return MQTT_OK;
}

void network_mqtt_publish() {
    int32_t retval = 0;
    json_publish_to_mqtt["temperature"] = (float)(((rand() % (300 - 250 + 1)) + 250) / 10);
    serializeJson(json_publish_to_mqtt, json_buffer);

    mqtt_pub_msg.payload = (uint8_t*)json_buffer;
    mqtt_pub_msg.payloadlen = strlen(json_buffer);

    retval = mqtt_publish(&client, "v1/devices/me/telemetry", &mqtt_pub_msg);
    if (retval < 0) {
        APP_PRINT("[task_cloud] mqtt publish failed: %d\n", retval);
    }
    else {
        APP_PRINT("[task_cloud] mqtt published the message !\n");
    }
}

void task_polling_mqtt() {
    int32_t retval = 0;
    if ((retval = mqtt_yield(&client, 60)) < 0) {
        CLOUD_LOG("MQTT yield error: %d\n", retval);
    } 
}

/*****************************************************************************/
/* fota functions 
******************************************************************************/
void network_mqtt_subcribe_fota_rev_attr(MessageData* msg_rev) {
    MQTTMessage *msg = msg_rev->message;
    CLOUD_LOG("%s\n", (uint8_t*)msg->payload);
    if (msg->payloadlen > 0) {
        StaticJsonDocument<512> doc;
        DeserializationError err = deserializeJson(doc, (uint8_t*)msg->payload);
        if (err) {
            CLOUD_LOG("deserializeJson() failed !\n");
        }
        else {
            /* parser firmware attributes */
            const char* fw_title = doc["shared"]["fw_title"];
            const char* fw_version = doc["shared"]["fw_version"];
            fota_frame_rev.bin_len = doc["shared"]["fw_size"];
            const char* fw_checksum = doc["shared"]["fw_checksum"];
            fota_frame_rev.check_sum = strtoul(fw_checksum, NULL, 16);

            if ((fota_frame_rev.bin_len != 0) && (fota_frame_rev.check_sum) != 0) {
                if (strcmp(APP_VER, fw_version) == 0) {
                    CLOUD_LOG("\n");
                    CLOUD_LOG("============================\n");
                    CLOUD_LOG(" checking firmware update\n");
                    CLOUD_LOG("============================\n");
                    CLOUD_LOG("no new firmware updates are available !\n");
                }
                else {
                    /* new firmware update available */
                    CLOUD_LOG("\n");
                    CLOUD_LOG("============================\n");
                    CLOUD_LOG(" checking firmware update\n");
                    CLOUD_LOG("============================\n");
                    CLOUD_LOG("new firmware update available !\n");
                    CLOUD_LOG("[fota_attr] title: %s\n", fw_title);
                    CLOUD_LOG("[fota_attr] version: %s\n", fw_version);
                    CLOUD_LOG("[fota_attr] bin_len: %d\n", fota_frame_rev.bin_len);
                    CLOUD_LOG("[fota_attr] checksum: 0x%08X\n", fota_frame_rev.check_sum);
                    CLOUD_LOG("\n");
                    fota_frame_rev.fw_remain = fota_frame_rev.bin_len;
                    fota_frame_rev.index_write_in_flash = 0;
                    fota_frame_rev.fw_packet_len = CHUNK_SIZE;
                    fota_frame_rev.chunk_id = 0;
                    fota_frame_rev.chunk_retry_req_counter = 0;
                    task_post_pure_msg(TASK_FW_ID, FW_PREPARE_REV_NEW_FIRMWARE);
                }
            }
            else { 
                /* no new firmware updates are available */
                CLOUD_LOG("no new firmware updates are available !\n");
            }
        }
    }
}

void network_mqtt_subcribe_fota_rev_payload(MessageData* msg_rev) {
    MQTTMessage *msg = msg_rev->message;
    if (msg->payloadlen > 0) {
        mem_cpy((uint8_t*)&fota_frame_rev.payload, (uint8_t*)msg->payload, msg->payloadlen);
        task_post_pure_msg(TASK_FW_ID, FW_WRITE_TO_EXTERNAL_FLASH);
    }
}

void network_mqtt_fota_request_attr() {
    if (network_mqtt_init_status == MQTT_OK) {
        /* msg request firmware attributes */
        mqtt_pub_msg.payload = (uint8_t*)&MQTT_FOTA_ATTR_REQ_MSG;
        mqtt_pub_msg.payloadlen = strlen(MQTT_FOTA_ATTR_REQ_MSG);
        int32_t retval = 0;

        retval = mqtt_publish(&client, MQTT_FOTA_PUB_TOPIC_ATTR_REQ, &mqtt_pub_msg);

        if (retval < 0) {
            APP_PRINT("[task_cloud] mqtt publish failed: %d\n", retval);
        }
        else {
            APP_PRINT("[task_cloud] mqtt published the message !\n");
        }
    }
}

int8_t network_mqtt_fota_request_chunk(uint32_t chunk_id) {
    int32_t ret = 0;
    mqtt_pub_msg.payload = (uint8_t*)&"256"; /* chuck size */
    mqtt_pub_msg.payloadlen = 3;

    /* publish topic */
    char topic[50];
    snprintf(topic, sizeof(topic), "v2/fw/request/1/chunk/%lu", chunk_id);

    ret = mqtt_publish(&client, topic, &mqtt_pub_msg);
    
    if (ret < 0) {
        APP_PRINT("[task_cloud] mqtt publish failed: %d\n", ret);
        return MQTT_NG;
    } 
    else {
        /* TODO */
    }
    return MQTT_OK;
}

void network_mqtt_fota_print_progress(uint8_t percent) {
    xprintf("\rDownloading %d", percent);
    xputc('%');
    xputc(' ');
    xputc('[');
    for(uint8_t i = 0; i < (uint8_t)((percent / 100.0) * 30); i++) {
        xprintf("#");
    }
    for(uint8_t i = 0; i < (30 - ((percent / 100.0) * 30)); i++) {
        xprintf("-");
    }
    xprintf("]     ");
}
/**
 ******************************************************************************
 * @file: dns.c
 * @brief: DNS APIs Header file: send DNS query & receive DNS reponse. It depends on stdlib.h & string.h in ansi-c library
 * @date: 2013/10/21
 * @version: 1.1.0
 * @par: Revision history
       	<2013/10/21> 1st Release
       	<2013/12/20> V1.1.0
        1. Remove secondary DNS server in DNS_run
            If 1st DNS_run failed, call DNS_run with 2nd DNS again
        2. DNS_timerHandler -> DNS_time_handler
        3. Remove the unused define
        4. Integrated dns.h dns.c & dns_parse.h dns_parse.c into dns.h & dns.c
       <2013/12/20> V1.1.0
 * @author: Eric Jung & MidnightCow
 * @copyright: Copyright (c)  2013, WIZnet Co., LTD. 
 * All rights reserved.
 * 
 * [Update]
 * @author: Nark
 * @date:   22/08/2024
 * @brief:  code refactor
 ******************************************************************************
**/

#ifndef	__DNS_H__
#define	__DNS_H__

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>

#if defined (DNS_DBG_EN)
#include "xprintf.h"
#define DNS_LOG(fmt, ...)         xprintf("DNS >> " fmt, ##__VA_ARGS__)
#else
#define DNS_LOG(fmt, ...)              
#endif

#define	MAX_DNS_BUF_SIZE            (256)       /* maximum size of DNS buffer */

/**
 * @brief Maxium length of your queried Domain name 
 * @todo SHOULD BE defined it equal as or greater than your Domain name lenght + null character(1)
 * @note SHOULD BE careful to stack overflow because it is allocated 1.5 times as MAX_DOMAIN_NAME in stack.
**/
#define MAX_DOMAIN_NAME             (256)       /* for example "www.google.com" */
#define	MAX_DNS_RETRY               (3)         /* requery count */
#define	DNS_WAIT_TIME               (3)         /* wait response time: unit 1s */
#define	IPPORT_DOMAIN               (53)        /* DNS server port number */
#define DNS_MSG_ID                  (0x1122)    /* ID for DNS message. You can be modifyed it any number */
/**
 * @brief DNS process initialize
 * @param s   : Socket number for DNS
 * @param buf : buffer for DNS message
**/
void dns_init(uint8_t s, uint8_t * buf);

/**
 * @brief DNS process
 * @details Send DNS query and receive DNS response
 * @param dns_ip        : DNS server ip
 * @param name          : Domain name to be queryed
 * @param ip_from_dns   : IP address from DNS server
 * @return  -1 : failed. @ref MAX_DOMIN_NAME is too small
 *           0 : failed  (Timeout or Parse error)\n
 *           1 : success
 * @note This funtion blocks until success or fail. max time = @ref MAX_DNS_RETRY * @ref DNS_WAIT_TIME
**/
int8_t dns_run(uint8_t * dns_ip, uint8_t * name, uint8_t * ip_from_dns);

/**
 * @brief DNS 1s tick timer handler
 * @note SHOULD BE register to your system 1s tick timer handler 
**/
void dns_time_handler();

#ifdef __cplusplus
}
#endif

#endif	/* __DNS_H__ */

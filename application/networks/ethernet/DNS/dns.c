/**
 ******************************************************************************
 * @file: dns.c
 * @brief: DNS APIs Implement file: send DNS query & receive DNS reponse. It depends on stdlib.h & string.h in ansi-c library
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

#include <string.h>
#include <stdlib.h>

#include "socket.h"
#include "dns.h"

#ifdef _DNS_DEBUG_
   #include <stdio.h>
#endif

#define INITRTT        (2000L)  /* initial smoothed response time */
#define MAXCNAME       (MAX_DOMAIN_NAME + (MAX_DOMAIN_NAME >> 1))  /* maximum amount of cname recursion */

#define TYPE_A         (1)     /* host address */
#define TYPE_NS        (2)     /* name server */
#define TYPE_MD        (3)     /* mail destination (obsolete) */
#define TYPE_MF        (4)     /* mail forwarder (obsolete) */
#define TYPE_CNAME     (5)     /* canonical name */
#define TYPE_SOA       (6)     /* start of authority */
#define TYPE_MB        (7)     /* mailbox name (experimental) */
#define TYPE_MG        (8)     /* mail group member (experimental) */
#define TYPE_MR        (9)     /* mail rename name (experimental) */
#define TYPE_NULL      (10)    /* null (experimental) */
#define TYPE_WKS       (11)    /* well-known sockets */
#define TYPE_PTR       (12)    /* pointer record */
#define TYPE_HINFO     (13)    /* host information */
#define TYPE_MINFO     (14)    /* mailbox information (experimental) */
#define TYPE_MX        (15)    /* mail exchanger */
#define TYPE_TXT       (16)    /* text strings */
#define TYPE_ANY       (255)   /* matches any type */

#define	CLASS_IN	   1	   /* The ARPA Internet */

/* Round trip timing parameters */
#define AGAIN         (8)    /* average RTT gain = 1/8 */
#define LAGAIN        (3)    /* log2(AGAIN) */
#define DGAIN         (4)    /* mean deviation gain = 1/4 */
#define LDGAIN        (2)    /* log2(DGAIN) */

/* Header for all domain messages */
struct dhdr {
    uint16_t id;   			/* identification */
    uint8_t  qr;   			/* query/response */
#define QUERY    		(0) /* query */
#define RESPONSE 		(1) /* response */
    uint8_t  opcode;
#define IQUERY   		(1) /* inverse query */
    uint8_t  aa;   			/* authoritative answer */
    uint8_t  tc;   			/* truncation */
    uint8_t  rd;   			/* recursion desired */
    uint8_t  ra;   			/* recursion available */
    uint8_t  rcode;			/* response code */
#define NO_ERROR       	(0) /* no error */
#define FORMAT_ERROR   	(1) /* format error */
#define SERVER_FAIL    	(2) /* server failure */
#define NAME_ERROR     	(3) /* name error */
#define NOT_IMPL       	(4) /* not implemented */
#define REFUSED        	(5) /* refused */
    uint16_t qdcount;  		/* question count */
    uint16_t ancount;  		/* answer count */
    uint16_t nscount;  		/* authority (name server) count */
    uint16_t arcount;  		/* additional record count */
};


uint8_t* pDNSMSG;       	/* DNS message buffer */
uint8_t  DNS_SOCKET;    	/* socket number for DNS */
uint16_t DNS_MSGID;     	/* DNS message ID */

uint32_t dns_1s_tick;   	/* for timeout of DNS processing */
static uint8_t retry_count; /* retry count */

/* converts uint16_t from network buffer to a host byte order integer */
uint16_t get16(uint8_t * s) {
	uint16_t i;
	i = *s++ << 8;
	i = i + *s;
	return i;
}

/* copies uint16_t to the network buffer with network byte order */
uint8_t * put16(uint8_t * s, uint16_t i) {
	*s++ = i >> 8;
	*s++ = i;
	return s;
}

/**
 * CONVERT A DOMAIN NAME TO THE HUMAN-READABLE FORM
 *
 * Description : This function converts a compressed domain name to the human-readable form
 * Arguments   : msg        - is a pointer to the reply message
 *               compressed - is a pointer to the domain name in reply message.
 *               buf        - is a pointer to the buffer for the human-readable form name.
 *               len        - is the MAX. size of buffer.
 * Returns     : the length of compressed message
**/
int parse_name(uint8_t * msg, uint8_t * compressed, char * buf, int16_t len) {
	uint16_t slen;		/* length of current segment */
	uint8_t * cp;
	int clen = 0;		/* lotal length of compressed name */
	int indirect = 0;	/* set if indirection encountered */
	int nseg = 0;		/* total number of segments in name */

	cp = compressed;

	for (;;) {
		slen = *cp++;	/* length of this segment */

		if (!indirect) clen++;

		if ((slen & 0xc0) == 0xc0) {
			if (!indirect)
				clen++;
			indirect = 1;
			/* follow indirection */
			cp = &msg[((slen & 0x3f)<<8) + *cp];
			slen = *cp++;
		}

		if (slen == 0) { /* zero length == all done */
			break;
		}
			
		len -= slen + 1;

		if (len < 0) {
			return -1;
		}

		if (!indirect) {
			clen += slen;
		}

		while (slen-- != 0) {
			*buf++ = (char)*cp++;
		}
		*buf++ = '.';
		nseg++;
	}

	if (nseg == 0) {
		/* root name; represent as single dot */
		*buf++ = '.';
		len--;
	}

	*buf++ = '\0';
	len--;

	return clen;	/* length of compressed message */
}

/**
 * PARSE QUESTION SECTION
 *
 * Description : This function parses the qeustion record of the reply message.
 * Arguments   : msg - is a pointer to the reply message
 *               cp  - is a pointer to the qeustion record.
 * Returns     : a pointer the to next record.
**/
uint8_t * dns_question(uint8_t * msg, uint8_t * cp) {
	int len;
	char name[MAXCNAME];

	len = parse_name(msg, cp, name, MAXCNAME);

	if (len == -1) {
		return 0;
	}

	cp += len;
	cp += 2;		/* type */
	cp += 2;		/* class */

	return cp;
}


/**
 * PARSE ANSER SECTION
 *
 * Description : This function parses the answer record of the reply message.
 * Arguments   : msg - is a pointer to the reply message
 *               cp  - is a pointer to the answer record.
 * Returns     : a pointer the to next record.
**/
uint8_t * dns_answer(uint8_t * msg, uint8_t * cp, uint8_t * ip_from_dns) {
	int len, type;
	char name[MAXCNAME];

	len = parse_name(msg, cp, name, MAXCNAME);

	if (len == -1) {
		return 0;
	}

	cp += len;
	type = get16(cp);
	cp += 2;		/* type */
	cp += 2;		/* class */
	cp += 4;		/* ttl */
	cp += 2;		/* len */


	switch (type) {
	case TYPE_A:
		/* Just read the address directly into the structure */
		ip_from_dns[0] = *cp++;
		ip_from_dns[1] = *cp++;
		ip_from_dns[2] = *cp++;
		ip_from_dns[3] = *cp++;
		break;

	case TYPE_CNAME:
	case TYPE_MB:
	case TYPE_MG:
	case TYPE_MR:
	case TYPE_NS:
	case TYPE_PTR:
		/* these types all consist of a single domain name */
		/* convert it to ascii format */
		len = parse_name(msg, cp, name, MAXCNAME);
		if (len == -1) {
			return 0;
		}

		cp += len;
		break;

	case TYPE_HINFO:
		len = *cp++;
		cp += len;
		len = *cp++;
		cp += len;
		break;

	case TYPE_MX:
		cp += 2;
		/* get domain name of exchanger */
		len = parse_name(msg, cp, name, MAXCNAME);
		if (len == -1) {
			return 0;
		}
		cp += len;
		break;

	case TYPE_SOA:
		/* get domain name of name server */
		len = parse_name(msg, cp, name, MAXCNAME);
		if (len == -1) {
			return 0;
		}

		cp += len;

		/* get domain name of responsible person */
		len = parse_name(msg, cp, name, MAXCNAME);
		if (len == -1) {
			return 0;
		}

		cp += len;
		cp += 4;
		cp += 4;
		cp += 4;
		cp += 4;
		cp += 4;
		break;

	case TYPE_TXT:
		/* just stash */
		break;

	default:
		/* ignore */
		break;
	}

	return cp;
}

/**
 * PARSE THE DNS REPLY
 *
 * Description : This function parses the reply message from DNS server.
 * Arguments   : dhdr - is a pointer to the header for DNS message
 *               buf  - is a pointer to the reply message.
 *               len  - is the size of reply message.
 * Returns     : -1 - Domain name lenght is too big
 *                0 - Fail (Timout or parse error)
 *                1 - Success,
**/
int8_t parseDNSMSG(struct dhdr * pdhdr, uint8_t * pbuf, uint8_t * ip_from_dns) {
	uint16_t tmp;
	uint16_t i;
	uint8_t * msg;
	uint8_t * cp;

	msg = pbuf;
	memset(pdhdr, 0, sizeof(*pdhdr));

	pdhdr->id = get16(&msg[0]);
	tmp = get16(&msg[2]);

	if (tmp & 0x8000) {
		pdhdr->qr = 1;
	}

	pdhdr->opcode = (tmp >> 11) & 0xf;

	if (tmp & 0x0400) {
		pdhdr->aa = 1;
	}
	if (tmp & 0x0200) {
		pdhdr->tc = 1;
	}
	if (tmp & 0x0100) {
		pdhdr->rd = 1;
	}
	if (tmp & 0x0080) {
		pdhdr->ra = 1;
	}

	pdhdr->rcode = tmp & 0xf;
	pdhdr->qdcount = get16(&msg[4]);
	pdhdr->ancount = get16(&msg[6]);
	pdhdr->nscount = get16(&msg[8]);
	pdhdr->arcount = get16(&msg[10]);

	/* now parse the variable length sections */
	cp = &msg[12];

	/* question section */
	for (i = 0; i < pdhdr->qdcount; i++) {
		cp = dns_question(msg, cp);
      	DNS_LOG("MAX_DOMAIN_NAME is too small, it should be redfine in <dns.h> !\n");
		if (!cp) {
			return -1;
		}
	}

	/* answer section */
	for (i = 0; i < pdhdr->ancount; i++) {
		cp = dns_answer(msg, cp, ip_from_dns);
      	DNS_LOG("MAX_DOMAIN_NAME is too small, it should be redfine in <dns.h> !\n");
		if (!cp) {
			return -1;
		}
	}

	/* name server (authority) section */
	for (i = 0; i < pdhdr->nscount; i++) {
		;
	}

	/* additional section */
	for (i = 0; i < pdhdr->arcount; i++) {
		;
	}

	if(pdhdr->rcode == 0) return 1; /* no error */
	else return 0;
}

/**
 * MAKE DNS QUERY MESSAGE
 *
 * Description : This function makes DNS query message.
 * Arguments   : op   - Recursion desired
 *               name - is a pointer to the domain name.
 *               buf  - is a pointer to the buffer for DNS message.
 *               len  - is the MAX. size of buffer.
 * Returns     : the pointer to the DNS message.
**/
int16_t dns_makequery(uint16_t op, char * name, uint8_t * buf, uint16_t len) {
	uint8_t *cp;
	char *cp1;
	char sname[MAXCNAME];
	char *dname;
	uint16_t p;
	uint16_t dlen;

	cp = buf;

	DNS_MSGID++;
	cp = put16(cp, DNS_MSGID);
	p = (op << 11) | 0x0100; /* recursion desired */
	cp = put16(cp, p);
	cp = put16(cp, 1);
	cp = put16(cp, 0);
	cp = put16(cp, 0);
	cp = put16(cp, 0);

	strcpy(sname, name);
	dname = sname;
	dlen = strlen(dname);

	for (;;) {
		/* look for next dot */
		cp1 = strchr(dname, '.');

		if (cp1 != NULL) len = cp1 - dname;	/* More to come */
		else len = dlen;			/* Last component */

		*cp++ = len;				/* Write length of component */
		if (len == 0) break;

		/* copy component up to (but not including) dot */
		strncpy((char *)cp, dname, len);
		cp += len;
		if (cp1 == NULL)
		{
			*cp++ = 0;			/* last one; write null and finish */
			break;
		}
		dname += len+1;
		dlen -= len+1;
	}

	cp = put16(cp, 0x0001);				/* type */
	cp = put16(cp, 0x0001);				/* class */

	return ((int16_t)((uint32_t)(cp) - (uint32_t)(buf)));
}

/**
 * CHECK DNS TIMEOUT
 *
 * Description : This function check the DNS timeout
 * Arguments   : None.
 * Returns     : -1 - timeout occurred, 0 - timer over, but no timeout, 1 - no timer over, no timeout occur
 * Note        : timeout : retry count and timer both over.
**/
int8_t check_dns_timeout() {
	if (dns_1s_tick >= DNS_WAIT_TIME) {
		dns_1s_tick = 0;
		if (retry_count >= MAX_DNS_RETRY) {
			retry_count = 0;
			DNS_LOG("DNS timeout !\n");
			return -1; /* timeout occurred */
		}
		retry_count++;
		return 0; /* timer over, but no timeout */
	}
	return 1; /* no timer over, no timeout occur */
}

void dns_init(uint8_t s, uint8_t * buf) {
	DNS_SOCKET = s;	/* SOCK_DNS */
	pDNSMSG = buf; 	/* User's shared buffer */
	DNS_MSGID = DNS_MSG_ID;
}

int8_t dns_run(uint8_t * dns_ip, uint8_t * name, uint8_t * ip_from_dns) {
	int8_t ret = 0;
	struct dhdr dhp;
	uint8_t ip[4];
	uint16_t len, port;
	int8_t ret_check_timeout;
	static uint8_t timeout_retry_counter = MAX_DNS_RETRY;

	dns_1s_tick = 0;

   	socket(DNS_SOCKET, Sn_MR_UDP, 0, 0);

	DNS_LOG("> DNS Query to DNS Server: %d.%d.%d.%d\n", dns_ip[0], dns_ip[1], dns_ip[2], dns_ip[3]);

	len = dns_makequery(0, (char *)name, pDNSMSG, MAX_DNS_BUF_SIZE);
	sendto(DNS_SOCKET, pDNSMSG, len, dns_ip, IPPORT_DOMAIN);

	while (timeout_retry_counter != 0) {
		if ((len = getSn_RX_RSR(DNS_SOCKET)) > 0) {
			if (len > MAX_DNS_BUF_SIZE) {
				len = MAX_DNS_BUF_SIZE;
			}
			len = recvfrom(DNS_SOCKET, pDNSMSG, len, ip, &port);
	      	DNS_LOG("> Receive DNS message from %d.%d.%d.%d(%d) - len = %d\n", ip[0], ip[1], ip[2], ip[3], port,len); 
         	ret = parseDNSMSG(&dhp, pDNSMSG, ip_from_dns);
			break;
		}

		ret_check_timeout = check_dns_timeout();

		if (ret_check_timeout < 0) {
			DNS_LOG("DNS Server is not responding : %d.%d.%d.%d\n", dns_ip[0], dns_ip[1], dns_ip[2], dns_ip[3]);
			close(DNS_SOCKET);
			return 0; /* timeout occurred */
		}
		else if (ret_check_timeout == 0) {
			DNS_LOG("DNS timeout !\n");
			sendto(DNS_SOCKET, pDNSMSG, len, dns_ip, IPPORT_DOMAIN);
			timeout_retry_counter--;
		}
	}
	close(DNS_SOCKET);
	/* return value
		0:  failed
		1 - success
	*/
	return ret;
}

void dns_time_handler() {
	dns_1s_tick++;
}
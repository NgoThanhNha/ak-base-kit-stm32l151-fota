/**
 ******************************************************************************
 * @file: w5500_port.h
 * @brief: WIZCHIP Config Header File: These functions is optional function. It could be replaced at WIZCHIP I/O function because they were made by WIZCHIP I/O functions.
 * @details: There are functions of configuring WIZCHIP, network, interrupt, phy, network information and timer.
 * @date: 2013/10/21
 * @version: 1.0.0
 * @par: Revision history
         <2015/02/05> Notice
         The version history is not updated after this point.
         Download the latest version directly from GitHub. Please visit the our GitHub repository for ioLibrary.
         >> https://github.com/Wiznet/ioLibrary_Driver
         <2013/10/21> 1st Release
 * @author: MidnightCow
 * @copyright: Copyright (c)  2013, WIZnet Co., LTD. 
 * All rights reserved.
 * 
 * [Update]
 * @author: Nark
 * @date:   22/08/2024
 * @brief:  code refactor
 ******************************************************************************
**/

#ifndef __W5500_PORT_H__
#define __W5500_PORT_H__

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>
#include <stdio.h>

#define W5100                             (5100)
#define W5100S						            (5100 + 5)
#define W5200						            (5200)
#define W5300						            (5300)
#define W5500						            (5500)

#ifndef _WIZCHIP_
#define _WIZCHIP_                         (W5500)
#endif

#define _WIZCHIP_IO_MODE_NONE_            (0x0000)
#define _WIZCHIP_IO_MODE_BUS_             (0x0100) /* Bus interface mode */
#define _WIZCHIP_IO_MODE_SPI_             (0x0200) /* SPI interface mode */

#define _WIZCHIP_IO_MODE_BUS_DIR_         (_WIZCHIP_IO_MODE_BUS_ + 1) /* BUS interface mode for direct  */
#define _WIZCHIP_IO_MODE_BUS_INDIR_       (_WIZCHIP_IO_MODE_BUS_ + 2) /* BUS interface mode for indirect */

#define _WIZCHIP_IO_MODE_SPI_VDM_         (_WIZCHIP_IO_MODE_SPI_ + 1) /* SPI interface mode for variable length data */
#define _WIZCHIP_IO_MODE_SPI_FDM_         (_WIZCHIP_IO_MODE_SPI_ + 2) /* SPI interface mode for fixed length data mode */
#define _WIZCHIP_IO_MODE_SPI_5500_        (_WIZCHIP_IO_MODE_SPI_ + 3) /* SPI interface mode for fixed length data mode */

#if (_WIZCHIP_ == W5500)
   #define _WIZCHIP_ID_                    "W5500\0"

/**
 * @brief Define interface mode.
 * @todo Should select interface mode as chip. 
 * @ref _WIZCHIP_IO_MODE_SPI_
 * @ref _WIZCHIP_IO_MODE_SPI_VDM_ : Valid only in @ref _WIZCHIP_ == W5500
 * @ref _WIZCHIP_IO_MODE_SPI_FDM_ : Valid only in @ref _WIZCHIP_ == W5500
 * @ref _WIZCHIP_IO_MODE_BUS_
 * @ref _WIZCHIP_IO_MODE_BUS_DIR_
 * @ref _WIZCHIP_IO_MODE_BUS_INDIR_
 * Others will be defined in future.
 * ex> <code> #define _WIZCHIP_IO_MODE_ _WIZCHIP_IO_MODE_SPI_VDM_ </code>
**/

#ifndef _WIZCHIP_IO_MODE_
   #define _WIZCHIP_IO_MODE_              _WIZCHIP_IO_MODE_SPI_VDM_
#endif /* _WIZCHIP_IO_MODE_ */

   typedef uint8_t iodata_t;
   #include "w5500.h"

#else
   #error "Unknown defined _WIZCHIP_. You should define one of 5100, 5200, and 5500 !"
#endif /* (_WIZCHIP_ == W5500) */

#ifndef _WIZCHIP_IO_MODE_
   #error "Undefined _WIZCHIP_IO_MODE_. You should define it !"
#endif

/**
 * @brief Define I/O base address when BUS IF mode.
 * @todo Should re-define it to fit your system when BUS IF Mode (@ref _WIZCHIP_IO_MODE_BUS_
 * @ref _WIZCHIP_IO_MODE_BUS_DIR_, @ref _WIZCHIP_IO_MODE_BUS_INDIR_).
 * ex> <code> #define _WIZCHIP_IO_BASE_ 0x00008000 </code>
**/

#if _WIZCHIP_IO_MODE_ & _WIZCHIP_IO_MODE_BUS_
	#define _WIZCHIP_IO_BASE_				0x68000000 /* for W5300 */
#elif _WIZCHIP_IO_MODE_ & _WIZCHIP_IO_MODE_SPI_
	#define _WIZCHIP_IO_BASE_				0x00000000 /* for 5100S SPI */
#endif

#ifndef _WIZCHIP_IO_BASE_
   #define _WIZCHIP_IO_BASE_           0x00000000 /* 0x8000 */
#endif

#if _WIZCHIP_IO_MODE_ & _WIZCHIP_IO_MODE_BUS_
   #ifndef _WIZCHIP_IO_BASE_
      #error "You should be define _WIZCHIP_IO_BASE to fit your system memory map !"
   #endif
#endif   

#if _WIZCHIP_ >= W5200
   #define _WIZCHIP_SOCK_NUM_   8 /* the count of independant socket of @b WIZCHIP */
#else
   #define _WIZCHIP_SOCK_NUM_   4 /* the count of independant socket of @b WIZCHIP */
#endif      


/********************************************************
* WIZCHIP BASIC IF functions for SPI, SDIO, I2C , ETC.
*********************************************************/
/**
 * @ingroup data type
 * @brief the set of callback functions for W5500:@ref WIZCHIP_IO_Functions W5200:@ref WIZCHIP_IO_Functions_W5200
**/

typedef struct __WIZCHIP {
   uint16_t  if_mode;               /* host interface mode */
   uint8_t   id[8];                 /* WIZCHIP ID such as @b 5100, @b 5100S, @b 5200, @b 5500, and so on. */

   /* the set of critical section callback function */
   struct _CRIS {
      void (*_enter)  (void);       /* crtical section enter */
      void (*_exit) (void);         /* critial section exit */  
   } CRIS;  

   /* the control callback function */
   struct _CS {
      void (*_select)  (void);      /* _WIZCHIP_ selected */
      void (*_deselect)(void);      /* _WIZCHIP_ deselected */
   } CS;  

   /* the set of interface IO callback function */
   union _IF {	 
      /* for BUS interface IO */
      struct {
         iodata_t (*_read_data) (uint32_t AddrSel);
         void (*_write_data) (uint32_t AddrSel, iodata_t wb);
      } BUS;     

      /* for SPI interface IO */
      struct {
         uint8_t (*_read_byte)   (void);
         void    (*_write_byte)  (uint8_t wb);
         void    (*_read_burst)  (uint8_t* pBuf, uint16_t len);
         void    (*_write_burst) (uint8_t* pBuf, uint16_t len);
      } SPI;
   } IF;
} _WIZCHIP;

extern _WIZCHIP WIZCHIP;

/**
 * @ingroup data type
 *  WIZCHIP control type enumration used in @ref ctlwizchip().
 */

typedef enum {
   CW_RESET_WIZCHIP,   /* resets wizchip by softly */
   CW_INIT_WIZCHIP,    /* initializes wizchip with socket buffer size 2 or 1 dimension array typed uint8_t */
   CW_GET_INTERRUPT,   /* get interrupt status of wizchip */
   CW_CLR_INTERRUPT,   /* clears interrupt */
   CW_SET_INTRMASK,    /* masks interrupt */
   CW_GET_INTRMASK,    /* get interrupt mask */
   CW_SET_INTRTIME,    /* set interval time between the current and next interrupt */
   CW_GET_INTRTIME,    /* get interval time between the current and next interrupt */
   CW_GET_ID,          /* gets wizchip name */

   CW_RESET_PHY,       /* resets internal phy. valid only w5500 */
   CW_SET_PHYCONF,     /* set phy operation mode (manual/auto, 10/100, half/full) in internal register. valid only w5500 */
   CW_GET_PHYCONF,     /* get phy operation mode in internal register. valid only w5500 */
   CW_GET_PHYSTATUS,   /* get real phy status on operating. valid only w5500 */
   CW_SET_PHYPOWMODE,  /* set phy power mode as normal and down when phystatus.opmd == 1. valid only w5500 */

   CW_GET_PHYPOWMODE,  /* get phy power mode as down or normal, valid only w5200, w5500 */
   CW_GET_PHYLINK      /* get phy link status, valid only w5100, w5200 */
} ctlwizchip_type;

/**
 * @ingroup data type
 *  Network control type enumration used in @ref ctlnetwork().
 */

typedef enum {
   CN_SET_NETINFO,  /* set network with @ref wiz_netinfo */
   CN_GET_NETINFO,  /* get network with @ref wiz_netinfo */
   CN_SET_NETMODE,  /* set network mode as wol, pppoe, ping block, and force arp mode */
   CN_GET_NETMODE,  /* get network mode as wol, pppoe, ping block, and force arp mode */
   CN_SET_TIMEOUT,  /* set network timeout as retry count and time */
   CN_GET_TIMEOUT,  /* get network timeout as retry count and time */
} ctlnetwork_type;

/**
 * @ingroup data type
 *  Interrupt kind when CW_SET_INTRRUPT, CW_GET_INTERRUPT, CW_SET_INTRMASK
 *  and CW_GET_INTRMASK is used in @ref ctlnetwork().
 *  It can be used with OR operation.
 */

typedef enum {
#if _WIZCHIP_ == W5500
   IK_WOL               = (1 << 4),   /* wake on lan by receiving the magic packet. valid in w5500 */
#elif _WIZCHIP_ == W5300
   IK_FMTU              = (1 << 4),   /* received an icmp message (fragment mtu) */
#endif   

   IK_PPPOE_TERMINATED  = (1 << 5),   /* pppoe disconnected */

#if _WIZCHIP_ != W5200
   IK_DEST_UNREACH      = (1 << 6),   /* destination ip & port unreachable, no use in w5200 */
#endif   

   IK_IP_CONFLICT       = (1 << 7),   /* ip conflict occurred */

   IK_SOCK_0            = (1 << 8),   /* socket 0 interrupt */
   IK_SOCK_1            = (1 << 9),   /* socket 1 interrupt */
   IK_SOCK_2            = (1 << 10),  /* socket 2 interrupt */
   IK_SOCK_3            = (1 << 11),  /* socket 3 interrupt */
#if _WIZCHIP_ > W5100S
   IK_SOCK_4            = (1 << 12),  /* socket 4 interrupt, no use in w5100 */
   IK_SOCK_5            = (1 << 13),  /* socket 5 interrupt, no use in w5100 */
   IK_SOCK_6            = (1 << 14),  /* socket 6 interrupt, no use in w5100 */
   IK_SOCK_7            = (1 << 15),  /* socket 7 interrupt, no use in w5100 */
#endif   

#if _WIZCHIP_ > W5100S
   IK_SOCK_ALL          = (0xFF << 8) /* all socket interrupt */
#else
   IK_SOCK_ALL          = (0x0F << 8) /* all socket interrupt */
#endif      
} intr_kind;

#define PHY_CONFBY_HW            (0)   /* configured phy operation mode by hw pin */
#define PHY_CONFBY_SW            (1)   /* configured phy operation mode by sw register */
#define PHY_MODE_MANUAL          (0)   /* configured phy operation mode with user setting */
#define PHY_MODE_AUTONEGO        (1)   /* configured phy operation mode with auto-negotiation */
#define PHY_SPEED_10             (0)   /* link speed 10 */
#define PHY_SPEED_100            (1)   /* link speed 100 */
#define PHY_DUPLEX_HALF          (0)   /* link half-duplex */
#define PHY_DUPLEX_FULL          (1)   /* link full-duplex */
#define PHY_LINK_OFF             (0)   /* link off */
#define PHY_LINK_ON              (1)   /* link on */
#define PHY_POWER_NORM           (0)   /* phy power normal mode */
#define PHY_POWER_DOWN           (1)   /* phy power down mode */

#if _WIZCHIP_ == W5100S || _WIZCHIP_ == W5500
/**
 * @ingroup data type
 *  Configures PHY configuration when CW_SET_PHYCONF or CW_GET_PHYCONF in W5500,  
 *  and indicates the real PHY status configured by HW or SW in all WIZCHIP. \n
 *  Valid only in W5500.
**/
typedef struct wiz_PhyConf_t {
      uint8_t by;       /* set by @ref PHY_CONFBY_HW or @ref PHY_CONFBY_SW */
      uint8_t mode;     /* set by @ref PHY_MODE_MANUAL or @ref PHY_MODE_AUTONEGO */
      uint8_t speed;    /* set by @ref PHY_SPEED_10 or @ref PHY_SPEED_100 */
      uint8_t duplex;   /* set by @ref PHY_DUPLEX_HALF or @ref PHY_DUPLEX_FULL */
      //uint8_t power;  /* set by @ref PHY_POWER_NORM or @ref PHY_POWER_DOWN */
      //uint8_t link;   /* valid only in CW_GET_PHYSTATUS. set by @ref PHY_LINK_ON or PHY_LINK_OFF */
} wiz_PhyConf;
#endif /* _WIZCHIP_ == W5100S || _WIZCHIP_ == W5500 */

/**
 * @ingroup data type
 *  It used in setting dhcp_mode of @ref wiz_NetInfo.
**/
typedef enum {
   NETINFO_STATIC = 1,  /* static IP configuration by manually */
   NETINFO_DHCP         /* dynamic IP configruation from a DHCP sever */
} dhcp_mode;

/**
 * @ingroup data type
 *  Network Information for WIZCHIP
 */
typedef struct wiz_NetInfo_t {
   uint8_t mac[6];      /* source mac address */
   uint8_t ip[4];       /* source ip address */
   uint8_t sn[4];       /* subnet mask */
   uint8_t gw[4];       /* gateway ip address */
   uint8_t dns[4];      /* dns server ip address */
   dhcp_mode dhcp;      /* 1 - static, 2 - dhcp */
} wiz_NetInfo;

/**
 * @ingroup data type
 *  Network mode
**/
typedef enum {
#if _WIZCHIP_ == W5500
   NM_FORCEARP    = (1 << 1),  /* force to app send whenever udp data is sent. valid only in w5500 */
#endif   
   NM_WAKEONLAN   = (1 << 5),  /* wake on lan */
   NM_PINGBLOCK   = (1 << 4),  /* block ping-request */
   NM_PPPOE       = (1 << 3),  /* pppoe mode */
} netmode_type;

/**
 * @ingroup data type
 *  Used in CN_SET_TIMEOUT or CN_GET_TIMEOUT of @ref ctlwizchip() for timeout configruation.
**/
typedef struct wiz_NetTimeout_t {
   uint8_t  retry_cnt;     ///< retry count 
   uint16_t time_100us;    ///< time unit 100us
} wiz_NetTimeout;

/**
 *@brief Registers call back function for critical section of I/O functions such as
 *\ref WIZCHIP_READ, @ref WIZCHIP_WRITE, @ref WIZCHIP_READ_BUF and @ref WIZCHIP_WRITE_BUF.
 *@param cris_en : callback function for critical section enter.
 *@param cris_ex : callback function for critical section exit.
 *@todo Describe @ref WIZCHIP_CRITICAL_ENTER and @ref WIZCHIP_CRITICAL_EXIT marco or register your functions.
 *@note If you do not describe or register, default functions(@ref wizchip_cris_enter & @ref wizchip_cris_exit) is called.
 */
void reg_wizchip_cris_cbfunc(void(*cris_en)(void), void(*cris_ex)(void));

/**
 *@brief Registers call back function for WIZCHIP select & deselect.
 *@param cs_sel : callback function for WIZCHIP select
 *@param cs_desel : callback fucntion for WIZCHIP deselect
 *@todo Describe @ref wizchip_cs_select and @ref wizchip_cs_deselect function or register your functions.
 *@note If you do not describe or register, null function is called.
 */
void reg_wizchip_cs_cbfunc(void(*cs_sel)(void), void(*cs_desel)(void));

/**
 *@brief Registers call back function for bus interface.
 *@param bus_rb   : callback function to read byte data using system bus
 *@param bus_wb   : callback function to write byte data using system bus
 *@todo Describe @ref wizchip_bus_readbyte and @ref wizchip_bus_writebyte function
 *or register your functions.
 *@note If you do not describe or register, null function is called.
 */
//M20150601 : For integrating with W5300
//void reg_wizchip_bus_cbfunc(uint8_t (*bus_rb)(uint32_t addr), void (*bus_wb)(uint32_t addr, uint8_t wb));
void reg_wizchip_bus_cbfunc(iodata_t (*bus_rb)(uint32_t addr), void (*bus_wb)(uint32_t addr, iodata_t wb));

/**
 *@brief Registers call back function for SPI interface.
 *@param spi_rb : callback function to read byte using SPI
 *@param spi_wb : callback function to write byte using SPI
 *@todo Describe \ref wizchip_spi_readbyte and \ref wizchip_spi_writebyte function
 *or register your functions.
 *@note If you do not describe or register, null function is called.
 */
void reg_wizchip_spi_cbfunc(uint8_t (*spi_rb)(void), void (*spi_wb)(uint8_t wb));

/**
 *@brief Registers call back function for SPI interface.
 *@param spi_rb : callback function to burst read using SPI
 *@param spi_wb : callback function to burst write using SPI
 *@todo Describe \ref wizchip_spi_readbyte and \ref wizchip_spi_writebyte function
 *or register your functions.
 *@note If you do not describe or register, null function is called.
 */
void reg_wizchip_spiburst_cbfunc(void (*spi_rb)(uint8_t* pBuf, uint16_t len), void (*spi_wb)(uint8_t* pBuf, uint16_t len));

/**
 * @ingroup extra_functions
 * @brief Controls to the WIZCHIP.
 * @details Resets WIZCHIP & internal PHY, Configures PHY mode, Monitor PHY(Link,Speed,Half/Full/Auto),
 * controls interrupt & mask and so on.
 * @param cwtype : Decides to the control type
 * @param arg : arg type is dependent on cwtype.
 * @return  0 : Success \n
 *         -1 : Fail because of invalid \ref ctlwizchip_type or unsupported \ref ctlwizchip_type in WIZCHIP 
 */          
int8_t ctlwizchip(ctlwizchip_type cwtype, void* arg);

/**
 * @ingroup extra_functions
 * @brief Controls to network.
 * @details Controls to network environment, mode, timeout and so on.
 * @param cntype : Input. Decides to the control type
 * @param arg : Inout. arg type is dependent on cntype.
 * @return -1 : Fail because of invalid \ref ctlnetwork_type or unsupported \ref ctlnetwork_type in WIZCHIP \n
 *          0 : Success      
 */          
int8_t ctlnetwork(ctlnetwork_type cntype, void* arg);

/* 
 * The following functions are implemented for internal use. 
 * but You can call these functions for code size reduction instead of ctlwizchip() and ctlnetwork().
**/
 
/**
 * @ingroup extra_functions
 * @brief Reset WIZCHIP by softly.
 */ 
void wizchip_sw_reset();

/**
 * @ingroup extra_functions
 * @brief Initializes WIZCHIP with socket buffer size
 * @param txsize Socket tx buffer sizes. If null, initialized the default size 2KB.
 * @param rxsize Socket rx buffer sizes. If null, initialized the default size 2KB.
 * @return 0 : succcess \n
 *        -1 : fail. Invalid buffer size
 */
int8_t wizchip_init(uint8_t* txsize, uint8_t* rxsize);

/** 
 * @ingroup extra_functions
 * @brief Clear Interrupt of WIZCHIP.
 * @param intr : @ref intr_kind value operated OR. It can type-cast to uint16_t.
 */
void wizchip_clrinterrupt(intr_kind intr);

/** 
 * @ingroup extra_functions
 * @brief Get Interrupt of WIZCHIP.
 * @return @ref intr_kind value operated OR. It can type-cast to uint16_t.
 */
intr_kind wizchip_getinterrupt();

/** 
 * @ingroup extra_functions
 * @brief Mask or Unmask Interrupt of WIZCHIP.
 * @param intr : @ref intr_kind value operated OR. It can type-cast to uint16_t.
 */
void wizchip_setinterruptmask(intr_kind intr);

/** 
 * @ingroup extra_functions
 * @brief Get Interrupt mask of WIZCHIP.
 * @return : The operated OR vaule of @ref intr_kind. It can type-cast to uint16_t.
 */
intr_kind wizchip_getinterruptmask();

//todo
#if _WIZCHIP_ > W5100
   int8_t wizphy_getphylink();   /* get the link status of phy in WIZCHIP. No use in W5100 */
   int8_t wizphy_getphypmode();  /* get the power mode of PHY in WIZCHIP. No use in W5100 */
#endif

#if _WIZCHIP_ == W5100S || _WIZCHIP_ == W5500
   void wizphy_reset();    /* Reset phy. Vailid only in W5500 */
/**
 * @ingroup extra_functions
 * @brief Set the phy information for WIZCHIP without power mode
 * @param phyconf : @ref wiz_PhyConf
**/
   void wizphy_setphyconf(wiz_PhyConf* phyconf);

 /**
 * @ingroup extra_functions
 * @brief Get phy configuration information.
 * @param phyconf : @ref wiz_PhyConf
 */
   void wizphy_getphyconf(wiz_PhyConf* phyconf);

 /**
 * @ingroup extra_functions
 * @brief Get phy status.
 * @param phyconf : @ref wiz_PhyConf
 */ 
   void wizphy_getphystat(wiz_PhyConf* phyconf);

 /**
 * @ingroup extra_functions
 * @brief set the power mode of phy inside WIZCHIP. Refer to @ref PHYCFGR in W5500, @ref PHYSTATUS in W5200
 * @param pmode Settig value of power down mode.
 */   
   int8_t wizphy_setphypmode(uint8_t pmode);    
#endif

/**
* @ingroup extra_functions
 * @brief Set the network information for WIZCHIP
 * @param pnetinfo : @ref wizNetInfo
 */
void wizchip_setnetinfo(wiz_NetInfo* pnetinfo);

/**
 * @ingroup extra_functions
 * @brief Get the network information for WIZCHIP
 * @param pnetinfo : @ref wizNetInfo
 */
void wizchip_getnetinfo(wiz_NetInfo* pnetinfo);

/**
 * @ingroup extra_functions
 * @brief Set the network mode such WOL, PPPoE, Ping Block, and etc. 
 * @param pnetinfo Value of network mode. Refer to @ref netmode_type.
 */
int8_t wizchip_setnetmode(netmode_type netmode);

/**
 * @ingroup extra_functions
 * @brief Get the network mode such WOL, PPPoE, Ping Block, and etc. 
 * @return Value of network mode. Refer to @ref netmode_type.
 */
netmode_type wizchip_getnetmode();

/**
 * @ingroup extra_functions
 * @brief Set retry time value(@ref _RTR_) and retry count(@ref _RCR_).
 * @details @ref _RTR_ configures the retransmission timeout period and @ref _RCR_ configures the number of time of retransmission.  
 * @param nettime @ref _RTR_ value and @ref _RCR_ value. Refer to @ref wiz_NetTimeout. 
 */
void wizchip_settimeout(wiz_NetTimeout* nettime);

/**
 * @ingroup extra_functions
 * @brief Get retry time value(@ref _RTR_) and retry count(@ref _RCR_).
 * @details @ref _RTR_ configures the retransmission timeout period and @ref _RCR_ configures the number of time of retransmission.  
 * @param nettime @ref _RTR_ value and @ref _RCR_ value. Refer to @ref wiz_NetTimeout. 
 */
void wizchip_gettimeout(wiz_NetTimeout* nettime);

#ifdef __cplusplus
 }
#endif

#endif /* __W5500_PORT_H__*/
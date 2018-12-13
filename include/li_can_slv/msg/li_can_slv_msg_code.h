/****************************************************************************/
/*                                                                          */
/*                     Copyright (c) 2018, Liebherr PME1                    */
/*                         ALL RIGHTS RESERVED                              */
/*                                                                          */
/* This file is part of li_can_slv stack which is free software: you can    */
/* redistribute it and/or modify it under the terms of the GNU General      */
/* Public License as published by the Free Software Foundation, either      */
/* version 3 of the License, or (at your option) any later version.         */
/*                                                                          */
/* The li_can_slv stack is distributed in the hope that it will be useful,  */
/* but WITHOUT ANY WARRANTY; without even the implied warranty of           */
/* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General */
/* Public License for more details.                                         */
/*                                                                          */
/* You should have received a copy of the GNU General Public License which  */
/* should be located in the the root of the Stack. If not, contact Liebherr */
/* to obtain a copy.                                                        */
/****************************************************************************/

/**
 * @file li_can_slv_msg_code.h
 * @brief header to li_can_slv_msg_code.h module
 * @addtogroup message_code
 * @{
 */

#ifndef LI_CAN_SLV_MSG_CODE_H_
#define LI_CAN_SLV_MSG_CODE_H_

#ifdef __cplusplus
extern "C" {
#endif // #ifdef __cplusplus

/*--------------------------------------------------------------------------*/
/* included files                                                           */
/*--------------------------------------------------------------------------*/

/*--------------------------------------------------------------------------*/
/* general definitions                                                      */
/*--------------------------------------------------------------------------*/
/**
 * @todo future usage
 */
//#define ERR_MSG_CAN_ASYNC_TNL_ALREADY_USED                                          (MSG_CODE_ADAPT(0x75000001)) /*!< \brief message 0x75000001*/
//#define ERR_MSG_CAN_ASYNC_TNL_WRONG_STATE                                           (MSG_CODE_ADAPT(0x75000002)) /*!< \brief message 0x75000002*/
//#define ERR_MSG_CAN_MAIN_UNDEFINED_ISR_ID                                           (MSG_CODE_ADAPT(0x75000003)) /*!< \brief message 0x75000003*/
//#define ERR_MSG_CAN_MON_UNDEFINED_ISR_ID                                            (MSG_CODE_ADAPT(0x75000004)) /*!< \brief message 0x75000004*/
//#define ERR_MSG_CAN_CONFIG_SET_INVALID_BAUDRATE                                     (MSG_CODE_ADAPT(0x75000005)) /*!< \brief message 0x75000005*/
//#define ERR_MSG_CAN_DEFINE_OBJ_INVALID_NR                                           (MSG_CODE_ADAPT(0x75000006)) /*!< \brief message 0x75000006*/
//#define ERR_MSG_CAN_DEFINE_OBJ_INVALID_ID                                           (MSG_CODE_ADAPT(0x75000007)) /*!< \brief message 0x75000007*/
//#define ERR_MSG_CAN_DEFINE_OBJ_INVALID_DLC                                          (MSG_CODE_ADAPT(0x75000008)) /*!< \brief message 0x75000008*/
//#define ERR_MSG_CAN_DEFINE_OBJ_INVALID_DIR                                          (MSG_CODE_ADAPT(0x75000009)) /*!< \brief message 0x75000009*/
//#define ERR_MSG_CAN_MAIN_OVER_OBJECT_NR                                             (MSG_CODE_ADAPT(0x7500000A)) /*!< \brief message 0x7500000A*/
//#define ERR_MSG_CAN_CONFIG_REDECLARED_MODULE_TYPE                                   (MSG_CODE_ADAPT(0x7500000B)) /*!< \brief message 0x7500000B*/
//#define ERR_MSG_CAN_CONFIG_MAXIMUM_NR_OF_MODULE_TYPE                                (MSG_CODE_ADAPT(0x7500000C)) /*!< \brief message 0x7500000C*/
//#define ERR_MSG_CAN_CONFIG_REDECLARED_MODULE_NR                                     (MSG_CODE_ADAPT(0x7500000D)) /*!< \brief message 0x7500000D*/
//#define ERR_MSG_CAN_TX_SYS_MSG_QUEUE_OVERFLOW                                       (MSG_CODE_ADAPT(0x7500000E)) /*!< \brief message 0x7500000E*/
//#define ERR_MSG_CAN_MAIN_TIMEOUT_PROCESS_DATA_TX                                    (MSG_CODE_ADAPT(0x7500000F)) /*!< \brief message 0x7500000F*/
//#define ERR_MSG_CAN_MAIN_TIMEOUT_SYS_MSG_TX                                         (MSG_CODE_ADAPT(0x75000010)) /*!< \brief message 0x75000010*/
//#define ERR_MSG_CAN_MAIN_RX_WRONG_MODULE_NR                                         (MSG_CODE_ADAPT(0x75000011)) /*!< \brief message 0x75000011*/
//#define ERR_MSG_CAN_MAIN_RX_WRONG_DLC                                               (MSG_CODE_ADAPT(0x75000012)) /*!< \brief message 0x75000012*/
//#define ERR_MSG_CAN_MAIN_NR_OF_PROCESS_REQUEST                                      (MSG_CODE_ADAPT(0x75000013)) /*!< \brief message 0x75000013*/
//#define ERR_MSG_CAN_MAIN_DLC_OF_PROCESS_REQUEST                                     (MSG_CODE_ADAPT(0x75000014)) /*!< \brief message 0x75000014*/
//#define ERR_MSG_CAN_MAIN_NR_OF_RX_DATA                                              (MSG_CODE_ADAPT(0x75000015)) /*!< \brief message 0x75000015*/
//#define ERR_MSG_CAN_MAIN_NR_OF_TX_DATA                                              (MSG_CODE_ADAPT(0x75000016)) /*!< \brief message 0x75000016*/
//#define ERR_MSG_CAN_MAIN_OVER_TABLE_POS                                             (MSG_CODE_ADAPT(0x75000017)) /*!< \brief message 0x75000017*/
//#define ERR_MSG_CAN_MON_RX_WRONG_MODULE_NR                                          (MSG_CODE_ADAPT(0x75000018)) /*!< \brief message 0x75000018*/
//#define ERR_MSG_CAN_INIT_FAILED                                                     (MSG_CODE_ADAPT(0x75000019)) /*!< \brief message 0x75000019*/
//#define ERR_MSG_CAN_MON_RX_WRONG_DLC                                                (MSG_CODE_ADAPT(0x7500001A)) /*!< \brief message 0x7500001A*/
//#define ERR_MSG_CAN_MON_TX_WRONG_MODULE_NR                                          (MSG_CODE_ADAPT(0x7500001B)) /*!< \brief message 0x7500001B*/
//#define ERR_MSG_CAN_MON_TX_WRONG_DLC                                                (MSG_CODE_ADAPT(0x7500001C)) /*!< \brief message 0x7500001C*/
//#define ERR_MSG_CAN_MON_NR_OF_PROCESS_REQUEST                                       (MSG_CODE_ADAPT(0x7500001D)) /*!< \brief message 0x7500001D*/
//#define ERR_MSG_CAN_MON_DLC_OF_PROCESS_REQUEST                                      (MSG_CODE_ADAPT(0x7500001E)) /*!< \brief message 0x7500001E*/
//#define ERR_MSG_CAN_MON_NR_OF_RX_DATA                                               (MSG_CODE_ADAPT(0x7500001F)) /*!< \brief message 0x7500001F*/
//#define ERR_MSG_CAN_MON_NR_OF_TX_DATA                                               (MSG_CODE_ADAPT(0x75000020)) /*!< \brief message 0x75000020*/
//#define ERR_MSG_CAN_MAIN_MON_DATA_RX                                                (MSG_CODE_ADAPT(0x75000021)) /*!< \brief message 0x75000021*/
//#define ERR_MSG_CAN_MAIN_MON_DATA_TX                                                (MSG_CODE_ADAPT(0x75000022)) /*!< \brief message 0x75000022*/
//#define ERR_MSG_CAN_NO_MSG_OBJ_FREE                                                 (MSG_CODE_ADAPT(0x75000023)) /*!< \brief message 0x75000023*/
//#define ERR_MSG_CAN_MAIN_RX_WRONG_SLOT                                              (MSG_CODE_ADAPT(0x75000024)) /*!< \brief message 0x75000024*/
//#define ERR_MSG_CAN_MAIN_TX_WRONG_SLOT                                              (MSG_CODE_ADAPT(0x75000025)) /*!< \brief message 0x75000025*/
//#define ERR_MSG_CAN_MON_RX_WRONG_SLOT                                               (MSG_CODE_ADAPT(0x75000026)) /*!< \brief message 0x75000026*/
//#define ERR_MSG_CAN_MON_TX_WRONG_SLOT                                               (MSG_CODE_ADAPT(0x75000027)) /*!< \brief message 0x75000027*/
//#define ERR_MSG_CAN_MAIN_MSG_OBJ_RX_DATA_CNFG                                       (MSG_CODE_ADAPT(0x75000028)) /*!< \brief message 0x75000028*/
//#define ERR_MSG_CAN_MAIN_MSG_OBJ_TX_DATA_CNFG                                       (MSG_CODE_ADAPT(0x75000029)) /*!< \brief message 0x75000029*/
//#define ERR_MSG_CAN_MON_MSG_OBJ_RX_DATA_CNFG                                        (MSG_CODE_ADAPT(0x7500002A)) /*!< \brief message 0x7500002A*/
//#define ERR_MSG_CAN_MON_MSG_OBJ_TX_DATA_CNFG                                        (MSG_CODE_ADAPT(0x7500002B)) /*!< \brief message 0x7500002B*/
//#define ERR_MSG_CAN_CONFIG_MODULE_WRONG_TABLE_POS                                   (MSG_CODE_ADAPT(0x7500002C)) /*!< \brief message 0x7500002C*/
//#define ERR_MSG_CAN_ASYNC_RX_QUEUE_OVERFLOW                                         (MSG_CODE_ADAPT(0x7500002D)) /*!< \brief message 0x7500002D*/
//#define ERR_MSG_CAN_TX_ASYNC_MSG_QUEUE_OVERFLOW                                     (MSG_CODE_ADAPT(0x7500002E)) /*!< \brief message 0x7500002E*/
//#define ERR_MSG_CAN_ASYNC_CTRL_TX_QUEUE_OVERFLOW                                    (MSG_CODE_ADAPT(0x7500002F)) /*!< \brief message 0x7500002F*/
//#define ERR_MSG_CAN_ASYNC_DATA_TX_QUEUE_OVERFLOW                                    (MSG_CODE_ADAPT(0x75000030)) /*!< \brief message 0x75000030*/
//#define ERR_MSG_CAN_ASYNC_DATA_TX_QUEUE_DLC                                         (MSG_CODE_ADAPT(0x75000031)) /*!< \brief message 0x75000031*/
//#define ERR_MSG_CAN_CONFIG_MODULE_TYPE_NOT_FOUND                                    (MSG_CODE_ADAPT(0x75000032)) /*!< \brief message 0x75000032*/
//#define ERR_MSG_CAN_CONFIG_MODULE_TYPE_NOT_MAIN                                     (MSG_CODE_ADAPT(0x75000033)) /*!< \brief message 0x75000033*/
//#define ERR_MSG_CAN_DLOAD_UNKNOWN_COMPONENT                                         (MSG_CODE_ADAPT(0x75000034)) /*!< \brief message 0x75000034*/
//#define ERR_MSG_CAN_DLOAD_IN_PROGRESS                                               (MSG_CODE_ADAPT(0x75000035)) /*!< \brief message 0x75000035*/
//#define ERR_MSG_CAN_DLOAD_BUF_FULL                                                  (MSG_CODE_ADAPT(0x75000036)) /*!< \brief message 0x75000036*/
//#define ERR_MSG_CAN_DLOAD_BUF_OVERFLOW                                              (MSG_CODE_ADAPT(0x75000037)) /*!< \brief message 0x75000037*/
//#define ERR_MSG_CAN_DLOAD_BUF_UNDERFLOW                                             (MSG_CODE_ADAPT(0x75000038)) /*!< \brief message 0x75000038*/
//#define ERR_MSG_CAN_DLOAD_BYTE_COUNT_NOT_MATCH                                      (MSG_CODE_ADAPT(0x75000039)) /*!< \brief message 0x75000039*/
//#define ERR_MSG_CAN_DLOAD_BAD_CRC                                                   (MSG_CODE_ADAPT(0x7500003A)) /*!< \brief message 0x7500003A*/
//#define ERR_MSG_CAN_INVALID_SYS_MSG_DLC                                             (MSG_CODE_ADAPT(0x7500003B)) /*!< \brief message 0x7500003B*/
//#define ERR_MSG_CAN_CONFIG_INVALID_SILENT_AWAKE_STATE                               (MSG_CODE_ADAPT(0x7500003C)) /*!< \brief message 0x7500003C*/
//#define ERR_MSG_CAN_CONFIG_SFTY_STAY_AWAKE                                          (MSG_CODE_ADAPT(0x7500003D)) /*!< \brief message 0x7500003D*/
//#define ERR_MSG_CAN_RECONNECT_UNEXPECTED_BAUDRATE                                   (MSG_CODE_ADAPT(0x7500003E)) /*!< \brief message 0x7500003E*/
//#define ERR_MSG_CAN_ASYNC_TNL_INVALID_COMMAND                                       (MSG_CODE_ADAPT(0x7500003F)) /*!< \brief message 0x7500003F*/
//#define ERR_MSG_CAN_ASYNC_TNL_BUSY                                                  (MSG_CODE_ADAPT(0x75000040)) /*!< \brief message 0x75000040*/
//#define ERR_MSG_CAN_ULOAD_BLOCK_NACK                                                (MSG_CODE_ADAPT(0x75000041)) /*!< \brief message 0x75000041*/
//#define ERR_MSG_CAN_MAIN_SYS_RX_LOST                                                (MSG_CODE_ADAPT(0x75000042)) /*!< \brief message 0x75000042*/
//#define ERR_MSG_CAN_CHANGING_TO_INVALID_MODULE_NR                            		(MSG_CODE_ADAPT(0x75000043)) /*!< \brief message 0x75000043*/
//#define ERR_MSG_CAN_MAIN_ASYNC_CTRL_RX_LOST                                         (MSG_CODE_ADAPT(0x75000044)) /*!< \brief message 0x75000044*/
//#define ERR_MSG_CAN_MAIN_ASYNC_DATA_RX_LOST                                         (MSG_CODE_ADAPT(0x75000045)) /*!< \brief message 0x75000045*/
//#define ERR_MSG_CAN_CONFIG_CHANGE_MODULE_IN_OPERATION                               (MSG_CODE_ADAPT(0x75000046)) /*!< \brief message 0x75000046*/
//#define ERR_MSG_CAN_MODULE_NR_TUPLE_ID_INVALID                                      (MSG_CODE_ADAPT(0x75000047)) /*!< \brief message 0x75000047*/
//#define ERR_MSG_CAN_DLOAD_DENIED                                                    (MSG_CODE_ADAPT(0x75000048)) /*!< \brief message 0x75000048*/
//#define ERR_MSG_CAN_DLOAD_IN_PROGRESS_NO_DL                                         (MSG_CODE_ADAPT(0x75000049)) /*!< \brief message 0x75000049*/
//#define ERR_MSG_CAN_DLOAD_IN_PROGRESS_ST_REQ1                                       (MSG_CODE_ADAPT(0x7500004A)) /*!< \brief message 0x7500004A*/
//#define ERR_MSG_CAN_DLOAD_IN_PROGRESS_ST_REQ2                                       (MSG_CODE_ADAPT(0x7500004B)) /*!< \brief message 0x7500004B*/
//#define ERR_MSG_CAN_DLOAD_IN_PROGRESS_ST_REQ1_2                                     (MSG_CODE_ADAPT(0x7500004C)) /*!< \brief message 0x7500004C*/
//#define ERR_MSG_CAN_DLOAD_IN_PROGRESS_ERASURE                                       (MSG_CODE_ADAPT(0x7500004D)) /*!< \brief message 0x7500004D*/
//#define ERR_MSG_CAN_DLOAD_IN_PROGRESS_START_ACK                                     (MSG_CODE_ADAPT(0x7500004E)) /*!< \brief message 0x7500004E*/
//#define ERR_MSG_CAN_DLOAD_IN_PROGRESS_DL_DATA                                       (MSG_CODE_ADAPT(0x7500004F)) /*!< \brief message 0x7500004F*/
//#define ERR_MSG_CAN_DLOAD_IN_PROGRESS_BLOCK_ACK                                     (MSG_CODE_ADAPT(0x75000050)) /*!< \brief message 0x75000050*/
//#define ERR_MSG_CAN_DLOAD_IN_PROGRESS_DL_END                                        (MSG_CODE_ADAPT(0x75000051)) /*!< \brief message 0x75000051*/
//#define ERR_MSG_CAN_CONNECT_STARTUP_125kB                                           (MSG_CODE_ADAPT(0x75000052)) /*!< \brief message 0x75000052*/
//#define ERR_MSG_CAN_CONNECT_STARTUP_250kB                                           (MSG_CODE_ADAPT(0x75000053)) /*!< \brief message 0x75000053*/
//#define ERR_MSG_CAN_CONNECT_STARTUP_500kB                                           (MSG_CODE_ADAPT(0x75000054)) /*!< \brief message 0x75000054*/
//#define ERR_MSG_CAN_CONNECT_STARTUP_1000kB                                          (MSG_CODE_ADAPT(0x75000055)) /*!< \brief message 0x75000055*/
//#define ERR_MSG_CAN_CONNECT_ONLINE_125kB                                            (MSG_CODE_ADAPT(0x75000056)) /*!< \brief message 0x75000056*/
//#define ERR_MSG_CAN_CONNECT_ONLINE_250kB                                            (MSG_CODE_ADAPT(0x75000057)) /*!< \brief message 0x75000057*/
//#define ERR_MSG_CAN_CONNECT_ONLINE_500kB                                            (MSG_CODE_ADAPT(0x75000058)) /*!< \brief message 0x75000058*/
//#define ERR_MSG_CAN_CONNECT_ONLINE_1000kB                                           (MSG_CODE_ADAPT(0x75000059)) /*!< \brief message 0x75000059*/
//#define ERR_MSG_CAN_ASYNC_UNDEFINED_CONNECTION_TYPE                                 (MSG_CODE_ADAPT(0x7500005A)) /*!< \brief message 0x7500005A*/
//#define ERR_MSG_CAN_CONFIG_MODULE_NOT_FOUND                                         (MSG_CODE_ADAPT(0x7500005B)) /*!< \brief message 0x7500005B*/
//#define ERR_MSG_CAN_STATE_MACHINE_ERROR                                             (MSG_CODE_ADAPT(0x7500005C)) /*!< \brief message 0x7500005C*/
//#define ERR_MSG_CAN_DLOAD_FLASH_WRITE_MAIN                                          (MSG_CODE_ADAPT(0x7500005D)) /*!< \brief message 0x7500005D*/
//#define ERR_MSG_CAN_DLOAD_FLASH_WRITE_MON                                           (MSG_CODE_ADAPT(0x7500005E)) /*!< \brief message 0x7500005E*/
//#define ERR_MSG_CAN_DLOAD_FLASH_READ_MAIN                                           (MSG_CODE_ADAPT(0x7500005F)) /*!< \brief message 0x7500005F*/
//#define ERR_MSG_CAN_DLOAD_FLASH_READ_MON                                            (MSG_CODE_ADAPT(0x75000060)) /*!< \brief message 0x75000060*/
//#define ERR_MSG_CAN_DLOAD_FLASH_ERASE_MAIN                                          (MSG_CODE_ADAPT(0x75000061)) /*!< \brief message 0x75000061*/
//#define ERR_MSG_CAN_DLOAD_FLASH_ERASE_MON                                           (MSG_CODE_ADAPT(0x75000062)) /*!< \brief message 0x75000062*/
//#define ERR_MSG_CAN_DLOAD_MAINMON_CONNECTION                                        (MSG_CODE_ADAPT(0x75000063)) /*!< \brief message 0x75000063*/
//#define ERR_MSG_CAN_ASYNC_UNKNOWN_COMMAND                                           (MSG_CODE_ADAPT(0x75000064)) /*!< \brief message 0x75000064*/
//#define ERR_MSG_CAN_SYS_UNKNOWN_COMMAND                                             (MSG_CODE_ADAPT(0x75000065)) /*!< \brief message 0x75000065*/
//#define ERR_MSG_CAN_MODULE_TYPE_CHANGE_FAILED                                       (MSG_CODE_ADAPT(0x75000066)) /*!< \brief message 0x75000066*/
//#define ERR_MSG_CAN_BAUD_RATE_CONFIG_INVALID                                        (MSG_CODE_ADAPT(0x75000067)) /*!< \brief message 0x75000067*/
//#define ERR_MSG_CAN_NOT_IMPLEMENTED                                                 (MSG_CODE_ADAPT(0x75000068)) /*!< \brief message 0x75000068*/
//#define ERR_MSG_CAN_IO_CONFIG_INVALID_CRC                                           (MSG_CODE_ADAPT(0x75000069)) /*!< \brief message 0x75000069*/
//#define ERR_MSG_CAN_SYNC_PROCESS_IMAGE_NOT_VALID                                    (MSG_CODE_ADAPT(0x7500006A)) /*!< \brief message 0x7500006A*/
//#define ERR_MSG_CAN_MODULE_NR_CONFIGURATION_FAILED                                  (MSG_CODE_ADAPT(0x7500006B)) /*!< \brief message 0x7500006B*/
//#define ERR_MSG_CAN_SYSTEM_MSG_NOT_ALLOWED_BROADCAST                                (MSG_CODE_ADAPT(0x7500006C)) /*!< \brief message 0x7500006C*/
//#define ERR_MSG_CAN_ASYNC_TUNNEL_HNDL_NOT_DEFINED                                   (MSG_CODE_ADAPT(0x7500006D)) /*!< \brief message 0x7500006D*/
//#define ERR_MSG_CAN_SYSTEM_MSG_NOT_IMPLEMENTED                                      (MSG_CODE_ADAPT(0x7500006E)) /*!< \brief message 0x7500006E*/
//#define ERR_MSG_CAN_SYS_ERR_QUEUE_FULL                                              (MSG_CODE_ADAPT(0x7500006F)) /*!< \brief message 0x7500006F*/
//#define ERR_MSG_CAN_ERR_NOT_INITIALIZED                                             (MSG_CODE_ADAPT(0x75000070)) /*!< \brief message 0x75000070*/
//#define ERR_MSG_CAN_ERR_NO_ENTRY                                                    (MSG_CODE_ADAPT(0x75000071)) /*!< \brief message 0x75000071*/
//#define ERR_MSG_CAN_DLOAD_ADD_HANDLE                                                (MSG_CODE_ADAPT(0x75000072)) /*!< \brief message 0x75000072*/
//#define ERR_MSG_CAN_ULOAD_ADD_HANDLE                                                (MSG_CODE_ADAPT(0x75000073)) /*!< \brief message 0x75000073*/
//#define ERR_MSG_CAN_XLOAD_INFO_BAD_CRC                                              (MSG_CODE_ADAPT(0x75000074)) /*!< \brief message 0x75000074*/

/*--------------------------------------------------------------------------*/
/* structure/type definitions                                               */
/*--------------------------------------------------------------------------*/

/*--------------------------------------------------------------------------*/
/* global variables                                                         */
/*--------------------------------------------------------------------------*/

/*--------------------------------------------------------------------------*/
/* function prototypes                                                      */
/*--------------------------------------------------------------------------*/

#ifdef __cplusplus
}// closing brace for extern "C"
#endif // #ifdef __cplusplus

#endif // #ifndef LI_CAN_SLV_MSG_CODE_H_

/** @} */

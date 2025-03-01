#include "cp_core.h"
#include "cp_msg.h"
#include "canpie.h"

#include <string.h>

#ifndef CP_DEBUG
#define CP_DEBUG  1
#endif

#if CP_DEBUG > 0
#include <stdio.h>
#endif

#include <stdlib.h>

#ifndef CP_RETRANSMIT_WHEN_BUSY
#define CP_RETRANSMIT_WHEN_BUSY  0
#endif

// Definitions
#define HAL_CAN_TIMEOUT_VALUE  10U
#define MAX_CAN_FILTER_NUMBER	14
#define BUFFER_TX	(0)
#define BUFFER_RX	(1)
#define BUFFER_NONE	(-1)

#define TX_MAILBOX_0 (0)
#define TX_MAILBOX_1 (1)
#define TX_MAILBOX_2 (2)

#define  CP_BUFFER_IVAL		((uint32_t)0x00000000) // buffer invalid
#define  CP_BUFFER_VAL		((uint32_t)0x00000001) // buffer valid
#define  CP_BUFFER_TRM		((uint32_t)0x00000002) // Tx buffer
#define  CP_BUFFER_RCV		((uint32_t)0x00000004) // Rx buffer

#define  CP_BUFFER_PND		((uint32_t)0x00000020) // Tx buffer pending
#define  CP_BUFFER_UPD		((uint32_t)0x00000040) // Rx buffer update

#define  CP_BUFFER_MAIN_TX_MON_RX_PND	((uint32_t)0x10000000) // TEST flag for emulating TX MAIN -> RX MON

#if CP_STATISTIC > 0
static uint32_t tx1_counter;
static uint32_t rx1_counter;
static uint32_t err1_counter;
#endif

struct hal_baudrate
{
	enum CpBitrate_e bitrate;
	uint32_t prescaler;
	uint32_t sjw;
	uint32_t bs1;
	uint32_t bs2;
};

enum DrvInfo_e
{
	eDRV_INFO_OFF = 0, eDRV_INFO_INIT, eDRV_INFO_ACTIVE
};


//-------------------------------------------------------------------
// simulation of CAN message buffer
//
static CpCanMsg_ts atsCan1MsgS[CP_BUFFER_MAX];
static CpFifo_ts *aptsCan1FifoS[CP_BUFFER_MAX];

static CpCanMsg_ts atsCan2MsgS[CP_BUFFER_MAX];
static CpFifo_ts *aptsCan2FifoS[CP_BUFFER_MAX];

/*!
 ** \var aptsPortS
 **
 ** For interrupts we need to know the CAN port information,
 ** so store the pointer to the corresponding global port information.
 */

//static CpPort_ts *aptsPortS[CP_CHANNEL_MAX] = {0};
static CpPort_ts *aptsPortS[2] = {0};

//-------------------------------------------------------------------
// these pointers store the callback handlers
//

static CpRcvHandler_Fn pfnCan1RcvHandlerS = CPP_NULL;
static CpTrmHandler_Fn pfnCan1TrmHandlerS = CPP_NULL;
static CpErrHandler_Fn pfnCan1ErrHandlerS = CPP_NULL;

static CpRcvHandler_Fn pfnCan2RcvHandlerS = CPP_NULL;
static CpTrmHandler_Fn pfnCan2TrmHandlerS = CPP_NULL;
static CpErrHandler_Fn pfnCan2ErrHandlerS = CPP_NULL;

static FILE *can_main_hw_log;
static char can_main_hw_log_file_path[FILENAME_MAX];

static FILE *cp_driver_log;
static char cp_driver_log_file_path[FILENAME_MAX];
static char cp_driver_log_file_name[] = "cp_driver.log";

/**/

/*----------------------------------------------------------------------------*\
** Function implementation                                                    **
 **                                                                            **
 \*----------------------------------------------------------------------------*/
static CpStatus_tv CheckParam(const CpPort_ts *ptsPortV, const uint8_t ubBufferIdxV, const uint8_t unReqStateV)
{
	CpStatus_tv tvStatusT = eCP_ERR_CHANNEL;

	//----------------------------------------------------------------
	// test CAN port
	//
	if (ptsPortV != (CpPort_ts *) 0L)
	{
		tvStatusT = eCP_ERR_INIT_MISSING;

		//--------------------------------------------------------
		// check for initialization
		//
		if (ptsPortV->ubDrvInfo >= unReqStateV)
		{
			tvStatusT = eCP_ERR_BUFFER;

			//------------------------------------------------
			// check for valid buffer number
			//
			if (ubBufferIdxV < CP_BUFFER_MAX)
			{
				tvStatusT = eCP_ERR_NONE;
			}
		}
	}

	return (tvStatusT);
}

//----------------------------------------------------------------------------//
// CpCoreBitrate()                                                            //
//                                                                            //
//----------------------------------------------------------------------------//
CpStatus_tv CpCoreBitrate(CpPort_ts *ptsPortV, int32_t slNomBitRateV, int32_t slDatBitRateV)
{
	CpStatus_tv tvStatusT = eCP_ERR_CHANNEL;

	//----------------------------------------------------------------
	// test CAN port
	//
	if (ptsPortV != (CpPort_ts *) 0L)
	{
		if (ptsPortV->ubDrvInfo > eDRV_INFO_OFF)
		{
			tvStatusT = eCP_ERR_NONE;

			//-----------------------------------------------------
			// test bit-rate
			//
			if ((slNomBitRateV > eCP_BITRATE_1M) || (slNomBitRateV == eCP_BITRATE_NONE))
			{
				tvStatusT = eCP_ERR_BITRATE;
			}
			if ((slDatBitRateV != eCP_BITRATE_NONE) && (slNomBitRateV > slDatBitRateV))
			{
				tvStatusT = eCP_ERR_BITRATE;
			}

			//-----------------------------------------------------
			// configure the btr register here
			//
			if (tvStatusT == eCP_ERR_NONE)
			{
				/* TODO: DO ME! */
			}
		}
		else
		{
			tvStatusT = eCP_ERR_INIT_MISSING;
		}
	}

	return (tvStatusT);
}

//----------------------------------------------------------------------------//
// CpCoreBufferConfig()                                                       //
//                                                                            //
//----------------------------------------------------------------------------//
CpStatus_tv CpCoreBufferConfig(CpPort_ts *ptsPortV, uint8_t ubBufferIdxV, uint32_t ulIdentifierV, uint32_t ulAcceptMaskV, uint8_t ubFormatV, uint8_t ubDirectionV)
{
	CpStatus_tv tvStatusT;

	//----------------------------------------------------------------
	// test parameter ptsPortV and ubBufferIdxV
	//
	tvStatusT = CheckParam(ptsPortV, ubBufferIdxV, eDRV_INFO_INIT);
	if (tvStatusT == eCP_ERR_NONE)
	{
		//--------------------------------------------------------
		// test message format and mask identifier
		//

		if (cp_driver_log != NULL)
		{
			fprintf(cp_driver_log, "CpCoreBufferConfig: msg_obj = %d, can_id = %d, ", ubBufferIdxV, ulIdentifierV);
			fprintf(cp_driver_log, "acceptance_mask = %d, dir = %d\r\n", ulAcceptMaskV, ubDirectionV);
		}

		switch (ubFormatV & CP_MASK_MSG_FORMAT)
		{
			case CP_MSG_FORMAT_CBFF:
			case CP_MSG_FORMAT_FBFF:
				ulIdentifierV = ulIdentifierV & CP_MASK_STD_FRAME;
				ulAcceptMaskV = ulAcceptMaskV & CP_MASK_STD_FRAME;
				break;

			case CP_MSG_FORMAT_CEFF:
			case CP_MSG_FORMAT_FEFF:
				ulIdentifierV = ulIdentifierV & CP_MASK_EXT_FRAME;
				ulAcceptMaskV = ulAcceptMaskV & CP_MASK_EXT_FRAME;
				break;
		}

		switch (ptsPortV->ubPhyIf)
		{
			case eCP_CHANNEL_1:
				// save identifier
				atsCan1MsgS[ubBufferIdxV].ulIdentifier = ulIdentifierV;

				// save format in message control
				atsCan1MsgS[ubBufferIdxV].ubMsgCtrl = ubFormatV;

				switch (ubDirectionV)
				{
					case eCP_BUFFER_DIR_RCV:
						atsCan1MsgS[ubBufferIdxV].ulMsgUser = CP_BUFFER_VAL | CP_BUFFER_RCV;
						break;

					case eCP_BUFFER_DIR_TRM:
						atsCan1MsgS[ubBufferIdxV].ulMsgUser = CP_BUFFER_VAL | CP_BUFFER_TRM;
						break;
				}
				break;
			case eCP_CHANNEL_2:
				atsCan2MsgS[ubBufferIdxV].ulIdentifier = ulIdentifierV;
				atsCan2MsgS[ubBufferIdxV].ubMsgCtrl = ubFormatV;

				switch (ubDirectionV)
				{
					case eCP_BUFFER_DIR_RCV:
						atsCan2MsgS[ubBufferIdxV].ulMsgUser = CP_BUFFER_VAL | CP_BUFFER_RCV;
						break;

					case eCP_BUFFER_DIR_TRM:
						atsCan2MsgS[ubBufferIdxV].ulMsgUser = CP_BUFFER_VAL | CP_BUFFER_TRM;
						break;
				}
				break;
			default:
				break;
		}
	}
	return (tvStatusT);
}

//----------------------------------------------------------------------------//
// CpCoreBufferGetData()                                                      //
//                                                                            //
//----------------------------------------------------------------------------//
CpStatus_tv CpCoreBufferGetData(CpPort_ts *ptsPortV, uint8_t ubBufferIdxV, uint8_t *pubDestDataV, uint8_t ubStartPosV, uint8_t ubSizeV)
{
	CpStatus_tv tvStatusT;
	uint8_t ubCntT;

	//----------------------------------------------------------------
	// test parameter ptsPortV and ubBufferIdxV
	//
	tvStatusT = CheckParam(ptsPortV, ubBufferIdxV, eDRV_INFO_INIT);
	if (tvStatusT == eCP_ERR_NONE)
	{
		//--------------------------------------------------------
		// test start position and size
		//
		if ((ubStartPosV + ubSizeV) > CP_DATA_SIZE)
		{
			tvStatusT = eCP_ERR_PARAM;
		}
		else
		{
			switch (ptsPortV->ubPhyIf)
			{
				case eCP_CHANNEL_1:
					for (ubCntT = ubStartPosV; ubCntT < ubSizeV; ubCntT++)
					{
						*pubDestDataV = CpMsgGetData(&atsCan1MsgS[ubBufferIdxV], ubCntT);
						pubDestDataV++;
					}

					break;

				case eCP_CHANNEL_2:
					for (ubCntT = ubStartPosV; ubCntT < ubSizeV; ubCntT++)
					{
						*pubDestDataV = CpMsgGetData(&atsCan2MsgS[ubBufferIdxV], ubCntT);
						pubDestDataV++;
					}
					break;

				default:
					return (eCP_ERR_CHANNEL);
			}
		}
	}

	return (tvStatusT);
}

//----------------------------------------------------------------------------//
// CpCoreBufferGetDlc()                                                       //
//                                                                            //
//----------------------------------------------------------------------------//
CpStatus_tv CpCoreBufferGetDlc(CpPort_ts *ptsPortV, uint8_t ubBufferIdxV, uint8_t *pubDlcV)
{
	CpStatus_tv tvStatusT;

	tvStatusT = CheckParam(ptsPortV, ubBufferIdxV, eDRV_INFO_INIT);
	if (tvStatusT == eCP_ERR_NONE)
	{
		//----------------------------------------------------------------
		// read DLC from simulated CAN buffer
		//
		switch (ptsPortV->ubPhyIf)
		{
			case eCP_CHANNEL_1:
				*pubDlcV = atsCan1MsgS[ubBufferIdxV].ubMsgDLC;
				break;

			case eCP_CHANNEL_2:
				*pubDlcV = atsCan2MsgS[ubBufferIdxV].ubMsgDLC;
				break;

			default:
				tvStatusT = eCP_ERR_CHANNEL;
		}
	}

	return (tvStatusT);
}

//----------------------------------------------------------------------------//
// CpCoreBufferRelease()                                                      //
//                                                                            //
//----------------------------------------------------------------------------//
CpStatus_tv CpCoreBufferRelease(CpPort_ts *ptsPortV, uint8_t ubBufferIdxV)
{
	CpStatus_tv tvStatusT;

	tvStatusT = CheckParam(ptsPortV, ubBufferIdxV, eDRV_INFO_INIT);
	if (tvStatusT == eCP_ERR_NONE)
	{
		//-----------------------------------------------------------------
		// clear the message buffer structure
		// clearing bit 0 of ulMsgUser disables the buffer
		//
		switch (ptsPortV->ubPhyIf)
		{
			case eCP_CHANNEL_1:
				memset(&atsCan1MsgS[ubBufferIdxV], 0x00, sizeof(CpCanMsg_ts));
				break;

			case eCP_CHANNEL_2:
				memset(&atsCan2MsgS[ubBufferIdxV], 0x00, sizeof(CpCanMsg_ts));
				break;

			default:
				return (eCP_ERR_CHANNEL);
		}
	}

	return (tvStatusT);
}

//----------------------------------------------------------------------------//
// CpCoreBufferSend()                                                         //
// send message out of the CAN controller                                     //
//----------------------------------------------------------------------------//
CpStatus_tv CpCoreBufferSend(CpPort_ts *ptsPortV, uint8_t ubBufferIdxV)
{
	uint16_t i;
	CpStatus_tv tvStatusT;
//	uint32_t tx_mailbox;
	uint16_t dlc;

	//----------------------------------------------------------------
	// test parameter ptsPortV and ubBufferIdxV
	//
	tvStatusT = CheckParam(ptsPortV, ubBufferIdxV, eDRV_INFO_INIT);

	//	// check if buffer is used for tx and valid
	//	if((atsCan1MsgS[ubBufferIdxV].ulMsgUser & (CP_BUFFER_VAL | CP_BUFFER_TRM)) != (CP_BUFFER_VAL | CP_BUFFER_TRM))
	//	{
	//		return eCP_ERR_BUFFER;
	//	}

	if (ptsPortV->ubPhyIf == eCP_CHANNEL_1)
	{
		if (can_main_hw_log != NULL)
		{

			fprintf(can_main_hw_log, "CpCoreBufferSend: %d ", ubBufferIdxV);

			dlc = atsCan1MsgS[ubBufferIdxV].ubMsgDLC;

			if (tvStatusT == eCP_ERR_NONE)
			{
				//-----------------------------------------------------------------
				// setup identifier
				//
				if (CpMsgIsExtended(&atsCan1MsgS[ubBufferIdxV]))
				{
					fprintf(can_main_hw_log, "ext_can_id = 0x%X, dlc = %d, ", atsCan1MsgS[ubBufferIdxV].ulIdentifier, dlc);
				}
				else
				{
					fprintf(can_main_hw_log, "std_can_id = 0x%X, dlc = %d, ", atsCan1MsgS[ubBufferIdxV].ulIdentifier, dlc);
				}

				//-----------------------------------------------------------------
				// check for RTR bit
				//
				if (CpMsgIsRemote(&atsCan1MsgS[ubBufferIdxV]))
				{
					/* TODO: DO ME! */
				}
				else
				{
					/* TODO: DO ME! */
				}

				for (i = 0; i < dlc; i++)
				{
					fprintf(can_main_hw_log, "%02X ", atsCan1MsgS[ubBufferIdxV].tuMsgData.aubByte[i]);
				}

				fprintf(can_main_hw_log, "\r\n");

				if (pfnCan2RcvHandlerS != CPP_NULL)
				{
					/* copy MAIN TX to MON RX buffer */
					for (uint32_t buffer_id = 0; buffer_id < CP_BUFFER_MAX; buffer_id++)
					{
						if (atsCan2MsgS[buffer_id].ulIdentifier == atsCan1MsgS[ubBufferIdxV].ulIdentifier )
						{
							atsCan2MsgS[buffer_id].ubMsgCtrl = atsCan1MsgS[ubBufferIdxV].ubMsgCtrl;
							atsCan2MsgS[buffer_id].ubMsgDLC = atsCan1MsgS[ubBufferIdxV].ubMsgDLC;
							atsCan2MsgS[buffer_id].tuMsgData = atsCan1MsgS[ubBufferIdxV].tuMsgData;
							atsCan2MsgS[buffer_id].ulMsgUser |= CP_BUFFER_MAIN_TX_MON_RX_PND;
							break;
						}
					}
				}
			}
			fflush(can_main_hw_log);
		}
	}

	if (ptsPortV->ubPhyIf == eCP_CHANNEL_2)
	{
		while(1);
	}

	return (tvStatusT);
}

//----------------------------------------------------------------------------//
// CpCoreBufferSetData()                                                      //
//                                                                            //
//----------------------------------------------------------------------------//
CpStatus_tv CpCoreBufferSetData(CpPort_ts *ptsPortV, uint8_t ubBufferIdxV, uint8_t *pubSrcDataV, uint8_t ubStartPosV, uint8_t ubSizeV)
{
	CpStatus_tv tvStatusT;
	uint8_t ubCntT;

	//----------------------------------------------------------------
	// test parameter ptsPortV and ubBufferIdxV
	//
	tvStatusT = CheckParam(ptsPortV, ubBufferIdxV, eDRV_INFO_INIT);
	if (tvStatusT == eCP_ERR_NONE)
	{
		//--------------------------------------------------------
		// test start position and size
		//
		if ((ubStartPosV + ubSizeV) > CP_DATA_SIZE)
		{
			tvStatusT = eCP_ERR_PARAM;
		}
		else
		{
			switch (ptsPortV->ubPhyIf)
			{
				case eCP_CHANNEL_1:
					for (ubCntT = ubStartPosV; ubCntT < ubSizeV; ubCntT++)
					{
						CpMsgSetData(&atsCan1MsgS[ubBufferIdxV], ubCntT, *pubSrcDataV);
						pubSrcDataV++;
					}
					break;

				case eCP_CHANNEL_2:
					for (ubCntT = ubStartPosV; ubCntT < ubSizeV; ubCntT++)
					{
						CpMsgSetData(&atsCan2MsgS[ubBufferIdxV], ubCntT, *pubSrcDataV);
						pubSrcDataV++;
					}
					break;

				default:
					tvStatusT = eCP_ERR_CHANNEL;
			}
		}
	}

	return (tvStatusT);
}

//----------------------------------------------------------------------------//
// CpCoreBufferSetDlc()                                                       //
//                                                                            //
//----------------------------------------------------------------------------//
CpStatus_tv CpCoreBufferSetDlc(CpPort_ts *ptsPortV, uint8_t ubBufferIdxV, uint8_t ubDlcV)
{
	CpStatus_tv tvStatusT;

	//----------------------------------------------------------------
	// test parameter ptsPortV and ubBufferIdxV
	//
	tvStatusT = CheckParam(ptsPortV, ubBufferIdxV, eDRV_INFO_INIT);
	if (tvStatusT == eCP_ERR_NONE)
	{
		switch (ptsPortV->ubPhyIf)
		{
			case eCP_CHANNEL_1:
				atsCan1MsgS[ubBufferIdxV].ubMsgDLC = ubDlcV;
				break;

			case eCP_CHANNEL_2:
				atsCan2MsgS[ubBufferIdxV].ubMsgDLC = ubDlcV;
				break;

			default:
				tvStatusT = eCP_ERR_CHANNEL;
		}
	}
	return (tvStatusT);
}

//----------------------------------------------------------------------------//
// CpCoreCanMode()                                                            //
//                                                                            //
//----------------------------------------------------------------------------//
CpStatus_tv CpCoreCanMode(CpPort_ts *ptsPortV, uint8_t ubModeV)
{
	CpStatus_tv tvStatusT = eCP_ERR_CHANNEL;

	//----------------------------------------------------------------
	// test CAN port
	//
	if (ptsPortV != (CpPort_ts *) 0L)
	{
		if (ptsPortV->ubDrvInfo > eDRV_INFO_OFF)
		{
			tvStatusT = eCP_ERR_NONE;
			//----------------------------------------------------------------
			// switch CAN controller into mode "ubModeV"
			//
			switch (ubModeV)
			{
				//--------------------------------------------------------
				// Stop the CAN controller (passive on the bus)
				//
				case eCP_MODE_STOP:
					break;

				//--------------------------------------------------------
				// Start the CAN controller (active on the bus)
				//
				case eCP_MODE_OPERATION:
					break;

				//--------------------------------------------------------
				// Start the CAN controller (Listen-Only)
				//
				case eCP_MODE_LISTEN_ONLY:
					break;

				//--------------------------------------------------------
				// Other modes are not supported
				//
				default:
					return (eCP_ERR_NOT_SUPPORTED);
					break;
			}
		}
	}

	return (tvStatusT);
}

//----------------------------------------------------------------------------//
// CpCoreCanState()                                                           //
//                                                                            //
//----------------------------------------------------------------------------//
CpStatus_tv CpCoreCanState(CpPort_ts *ptsPortV, CpState_ts *ptsStateV)
{
//	uint32_t hal_error;
//	uint32_t hal_esr;

	CpStatus_tv tvStatusT = eCP_ERR_CHANNEL;

	//----------------------------------------------------------------
	// test CAN port
	//
	if (ptsPortV != (CpPort_ts *) 0L)
	{
		if (ptsPortV->ubDrvInfo > eDRV_INFO_OFF)
		{
			tvStatusT = eCP_ERR_NONE;

			// first set the state of the bus
			ptsStateV->ubCanErrState = eCP_STATE_BUS_ACTIVE;
		}
	}

	return (tvStatusT);
}

//----------------------------------------------------------------------------//
// CpCoreDriverInit()                                                         //
// init CAN controller                                                        //
//----------------------------------------------------------------------------//
CpStatus_tv CpCoreDriverInit(uint8_t ubPhyIfV, CpPort_ts *ptsPortV, uint8_t ubConfigV)
{
	uint8_t i;
	CpStatus_tv tvStatusT = eCP_ERR_CHANNEL;
	(void)ubConfigV;

	(void) strncpy(cp_driver_log_file_path, cp_driver_log_file_name, sizeof(cp_driver_log_file_path));

	cp_driver_log = fopen(cp_driver_log_file_path, "wb+");

	if (NULL == cp_driver_log)
	{
		return tvStatusT;
	}

#if CP_STATISTIC > 0
		tx1_counter = 0;
		rx1_counter = 0;
		err1_counter = 0;
#endif

	//--------------------------------------------------------
	// test CAN port
	//
	if (ptsPortV != (CpPort_ts *) 0L)
	{
		if (ptsPortV->ubDrvInfo == eDRV_INFO_OFF)
		{
			switch (ubPhyIfV)
			{
				case eCP_CHANNEL_1:
					aptsPortS[0] = ptsPortV; /* MAIN port */
					ptsPortV->ubPhyIf = eCP_CHANNEL_1;
					ptsPortV->ubDrvInfo = eDRV_INFO_INIT;

					pfnCan1RcvHandlerS = CPP_NULL;
					pfnCan1TrmHandlerS = CPP_NULL;
					pfnCan1ErrHandlerS = CPP_NULL;

					// release all buffers and
					for (i = eCP_BUFFER_1; i < CP_BUFFER_MAX; i++)
					{
						CpCoreBufferRelease(ptsPortV, i);
					}
					tvStatusT = eCP_ERR_NONE;
					break;
				case eCP_CHANNEL_2:

					aptsPortS[1] = ptsPortV; /* MON port CpPort_ts * */
					ptsPortV->ubPhyIf = eCP_CHANNEL_2;
					ptsPortV->ubDrvInfo = eDRV_INFO_INIT;

					pfnCan2RcvHandlerS = CPP_NULL;
					pfnCan2TrmHandlerS = CPP_NULL;
					pfnCan2ErrHandlerS = CPP_NULL;

					for (i = eCP_BUFFER_1; i < CP_BUFFER_MAX; i++)
					{
						CpCoreBufferRelease(ptsPortV, i);
					}

					tvStatusT = eCP_ERR_NONE;
					break;
				default:
					break;
			}

			tvStatusT = eCP_ERR_NONE;
		}
		else
		{
			//---------------------------------------------
			// already initialized
			//
			tvStatusT = eCP_ERR_INIT_FAIL;
		}
	}
	else
	{
		//-----------------------------------------------------
		// parameter ptsPortV is not correct
		//
		tvStatusT = eCP_ERR_PARAM;
	}

	return (tvStatusT);
}

//----------------------------------------------------------------------------//
// CpCoreDriverRelease()                                                      //
//                                                                            //
//----------------------------------------------------------------------------//
CpStatus_tv CpCoreDriverRelease(CpPort_ts *ptsPortV)
{
	CpStatus_tv tvStatusT = eCP_ERR_CHANNEL;

	//----------------------------------------------------------------
	// test CAN port
	//
	if (ptsPortV != (CpPort_ts *) 0L)
	{

		if (cp_driver_log != NULL)
		{
			(void) fclose(cp_driver_log);
		}

		if (ptsPortV->ubDrvInfo > eDRV_INFO_OFF)
		{
			tvStatusT = CpCoreCanMode(ptsPortV, eCP_MODE_STOP);
			ptsPortV->ubDrvInfo = eDRV_INFO_OFF;

			/* release Buffers */
			for (uint32_t n = eCP_BUFFER_1; n < CP_BUFFER_MAX; n++)
			{
				CpCoreBufferRelease(ptsPortV, n);
			}

		}
		else
		{
			tvStatusT = eCP_ERR_INIT_MISSING;
		}
	}

	return (tvStatusT);
}

//----------------------------------------------------------------------------//
// CpCoreFifoConfig()                                                         //
//                                                                            //
//----------------------------------------------------------------------------//
CpStatus_tv CpCoreFifoConfig(CpPort_ts *ptsPortV, uint8_t ubBufferIdxV, CpFifo_ts *ptsFifoV)
{
	CpStatus_tv tvStatusT;

	//----------------------------------------------------------------
	// test parameter ptsPortV and ubBufferIdxV
	//
	tvStatusT = CheckParam(ptsPortV, ubBufferIdxV, eDRV_INFO_INIT);
	if (tvStatusT == eCP_ERR_NONE)
	{
		if (ptsFifoV != (CpFifo_ts *) 0)
		{
			switch (ptsPortV->ubPhyIf)
			{
				case eCP_CHANNEL_1:

					aptsCan1FifoS[ubBufferIdxV] = ptsFifoV;

					tvStatusT = eCP_ERR_NONE;
					break;
				case eCP_CHANNEL_2:

					aptsCan2FifoS[ubBufferIdxV] = ptsFifoV;

					tvStatusT = eCP_ERR_NONE;
					break;
				default:
					tvStatusT = eCP_ERR_CHANNEL;
					break;
			}
		}
		else
		{
			tvStatusT = eCP_ERR_FIFO_PARAM;
		}
	}

	return (tvStatusT);
}

//----------------------------------------------------------------------------//
// CpCoreFifoRead()                                                           //
//                                                                            //
//----------------------------------------------------------------------------//
CpStatus_tv CpCoreFifoRead(CpPort_ts *ptsPortV, uint8_t ubBufferIdxV, CpCanMsg_ts *ptsCanMsgV, uint32_t *pulBufferSizeV)
{
	CpFifo_ts *ptsFifoT;
	CpStatus_tv tvStatusT;
	CpCanMsg_ts *ptsCanMsgT;

	//----------------------------------------------------------------
	// test parameter ptsPortV and ubBufferIdxV
	//
	tvStatusT = CheckParam(ptsPortV, ubBufferIdxV, eDRV_INFO_INIT);
	if (tvStatusT == eCP_ERR_NONE)
	{
		if (pulBufferSizeV != (uint32_t *) 0L)
		{
			if (ptsCanMsgV != (CpCanMsg_ts *) 0L)
			{
				switch (ptsPortV->ubPhyIf)
				{
					case eCP_CHANNEL_1:
						ptsFifoT = aptsCan1FifoS[ubBufferIdxV];
						break;
					case eCP_CHANNEL_2:
						ptsFifoT = aptsCan2FifoS[ubBufferIdxV];
						break;
					default:
						tvStatusT = eCP_ERR_CHANNEL;
						break;
				}

				if (ptsFifoT == 0L)
				{
					return (eCP_ERR_FIFO_PARAM);
				}

				if (CpFifoIsEmpty(ptsFifoT) == 1)
				{
					//--------------------------------------------------------
					// FIFO is empty, no data has been copied
					//
					*pulBufferSizeV = 0;
					tvStatusT = eCP_ERR_FIFO_EMPTY;
				}
				else
				{
					ptsCanMsgT = CpFifoDataOutPtr(ptsFifoT);
					memcpy(ptsCanMsgV, ptsCanMsgT, sizeof(CpCanMsg_ts));
					CpFifoIncOut(ptsFifoT);
					*pulBufferSizeV = 1;
					tvStatusT = eCP_ERR_NONE;
				}
			}
		}
	}

	return (tvStatusT);
}

//----------------------------------------------------------------------------//
// CpCoreFifoRelease()                                                        //
//                                                                            //
//----------------------------------------------------------------------------//
CpStatus_tv CpCoreFifoRelease(CpPort_ts *ptsPortV, uint8_t ubBufferIdxV)
{
	CpStatus_tv tvStatusT;

	//----------------------------------------------------------------
	// test parameter ptsPortV and ubBufferIdxV
	//
	tvStatusT = CheckParam(ptsPortV, ubBufferIdxV, eDRV_INFO_INIT);
	if (tvStatusT == eCP_ERR_NONE)
	{
		switch (ptsPortV->ubPhyIf)
		{
			case eCP_CHANNEL_1:
				aptsCan1FifoS[ubBufferIdxV] = 0L;
				break;

			case eCP_CHANNEL_2:
				aptsCan2FifoS[ubBufferIdxV] = 0L;
				break;

			default:
				tvStatusT = eCP_ERR_CHANNEL;
		}
	}

	return (tvStatusT);
}

//----------------------------------------------------------------------------//
// CpCoreFifoWrite()                                                          //
//                                                                            //
//----------------------------------------------------------------------------//
CpStatus_tv CpCoreFifoWrite(CpPort_ts *ptsPortV, uint8_t ubBufferIdxV, CpCanMsg_ts *ptsCanMsgV, uint32_t *pulBufferSizeV)
{
	CpFifo_ts *ptsFifoT;
	CpCanMsg_ts *ptsCanMsgT;
	CpStatus_tv tvStatusT;

	//----------------------------------------------------------------
	// test parameter ptsPortV and ubBufferIdxV
	//
	tvStatusT = CheckParam(ptsPortV, ubBufferIdxV, eDRV_INFO_INIT);

	if (tvStatusT == eCP_ERR_NONE)
	{
		//		// check if buffer is valid
		//		if ((atsCan1MsgS[ubBufferIdxV].ulMsgUser & CP_BUFFER_VAL) != (CP_BUFFER_VAL))
		//		{
		//			return eCP_ERR_BUFFER;
		//		}
		//
		//
		if (pulBufferSizeV != (uint32_t *) 0L)
		{
			if (ptsCanMsgV != (CpCanMsg_ts *) 0L)
			{
				switch (ptsPortV->ubPhyIf)
				{
					case eCP_CHANNEL_1:
						ptsFifoT = aptsCan1FifoS[ubBufferIdxV];
						break;
					case eCP_CHANNEL_2:
						ptsFifoT = aptsCan2FifoS[ubBufferIdxV];
						break;
					default:
						tvStatusT = eCP_ERR_CHANNEL;
						break;
				}

				if (ptsFifoT == 0L)
				{
					return (eCP_ERR_FIFO_PARAM);
				}

				//----------------------------------------------------------------
				// check if buffer is busy
				//
				if ((atsCan1MsgS[ubBufferIdxV].ulMsgUser & CP_BUFFER_PND) > 0)
				{

					if (CpFifoIsFull(ptsFifoT) == 1)
					{
						//--------------------------------------------------------
						// FIFO is empty, no data has been copied
						//
						*pulBufferSizeV = 0;
						tvStatusT = eCP_ERR_FIFO_FULL;
					}
					else
					{
						ptsCanMsgT = CpFifoDataInPtr(ptsFifoT);
						memcpy(ptsCanMsgT, ptsCanMsgV, sizeof(CpCanMsg_ts));
						CpFifoIncIn(ptsFifoT);
						*pulBufferSizeV = 1;
						tvStatusT = eCP_ERR_NONE;
					}
				}
				else
				{
					memcpy(&atsCan1MsgS[ubBufferIdxV], ptsCanMsgV, sizeof(CpCanMsg_ts));
					CpCoreBufferSend(ptsPortV, ubBufferIdxV);
					tvStatusT = eCP_ERR_NONE;
				}
			}
		}
	}

	return (tvStatusT);
}

//----------------------------------------------------------------------------//
// CpCoreHDI()                                                                //
//                                                                            //
//----------------------------------------------------------------------------//
CpStatus_tv CpCoreHDI(CpPort_ts *ptsPortV, CpHdi_ts *ptsHdiV)
{
	CpStatus_tv tvStatusT = eCP_ERR_CHANNEL;

	//----------------------------------------------------------------
	// test CAN port
	//
	if (ptsPortV != (CpPort_ts *) 0L)
	{
		if (ptsPortV->ubDrvInfo > eDRV_INFO_OFF)
		{
			tvStatusT = eCP_ERR_NONE;

			if (ptsHdiV != (CpHdi_ts *) 0)
			{
				//--------------------------------------------------
				// get version number
				//
				ptsHdiV->ubVersionMajor = CP_VERSION_MAJOR;
				ptsHdiV->ubVersionMinor = CP_VERSION_MINOR;

				ptsHdiV->ubCanFeatures = 0;
				ptsHdiV->ubDriverFeatures = 0;

				ptsHdiV->ubBufferMax = CP_BUFFER_MAX;

			}
			else
			{
				tvStatusT = eCP_ERR_PARAM;
			}
		}
	}

	return (tvStatusT);
}

//----------------------------------------------------------------------------//
// CpCoreIntFunctions()                                                       //
//                                                                            //
//----------------------------------------------------------------------------//
CpStatus_tv CpCoreIntFunctions(CpPort_ts *ptsPortV, CpRcvHandler_Fn pfnRcvHandlerV, CpTrmHandler_Fn pfnTrmHandlerV, CpErrHandler_Fn pfnErrHandlerV)
{
	CpStatus_tv tvStatusT = eCP_ERR_NONE;

	//----------------------------------------------------------------
	// test CAN port
	//
	if (ptsPortV != (CpPort_ts *) 0L)
	{
		if (ptsPortV->ubDrvInfo > eDRV_INFO_OFF)
		{
			//-----------------------------------------------------
			// store the new callback
			//
			switch (ptsPortV->ubPhyIf)
			{
				case eCP_CHANNEL_1:
					pfnCan1RcvHandlerS = pfnRcvHandlerV;
					pfnCan1TrmHandlerS = pfnTrmHandlerV;
					pfnCan1ErrHandlerS = pfnErrHandlerV;
					break;
				case eCP_CHANNEL_2:
					pfnCan2RcvHandlerS = pfnRcvHandlerV;
					pfnCan2TrmHandlerS = pfnTrmHandlerV;
					pfnCan2ErrHandlerS = pfnErrHandlerV;
					break;
				default:
					tvStatusT = eCP_ERR_CHANNEL;
					break;
			}
		}
	}
	return (tvStatusT);
}


//----------------------------------------------------------------------------//
// CpCoreStatistic()                                                          //
// return statistical information                                             //
//----------------------------------------------------------------------------//
CpStatus_tv CpCoreStatistic(CpPort_ts *ptsPortV, CpStatistic_ts *ptsStatsV)
{
	CpStatus_tv tvStatusT = eCP_ERR_CHANNEL;

	//----------------------------------------------------------------
	// test CAN port
	//
	if (ptsPortV != (CpPort_ts *) 0L)
	{
		if (ptsPortV->ubDrvInfo > eDRV_INFO_OFF)
		{
			tvStatusT = eCP_ERR_NONE;
#if CP_STATISTIC > 0
			ptsStatsV->ulErrMsgCount = tx1_counter;
			ptsStatsV->ulRcvMsgCount = rx1_counter;
			ptsStatsV->ulTrmMsgCount = err1_counter;
#else
			ptsStatsV->ulErrMsgCount = 0;
			ptsStatsV->ulRcvMsgCount = 0;
			ptsStatsV->ulTrmMsgCount = 0;
#endif
		}
	}

	return (tvStatusT);
}

void receive_main_tx_on_mon_rx(void)
{
	if (pfnCan2RcvHandlerS != CPP_NULL)
	{
		for (uint32_t buffer_id = 0; buffer_id < CP_BUFFER_MAX; buffer_id++)
		{
			if (atsCan2MsgS[buffer_id].ulMsgUser & CP_BUFFER_MAIN_TX_MON_RX_PND)
			{
				atsCan2MsgS[buffer_id].ulMsgUser ^= CP_BUFFER_MAIN_TX_MON_RX_PND;
				/* call MON handler */
				// typedef uint8_t (* CpRcvHandler_Fn)(CpCanMsg_ts *ptsMsgV, uint8_t ubBufferV);
				pfnCan2RcvHandlerS(&atsCan2MsgS[buffer_id], buffer_id);
			}
		}
	}
}

uint32_t modify_pending_frame_main_tx_to_mon_rx(uint16_t can_id, uint8_t dlc, uint8_t* data, uint8_t clear_pending)
{
	uint32_t ret = 1;
	if (pfnCan2RcvHandlerS != CPP_NULL)
	{
		for (uint32_t buffer_id = 0; buffer_id < CP_BUFFER_MAX; buffer_id++)
		{
			if ((atsCan2MsgS[buffer_id].ulMsgUser & CP_BUFFER_MAIN_TX_MON_RX_PND)
					&& (atsCan2MsgS[buffer_id].ulIdentifier == (uint32_t)can_id))
			{
				if (clear_pending != 0)
				{
					atsCan2MsgS[buffer_id].ulMsgUser ^= CP_BUFFER_MAIN_TX_MON_RX_PND;
				}
				else
				{
					atsCan2MsgS[buffer_id].ubMsgDLC = dlc;
					memcpy(&atsCan2MsgS[buffer_id].tuMsgData, data, dlc);
				}
				ret = 0;
			}
		}
	}
	return ret;
}

uint32_t send_to_main_rx_handler(uint16_t can_id, uint8_t dlc, uint8_t* data)
{
	uint32_t ret = 1;
	if (dlc < 9 && data != NULL)
	{
		for (uint32_t buffer_id = 0; buffer_id < CP_BUFFER_MAX; buffer_id++)
		{
			if (atsCan1MsgS[buffer_id].ulIdentifier == (uint32_t)can_id)
			{
				atsCan1MsgS[buffer_id].ubMsgDLC = dlc;
				memcpy(&atsCan1MsgS[buffer_id].tuMsgData, data, dlc);
				pfnCan1RcvHandlerS(&atsCan1MsgS[buffer_id], buffer_id);
				ret = 0;
			}
		}
	}
	return ret;
}

uint32_t send_to_mon_rx_handler(uint16_t can_id, uint8_t dlc, uint8_t* data)
{
	uint32_t ret = 1;
	if (dlc < 9 && data != NULL)
	{
		for (uint32_t buffer_id = 0; buffer_id < CP_BUFFER_MAX; buffer_id++)
		{
			if (atsCan2MsgS[buffer_id].ulIdentifier == (uint32_t)can_id)
			{
				atsCan2MsgS[buffer_id].ubMsgDLC = dlc;
				memcpy(&atsCan2MsgS[buffer_id].tuMsgData, data, dlc);
				pfnCan2RcvHandlerS(&atsCan2MsgS[buffer_id], buffer_id);
				ret = 0;
			}
		}
	}
	return ret;
}


void can_main_hw_set_log_file_name(char *file_name)
{
	(void) strcpy(can_main_hw_log_file_path, file_name);
}

int can_main_hw_log_open(void)
{
	can_main_hw_log = fopen(can_main_hw_log_file_path, "wb+");

	if (NULL == can_main_hw_log)
	{
		return EXIT_FAILURE;
	}

	return EXIT_SUCCESS;
}

void can_main_hw_log_close(void)
{
	(void) fclose(can_main_hw_log);
}

char* get_can_main_hw_log_file_path(void)
{
	return can_main_hw_log_file_path;
}

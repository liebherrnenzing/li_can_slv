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

/*!
 * \file io_can_error_mock.c
 * @addtogroup lcs_mock
 * @{
 */
/*--------------------------------------------------------------------------*/
/* include files                                                            */
/*--------------------------------------------------------------------------*/
#include "io_can_error.h"
#include "io_can_errno.h"
#include "io_can_config.h"

/*--------------------------------------------------------------------------*/
/* general definitions (private/not exported)                               */
/*--------------------------------------------------------------------------*/

/*--------------------------------------------------------------------------*/
/* structure/type definitions (private/not exported)                        */
/*--------------------------------------------------------------------------*/
/*!
 * \brief Defines a system error queue entry
 */
typedef struct
{
	li_can_slv_errorcode_t errnum; /*!< error number of queue entry*/
	err_prio_t priority; /*!< priority of error entry*/
	li_can_slv_module_nr_t module_nr; /*!< Logical module number that caused the error*/
	err_prio_t priority_redundant; /*!< redundant priority of error entry*/
#ifdef ERROR_SYSERR_ADDITIONAL_INFO_BYTE
	byte_t info; /*!< info of error entry*/
#endif // #ifdef ERROR_SYSERR_ADDITIONAL_INFO_BYTE
} error_syserr_queue_entry_mock_t;


/*--------------------------------------------------------------------------*/
/* global variables (public/exported)                                       */
/*--------------------------------------------------------------------------*/

/*--------------------------------------------------------------------------*/
/* global variables (private/not exported)                                  */
/*--------------------------------------------------------------------------*/
static error_syserr_queue_entry_mock_t error_syserr_queue_mock[ERROR_SYSERR_QUEUE_NUMENT];
static uint16_t error_syserr_queue_mock_stack_idx = 0;


/*--------------------------------------------------------------------------*/
/* public (exported) functions                                              */
/*--------------------------------------------------------------------------*/

/****************************************************************************/
/**
 * @brief Reads an entry from system error queue
 * @remarks \arg Read entry will be removed from queue
 * @arg All tasks are blocked while queue management
 * @arg If no entries in queue, #ERR_MSG_CAN_ERR_NO_ENTRY will be returned
 * @param pmodule_nr Pointer to variable that contains the logical module number that issued the error from queue
 * @param perrnum Pointer to variable that contains the system error number from queue
 * @param ppriority priority of error entry
 * @return #li_can_slv_errorcode_t or LI_CAN_SLV_ERR_OK if successful
 */
li_can_slv_errorcode_t error_syserr_get(li_can_slv_module_nr_t *pmodule_nr, msg_code_t *perrnum, err_prio_t *ppriority)
{
	uint16_t idx;

	if (error_syserr_queue_mock_stack_idx == 0)
	{
		return ERR_MSG_CAN_ERR_NO_ENTRY;
	}

	if (error_syserr_queue_mock_stack_idx > 0)
	{
		idx = error_syserr_queue_mock_stack_idx - 1;
	}

	*perrnum = error_syserr_queue_mock[idx].errnum;
	*ppriority = error_syserr_queue_mock[idx].priority;
	*pmodule_nr = error_syserr_queue_mock[idx].module_nr;

	return (LI_CAN_SLV_ERR_OK);

}

#ifdef ERROR_SYSERR_ADDITIONAL_INFO_BYTE
/****************************************************************************/
/**
 * @brief Reads an entry from system error queue
 * @remarks \arg Read entry will be removed from queue
 * @arg All tasks are blocked while queue management
 * @arg If no entries in queue, #ERR_MSG_CAN_ERR_NO_ENTRY will be returned
 * @param pmodule_nr Pointer to variable that contains the logical module number that issued the error from queue
 * @param perrnum Pointer to variable that contains the system error number from queue
 * @param ppriority priority of error entry
 * @param pinfo info of error entry
 * @return #li_can_slv_errorcode_t or LI_CAN_SLV_ERR_OK if successful
 */
li_can_slv_errorcode_t error_syserr_get_with_info_byte(li_can_slv_module_nr_t *pmodule_nr, msg_code_t *perrnum, err_prio_t *ppriority, byte_t *pinfo)
{
	uint16_t idx;

	if (error_syserr_queue_mock_stack_idx == 0)
	{
		return ERR_MSG_CAN_ERR_NO_ENTRY;
	}

	if (error_syserr_queue_mock_stack_idx > 0)
	{
		idx = error_syserr_queue_mock_stack_idx - 1;
	}

	*perrnum = error_syserr_queue_mock[idx].errnum;
	*ppriority = error_syserr_queue_mock[idx].priority;
	*pmodule_nr = error_syserr_queue_mock[idx].module_nr;
	*pinfo = error_syserr_queue_mock[idx].info;

	return (LI_CAN_SLV_ERR_OK);
}
#endif // #ifdef ERROR_SYSERR_ADDITIONAL_INFO_BYTE

/**
 * @brief send system error
 * @param errnum error code
 * @param priority error priority
 * @param module_nr module number
 * @param priority_redundant redundant error priority
 * @return LI_CAN_SLV_ERR_OK
 */
li_can_slv_errorcode_t error_syserr_send(li_can_slv_errorcode_t errnum, err_prio_t priority, li_can_slv_module_nr_t module_nr, err_prio_t priority_redundant)
{
	li_can_slv_errorcode_t err = LI_CAN_SLV_ERR_OK;

	if (error_syserr_queue_mock_stack_idx < ERROR_SYSERR_QUEUE_NUMENT)
	{
		error_syserr_queue_mock[error_syserr_queue_mock_stack_idx].errnum = errnum;
		error_syserr_queue_mock[error_syserr_queue_mock_stack_idx].priority = priority;
		error_syserr_queue_mock[error_syserr_queue_mock_stack_idx].module_nr = module_nr;
		error_syserr_queue_mock[error_syserr_queue_mock_stack_idx].priority_redundant = priority_redundant;
		error_syserr_queue_mock_stack_idx++;
	}
	else
	{
		err = ERR_SYS_ERR_QUEUE_FULL;
	}
	return (err);
}


#ifdef ERROR_SYSERR_ADDITIONAL_INFO_BYTE
/**
 * @brief send system error
 * @param errnum error code
 * @param priority error priority
 * @param module_nr module number
 * @param priority_redundant redundant error priority
 * @param info
 * @return LI_CAN_SLV_ERR_OK
 */
li_can_slv_errorcode_t error_syserr_send_with_info_byte(li_can_slv_errorcode_t errnum, err_prio_t priority, li_can_slv_module_nr_t module_nr, err_prio_t priority_redundant, byte_t info)
{
	li_can_slv_errorcode_t err;
	if (error_syserr_queue_mock_stack_idx < ERROR_SYSERR_QUEUE_NUMENT)
	{
		error_syserr_queue_mock[error_syserr_queue_mock_stack_idx].errnum = errnum;
		error_syserr_queue_mock[error_syserr_queue_mock_stack_idx].priority = priority;
		error_syserr_queue_mock[error_syserr_queue_mock_stack_idx].module_nr = module_nr;
		error_syserr_queue_mock[error_syserr_queue_mock_stack_idx].priority_redundant = priority_redundant;
		error_syserr_queue_mock[error_syserr_queue_mock_stack_idx].info = info;
		error_syserr_queue_mock_stack_idx++;
	}
	else
	{
		err = ERR_SYS_ERR_QUEUE_FULL;
	}

	return (err);
}
#endif // #ifdef ERROR_SYSERR_ADDITIONAL_INFO_BYTE


/**
 *
 * @return error_syserr_queue_mock_stack_idx
 */
uint16_t error_syserr_get_queue_mock_idx(void)
{
	return (error_syserr_queue_mock_stack_idx);
}

/**
 * @brief error_syserr_get_from_idx error
 * @param idx index to get
 * @param pmodule_nr module number
 * @param perrnum number for error message
 * @param ppriority error priority
 * @return LI_CAN_SLV_ERR_OK
 */
li_can_slv_errorcode_t error_syserr_get_from_idx(uint16_t idx, li_can_slv_module_nr_t *pmodule_nr, msg_code_t *perrnum, err_prio_t *ppriority)
{
	li_can_slv_errorcode_t err = LI_CAN_SLV_ERR_OK;

	if (idx < error_syserr_queue_mock_stack_idx)
	{
		*perrnum = error_syserr_queue_mock[idx].errnum;
		*ppriority = error_syserr_queue_mock[idx].priority;
		*pmodule_nr = error_syserr_queue_mock[idx].module_nr;
	}
	else
	{
		err = ERR_SYS_ERR_QUEUE_INDEX_TOO_BIG;
	}
	return (err);
}

#ifdef ERROR_SYSERR_ADDITIONAL_INFO_BYTE
/**
 * @brief error_syserr_get_with_info_byte_from_idx error
 * @param pmodule_nr module number
 * @param perrnum number for error message
 * @param ppriority error priority
 * @param ppriority error priority
 * @return LI_CAN_SLV_ERR_OK
 */
li_can_slv_errorcode_t error_syserr_get_with_info_byte_from_idx(uint16_t idx, li_can_slv_module_nr_t *pmodule_nr, msg_code_t *perrnum, err_prio_t *ppriority, byte_t *pinfo)
{
	li_can_slv_errorcode_t err = LI_CAN_SLV_ERR_OK;

	if (idx < error_syserr_queue_mock_stack_idx)
	{
		*perrnum = error_syserr_queue_mock[idx].errnum;
		*ppriority = error_syserr_queue_mock[idx].priority;
		*pmodule_nr = error_syserr_queue_mock[idx].module_nr;
		*pinfo = error_syserr_queue_mock[idx].info;
	}
	else
	{
		err = ERR_SYS_ERR_QUEUE_INDEX_TOO_BIG;
	}
	return (err);
}
#endif // #ifdef ERROR_SYSERR_ADDITIONAL_INFO_BYTE


/**
 *
 * @return
 */
li_can_slv_errorcode_t error_init(void)
{
	uint16_t i;

	for (i = 0; i < ERROR_SYSERR_QUEUE_NUMENT; i++)
	{
		error_syserr_queue_mock[i].errnum = LI_CAN_SLV_ERR_OK;
		error_syserr_queue_mock[i].priority = ERR_LVL_UNDEFINED;
		error_syserr_queue_mock[i].module_nr = 0;
#ifdef ERROR_SYSERR_ADDITIONAL_INFO_BYTE
		error_syserr_queue_mock[i].info = 0;
#endif // #ifdef ERROR_SYSERR_ADDITIONAL_INFO_BYTE
	}

	//reset the system error queue
	error_syserr_queue_mock_stack_idx = 0;

	return (LI_CAN_SLV_ERR_OK);
}

/**
 * @brief error_syserr_num returns the number of entries in system error queue
 * @return number of entries in system error queue
 */
uint16_t error_syserr_num(void)
{
	return (error_syserr_queue_mock_stack_idx);
}

/*--------------------------------------------------------------------------*/
/* private (not exported) functions                                         */
/*--------------------------------------------------------------------------*/
/** @} */


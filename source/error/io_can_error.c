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
 * @file io_can_error.c
 * @brief can error system including queue handling
 * @addtogroup lcs_error_core
 * @{
 */

/*--------------------------------------------------------------------------*/
/* include files                                                            */
/*--------------------------------------------------------------------------*/
#include <li_can_slv/error/io_can_error.h>
#include <li_can_slv/error/io_can_errno.h>
#include "io_can_port.h"
#include <li_can_slv/core/io_can_types.h>

#ifdef ERROR_MAIN_MON
#include "io_mainmon.h"
#include "io_mainmon_comm.h"
#endif // #ifdef ERROR_MAIN_MON

#ifdef LI_CAN_SLV_DEBUG
#include "li_can_slv_debug.h"
#endif // #ifdef LI_CAN_SLV_DEBUG

/*--------------------------------------------------------------------------*/
/* general definitions (private, not exported)                              */
/*--------------------------------------------------------------------------*/

/*--------------------------------------------------------------------------*/
/* structure/type definitions (private, not exported)                       */
/*--------------------------------------------------------------------------*/
#ifdef LI_CAN_SLV_SYS_ERROR_QUEUE
/**
 * @brief Defines a system error queue entry
 */
typedef struct
{
	msg_code_t msg_code; /**< msg_code entry */
	li_can_slv_msg_priority_t priority; /**< priority of error entry*/
	li_can_slv_module_nr_t module_nr; /**< Logical module number that caused the error*/
	byte_t add_info;
	byte_t group_subgroup;
} can_error_sys_error_queue_entry_t;

/**
 * @brief Defines the system error queue
 * @remarks @arg Implements the system error cyclic queue
 * @arg Includes also queue management data entries
 */
typedef struct
{
	can_error_sys_error_queue_entry_t error_syserr_queue[LI_CAN_SLV_ERROR_SYSERR_QUEUE_SIZE]; /*!< array space for system error queue entries*/
	uint16_t in_next_idx; /*!< index of next free queue entry*/
	uint16_t out_next_idx; /*!< index of next queue entry to be read*/
	uint16_t count; /*!< number of entries of array that have been used already*/
} can_error_sys_error_queue_t;

/**
 * @brief Defines the possible states of the error system
 * @enum error_system_state_tag
 * @typedef enum error_system_state_tag can_error_sys_error_state_t
*/
typedef enum error_system_state_tag
{
	CAN_ERR_SYS_NOT_INITIALIZED	= 0, /*!< error system has not been initialized */
	CAN_ERR_SYS_READY = 0xFFFF /*!< error system has been initialized already*/
} can_error_sys_error_state_t;
#endif // #ifdef LI_CAN_SLV_SYS_ERROR_QUEUE

/*--------------------------------------------------------------------------*/
/* global variables (public/exported)                                       */
/*--------------------------------------------------------------------------*/

/*--------------------------------------------------------------------------*/
/* function prototypes (private/not exported)                               */
/*--------------------------------------------------------------------------*/

/*--------------------------------------------------------------------------*/
/* global variables (private/not exported)                                  */
/*--------------------------------------------------------------------------*/
#ifdef LI_CAN_SLV_SYS_MODULE_ERROR
#ifdef LI_CAN_SLV_SYS_ERROR_QUEUE
static volatile can_error_sys_error_queue_t can_error_sys_error_queue;
static volatile can_error_sys_error_state_t can_error_sys_error_state = CAN_ERR_SYS_NOT_INITIALIZED;
#else // #ifdef LI_CAN_SLV_SYS_ERROR_QUEUE
static li_can_slv_module_nr_t can_error_module_nr;
static msg_code_t can_error_msg_code;
static err_prio_t can_error_priority;
static uint16_t can_error_syserr_count = 0;
static byte_t can_error_add_info;
static byte_t can_error_group_subgroup;
#endif // #ifdef LI_CAN_SLV_SYS_ERROR_QUEUE
#endif // #ifdef LI_CAN_SLV_SYS_MODULE_ERROR

/*--------------------------------------------------------------------------*/
/* function prototypes (private/not exported)                               */
/*--------------------------------------------------------------------------*/
#ifdef LI_CAN_SLV_SYS_ERROR_QUEUE
static void can_error_init(void);
#endif // #ifdef LI_CAN_SLV_SYS_ERROR_QUEUE

/*--------------------------------------------------------------------------*/
/* function definition (public/exported)                                    */
/*--------------------------------------------------------------------------*/
#ifdef LI_CAN_SLV_SYS_MODULE_ERROR
/*!
 * \brief Adds an system error to queue and triggers actions depending on priority
 * \remarks \arg For error priorities ERR_LVL_FATAL, ERR_LVL_ALERT and ERR_LVL_CRITICAL special actions are triggered by the error handler\n
 * \arg If a special action is required (e.g. CAN system is needed), this function takes care of triggering initialization of needed subsystems
 * \arg The parameters priority and priority_redundant must be equal for safety reasons; otherwise a FATAL error is issued
 * \note Use function from IO_SYSSTATE to change the system state to CRITICAL or ALERT
 * \note The FATAL state is directly set in this function
 * \param[in] errnum Error number of system error issued
 * \param[in] priority Priority of system error issued
 * \param[in] module_nr logical CAN module number that caused the error, if 0 the first module number of the whole module is taken
 * \param[in] priority_redundant Priority of system error issued (must be equal to parameter priority)
 * \return li_can_slv_errorcode_t or LI_CAN_SLV_ERR_OK if successful
 */
li_can_slv_errorcode_t error_syserr_send(li_can_slv_errorcode_t errnum, err_prio_t priority, li_can_slv_module_nr_t module_nr, err_prio_t priority_redundant)
{
	li_can_slv_errorcode_t err = LI_CAN_SLV_ERR_OK;
	msg_code_t msg_code;

	priority_redundant = priority_redundant;

	msg_code = MSG_SG_EXPAND(errnum) | MSG_GROUP_CAN;
	err = can_error_sys_error_send_full(msg_code, 0, priority, MSG_EXTRACT_GROUP(msg_code), MSG_EXTRACT_SUBGROUP(msg_code), module_nr);
	return err;
}

#if defined(ERROR_SYSERR_ADDITIONAL_INFO_BYTE) || defined(LI_CAN_SLV_SYS_MODULE_ERROR)
/*!
 * \brief Adds an system error to queue and triggers actions depending on priority
 * \remarks \arg For error priorities ERR_LVL_FATAL, ERR_LVL_ALERT and ERR_LVL_CRITICAL special actions are triggered by the error handler\n
 * \arg If a special action is required (e.g. CAN system is needed), this function takes care of triggering initialization of needed subsystems
 * \arg The parameters priority and priority_redundant must be equal for safety reasons; otherwise a FATAL error is issued
 * \note Use function from IO_SYSSTATE to change the system state to CRITICAL or ALERT
 * \note The FATAL state is directly set in this function
 * \param[in] errnum Error number of system error issued
 * \param[in] priority Priority of system error issued
 * \param[in] module_nr logical CAN module number that caused the error, if 0 the first module number of the whole module is taken
 * \param[in] priority_redundant Priority of system error issued (must be equal to parameter priority)
 * \param[in] add_info
 * \return li_can_slv_errorcode_t or LI_CAN_SLV_ERR_OK if successful
 */
li_can_slv_errorcode_t error_syserr_send_with_info_byte(li_can_slv_errorcode_t errnum, err_prio_t priority, li_can_slv_module_nr_t module_nr, err_prio_t priority_redundant, byte_t add_info)
{
	li_can_slv_errorcode_t err = LI_CAN_SLV_ERR_OK;
	msg_code_t msg_code;

	priority_redundant = priority_redundant;

	msg_code = MSG_SG_EXPAND(errnum) | MSG_GROUP_CAN;
	err = can_error_sys_error_send_full(msg_code, add_info, priority, MSG_EXTRACT_GROUP(msg_code), MSG_EXTRACT_SUBGROUP(msg_code), module_nr);
	return err;
}
#endif // #if defined(ERROR_SYSERR_ADDITIONAL_INFO_BYTE) || defined(LI_CAN_SLV_SYS_MODULE_ERROR)
#endif // #ifdef LI_CAN_SLV_SYS_MODULE_ERROR

#ifdef LI_CAN_SLV_SYS_MODULE_ERROR
li_can_slv_errorcode_t can_error_sys_error_send_full(msg_code_t msg_code, byte_t add_info, li_can_slv_msg_priority_t priority, byte_t group, byte_t subgroup, li_can_slv_module_nr_t module_nr)
{
	li_can_slv_errorcode_t err = LI_CAN_SLV_ERR_OK;
#ifdef ERROR_MAIN_MON
	mainmon_comm_error_t mmc_err;
#endif // #ifdef ERROR_MAIN_MON

#ifdef LI_CAN_SLV_SYS_ERROR_QUEUE
	int16_t free;
#endif //  #ifdef LI_CAN_SLV_SYS_ERROR_QUEUE

	byte_t group_subgroup;

#ifdef ERROR_MAIN_MON
	if (mainmon_get_type() == MAINMON_TYPE_MON)
	{
		mmc_err = mainmon_comm_syserr_send_full(msg_code, add_info, priority, group, subgroup, module_nr);

		if (MAINMON_COMM_ERROR_OK != mmc_err)
		{
			err = ERR_MSG_CAN_MSG_SEND;
		}

		goto clear_exit;
	}

#endif // #ifdef ERROR_MAIN_MON

#ifdef LI_CAN_SLV_SYS_ERROR_QUEUE
	//Check if error system has been initialized already and do so if not
	if (can_error_sys_error_state != CAN_ERR_SYS_READY)
	{
		can_error_init();
	}
#endif // #ifdef LI_CAN_SLV_SYS_ERROR_QUEUE

	/* calculate group subgroup info */
	group_subgroup = (byte_t)(subgroup + (group << 4));

	if (priority == ERR_LVL_FATAL)
	{
		/**
		 * @todo handle error level fatal
		 */
	}
#ifdef LI_CAN_SLV_SYS_ERROR_QUEUE
	else
	{
		// alert, critical, error or lower priority...send to the queue (if space) and trigger CAN-system
		// check and add error to queue if space
		// if too few space available, add the last CRITICAL error to the queue
		// (one entry remains empty, a second one has to be free for last error message)

		/**
		 *  @todo is here a irq lock needed
		 */

		// add the message to the sys error queue
		// check if the queue has 2 entries
		free = (int16_t)(LI_CAN_SLV_ERROR_SYSERR_QUEUE_SIZE - (can_error_sys_error_queue.count + 2));

		if (free < 0)
		{
			//we are full, not even able to add the "buffer full" message; set system to CRITICAL state
			priority = ERR_LVL_CRITICAL;
			err = ERR_MSG_CAN_SYS_ERR_QUEUE_FULL;
		}
		else
		{
			// adding the entry to the queue
			if (can_error_sys_error_queue.count <  LI_CAN_SLV_ERROR_SYSERR_QUEUE_SIZE)
			{
				++can_error_sys_error_queue.count;
			}
			can_error_sys_error_queue.error_syserr_queue[can_error_sys_error_queue.in_next_idx].msg_code = msg_code;
			can_error_sys_error_queue.error_syserr_queue[can_error_sys_error_queue.in_next_idx].module_nr = module_nr;
			can_error_sys_error_queue.error_syserr_queue[can_error_sys_error_queue.in_next_idx].priority = priority;
			can_error_sys_error_queue.error_syserr_queue[can_error_sys_error_queue.in_next_idx].add_info = add_info;
			can_error_sys_error_queue.error_syserr_queue[can_error_sys_error_queue.in_next_idx].group_subgroup = group_subgroup;
			can_error_sys_error_queue.in_next_idx = (can_error_sys_error_queue.in_next_idx + 1) % LI_CAN_SLV_ERROR_SYSERR_QUEUE_SIZE;

			// check if the error memory is full; if it is full add a CRITICAL message buffer full
			if (free == 0)
			{
				priority = ERR_LVL_CRITICAL;
				msg_code = MSG_SG_EXPAND(ERR_MSG_CAN_SYS_ERR_QUEUE_FULL) | MSG_GROUP_CAN;
				err = ERR_MSG_CAN_SYS_ERR_QUEUE_FULL;

				//enter the entry into queue
				if (can_error_sys_error_queue.count < LI_CAN_SLV_ERROR_SYSERR_QUEUE_SIZE)
				{
					++can_error_sys_error_queue.count;
				}

				can_error_sys_error_queue.error_syserr_queue[can_error_sys_error_queue.in_next_idx].msg_code = msg_code;
				can_error_sys_error_queue.error_syserr_queue[can_error_sys_error_queue.in_next_idx].module_nr = module_nr;
				can_error_sys_error_queue.error_syserr_queue[can_error_sys_error_queue.in_next_idx].priority = priority;
				can_error_sys_error_queue.error_syserr_queue[can_error_sys_error_queue.in_next_idx].add_info = 0;
				can_error_sys_error_queue.error_syserr_queue[can_error_sys_error_queue.in_next_idx].group_subgroup = MSG_EXTRACT_GROUP_SUBGROUP(msg_code);
				can_error_sys_error_queue.in_next_idx = (can_error_sys_error_queue.in_next_idx + 1);
			}
		}

		/**
		 * @todo is here an irq unlock needed
		 */

		/* if there is a CRITICAL and/or ALERT level, signal it to the system (and init the CAN-subsystem if not already running) */

		switch (priority)
		{
			case ERR_LVL_ALERT:
				/**
				 * @todo any special handling here
				 */
				break;
			case ERR_LVL_CRITICAL:
				/**
				 * @todo any special handling here
				 */
				break;

			case ERR_LVL_SYSTEM:
			case ERR_LVL_ERROR:
			case ERR_LVL_WARNING:
			case ERR_LVL_NOTICE:
			case ERR_LVL_INFO:
			case ERR_LVL_DEBUG:
			case ERR_LVL_DEBUG_MORE:
			case ERR_LVL_UNAWARE:
			case ERR_LVL_UNDEFINED:
				//do nothing
				break;

			default:
				//do nothing
				break;

		}

	}
	return (err);
#else // #ifdef LI_CAN_SLV_SYS_ERROR_QUEUE
	else
	{
		/* there is no queue so the last entry wins */
		can_error_syserr_count = 1;
		can_error_module_nr = module_nr;
		can_error_msg_code = msg_code;
		can_error_priority = priority;
		can_error_add_info = add_info;
		can_error_group_subgroup = group_subgroup;
	}
	return (err);
#endif // #ifdef LI_CAN_SLV_SYS_ERROR_QUEUE

#ifdef ERROR_MAIN_MON
clear_exit:
	return err;
#endif

}
#endif // #ifdef LI_CAN_SLV_SYS_MODULE_ERROR

/**
 * @param pmodule_nr pointer to module number
 * @param perrnum pointer to error number
 * @param ppriority pointer to priority
 * @return li_can_slv_errorcode_t or LI_CAN_SLV_ERR_OK if successful
 */
li_can_slv_errorcode_t can_error_sys_error_get(li_can_slv_module_nr_t *pmodule_nr, li_can_slv_errorcode_t *perrnum, err_prio_t *ppriority)
{
	li_can_slv_errorcode_t err = LI_CAN_SLV_ERR_OK;
	msg_code_t msg_code;
	byte_t group_subgroup;
	byte_t add_info;

	err = can_error_sys_error_get_full(&msg_code, &add_info, ppriority, &group_subgroup, pmodule_nr);
	*perrnum = (li_can_slv_errorcode_t)((MSG_EXTRACT_SUBGROUP(msg_code) << 12) | (MSG_EXTRACT_CODE(msg_code)));
	return err;
}

#ifdef LI_CAN_SLV_SYS_MODULE_ERROR
/**
 * @brief Reads an entry from error system
 * @remarks \arg Read entry will be removed from queue if a queue is used
 * @arg All tasks are blocked while queue management
 * @arg If no entries in queue, #ERR_MSG_CAN_ERR_NO_ENTRY will be returned
 * @param pmsg_code pointer to message code
 * @param padd_info pointer to additional info
 * @param ppriority pointer to priority
 * @param pgroup_subgroup pointer to group_subgroup
 * @param pmodule_nr pointer to module number
 * @return #li_can_slv_errorcode_t or LI_CAN_SLV_ERR_OK if successful
 */
li_can_slv_errorcode_t can_error_sys_error_get_full(msg_code_t *pmsg_code, byte_t *padd_info, err_prio_t *ppriority, byte_t *pgroup_subgroup, li_can_slv_module_nr_t *pmodule_nr)
{
#ifdef LI_CAN_SLV_SYS_ERROR_QUEUE
	li_can_slv_errorcode_t err = LI_CAN_SLV_ERR_OK;

	//check if there are any entries in queue
	if (can_error_sys_error_state != CAN_ERR_SYS_READY)
	{
		err = ERR_MSG_CAN_ERR_NOT_INITIALIZED;
	}
	else if (can_error_sys_error_queue.count == 0)
	{
		err = ERR_MSG_CAN_ERR_NO_ENTRY;
	}
	else
	{
		/**
		 *  @todo is here a irq lock needed
		 */

		*pmsg_code = (msg_code_t) can_error_sys_error_queue.error_syserr_queue[can_error_sys_error_queue.out_next_idx].msg_code;
		*pmodule_nr = (li_can_slv_module_nr_t) can_error_sys_error_queue.error_syserr_queue[can_error_sys_error_queue.out_next_idx].module_nr;
		*ppriority = (err_prio_t) can_error_sys_error_queue.error_syserr_queue[can_error_sys_error_queue.out_next_idx].priority;
		*padd_info = (byte_t) can_error_sys_error_queue.error_syserr_queue[can_error_sys_error_queue.out_next_idx].add_info;
		*pgroup_subgroup = (byte_t) can_error_sys_error_queue.error_syserr_queue[can_error_sys_error_queue.out_next_idx].group_subgroup;
		can_error_sys_error_queue.out_next_idx = (can_error_sys_error_queue.out_next_idx + 1) % LI_CAN_SLV_ERROR_SYSERR_QUEUE_SIZE;
		can_error_sys_error_queue.count--;

		/**
		 * @todo is here an irq unlock needed
		 */
	}
	return (err);
#else
	*pmodule_nr = can_error_module_nr;
	*pmsg_code = can_error_msg_code;
	*ppriority = can_error_priority;
	*padd_info = can_error_add_info;
	*pgroup_subgroup = can_error_group_subgroup;
	can_error_syserr_count = 0;
	return (LI_CAN_SLV_ERR_OK);
#endif // #ifdef LI_CAN_SLV_SYS_ERROR_QUEUE
}
#endif // #ifdef LI_CAN_SLV_SYS_MODULE_ERROR

#ifdef LI_CAN_SLV_SYS_ERROR_QUEUE
/*!	\brief Reads out a selected system error from app error queue
 * \remarks \arg Read entry will NOT be removed from queue
 * \arg No blocking of tasks is done while reading (can yield to temporary wrong results!)
 * \arg If no entries in queue, #li_can_slv_errorcode_t will be returned
 * \arg If the selected entry index shows no valid entry, #li_can_slv_errorcode_t is returned.
 * \param[in] idx Index number of entry (starting with 0 for first entry)
 * \param[out] perrnum Pointer to variable that receives the system error number from queue
 * \return #li_can_slv_errorcode_t or LI_CAN_SLV_ERR_OK if successful
 */
li_can_slv_errorcode_t can_error_sys_error_peek(uint16_t idx, msg_code_t *pmsg_code)
{
	li_can_slv_errorcode_t err = LI_CAN_SLV_ERR_OK;
	uint16_t start;

	if (can_error_sys_error_state != CAN_ERR_SYS_READY)
	{
		err = ERR_MSG_CAN_ERR_NOT_INITIALIZED;
	}
	else if (idx >= LI_CAN_SLV_ERROR_SYSERR_QUEUE_SIZE)
	{
		err = ERR_MSG_CAN_ERR_NO_ENTRY;
	}
	else if (idx >= can_error_sys_error_queue.count)
	{
		//no valid entry found
		err = ERR_MSG_CAN_ERR_NO_ENTRY;
	}
	else
	{
		//check where the "first" entry is located in queue (either idx=0 or depending on in-pointer)
		if (can_error_sys_error_queue.count < LI_CAN_SLV_ERROR_SYSERR_QUEUE_SIZE)
		{
			start = 0;
		}
		else
		{
			start = can_error_sys_error_queue.in_next_idx; //as all entries of array have been used at least once, use the oldest entry as start point
		}

		//read out the desired entry and return value
		idx = (start + idx + LI_CAN_SLV_ERROR_SYSERR_QUEUE_SIZE) % LI_CAN_SLV_ERROR_SYSERR_QUEUE_SIZE;
		*pmsg_code = (msg_code_t)(can_error_sys_error_queue.error_syserr_queue[idx].msg_code);
	}

	return (err);
}
#endif // #ifdef LI_CAN_SLV_SYS_ERROR_QUEUE

#ifdef LI_CAN_SLV_SYS_MODULE_ERROR
/**
 * @brief Returns the number of entries in system error queue
 * @return Number of entries in system error queue
 */
uint16_t can_error_sys_error_count(void)
{
#ifdef LI_CAN_SLV_SYS_ERROR_QUEUE
	uint16_t num = 0;
	if (can_error_sys_error_state != CAN_ERR_SYS_READY)
	{
		num = 0;
	}
	else
	{
		num = can_error_sys_error_queue.count;
	}
	return (num);
#else
	return (can_error_syserr_count);
#endif // #ifdef LI_CAN_SLV_SYS_ERROR_QUEUE
}
#endif // #ifdef LI_CAN_SLV_SYS_MODULE_ERROR

/*--------------------------------------------------------------------------*/
/* function definition (private/not exported)                               */
/*--------------------------------------------------------------------------*/
#ifdef LI_CAN_SLV_SYS_ERROR_QUEUE
/**
 * @brief Initializes the error system (handler and queues)
 * @return li_can_slv_errorcode_t or LI_CAN_SLV_ERR_OK if successful
 */
static void can_error_init(void)
{
	uint16_t i;

#ifdef LI_CAN_SLV_SYS_ERROR_QUEUE
	for (i = 0; i < LI_CAN_SLV_ERROR_SYSERR_QUEUE_SIZE; ++i)
	{
		can_error_sys_error_queue.error_syserr_queue[i].msg_code = LI_CAN_SLV_ERR_OK;
		can_error_sys_error_queue.error_syserr_queue[i].module_nr = 0;
		can_error_sys_error_queue.error_syserr_queue[i].priority = ERR_LVL_UNDEFINED;
		can_error_sys_error_queue.error_syserr_queue[i].add_info = 0x00;
		can_error_sys_error_queue.error_syserr_queue[i].group_subgroup = 0x00;
	}
	can_error_sys_error_queue.count = 0;
	can_error_sys_error_queue.out_next_idx = 0;
	can_error_sys_error_queue.in_next_idx = 0;
#endif // #ifdef LI_CAN_SLV_SYS_ERROR_QUEUE

	can_error_sys_error_state = CAN_ERR_SYS_READY;
}
#endif // #ifdef LI_CAN_SLV_SYS_ERROR_QUEUE

/** @} */


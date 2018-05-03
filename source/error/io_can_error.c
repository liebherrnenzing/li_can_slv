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
 * @brief Error handler system including error queue handling
 * @remarks @arg <B>Error Handling</B>\n
 * Provides an error handler for system errors\n
 * System errors are transmitted by number and priority to this error system\n
 * The error handler transfers the errors to the error queue
 * The error handler triggers depending on priority of error the following actions:
 * <UL>
 * <LI> <B>FATAL:</B> Updates the SYS_INFO data area and resets the controller (error display is done on next startup in endless loop)
 * <LI> <B>ALERT:</B> Stops the synchronous CAN process (error transmission or asynchronous protocol will still work)
 * <LI> <B>CRITICAL:</B> Sets synchronous CAN input/output data to 0 (CAN transmission continues if any)
 * <LI> <B>other:</B> No further action\n
 * </UL>
 * @arg <B>Error queues</B>\n
 * Provides separate error queues for system and application errors and their interface functions
 * for managing the queues (initializing, reading, writing, resetting)\n
 * Multiple users are allowed to enter data into the queues at the same time (all tasks are blocked while entering data)\n
 * Multiple users are allowed to get data from the queues at the same time (all tasks are blocked while reading data)\n
 * The queues will be read out by the CAN system (transmission of errors via CAN)\n
 * If a queue is full and another entry should be entered the queue management inserts an error in the queue
 * indicating that an error was lost and the function returns an error code\n
 * @pre @arg Number of queue entries of either queue is limited to 65534\n
 * @arg Before usage of the error system it has to be initialized by error_init
 * @test @arg The correct reaction of the error handler to specified error priorities has to be verified
 * @arg The correct function of queue management functions inclusive queue roll-overs has to be verified
 * @arg The correct behavior of the task lockout (concurrent entering/reading of queue entries) has to be verified
 * @arg The duration of task lockout while writing/reading to/from queues has to be measured and documented
 * @addtogroup lcs_error_core
 * @{
 */

/*--------------------------------------------------------------------------*/
/* include files                                                            */
/*--------------------------------------------------------------------------*/
#include "io_can_error.h"
#include "io_can_errno.h"
#include "io_can_port.h"
#include "io_can_types.h"

#ifdef SYSINFO
#include "io_sysinfo.h"
#endif // #ifdef SYSINFO
#ifdef SYSSTATE
#include "io_sysstate.h"
#endif // #ifdef SYSSTATE
#ifdef SYSERR_QUEUE
#include "io_irq.h"
#include "io_modhw.h"
#endif // #ifdef SYSERR_QUEUE

#if defined (MAIN_MON) && defined (MAIN_MON_COMM_UART)
#include "io_mainmon.h"
#include "io_mainmon_comm.h"
#endif // #if defined (MAIN_MON) && defined (MAIN_MON_COMM_UART)

#ifdef LI_CAN_SLV_DEBUG
#include "li_can_slv_debug.h"
#endif // #ifdef LI_CAN_SLV_DEBUG

/*--------------------------------------------------------------------------*/
/* general definitions (private, not exported)                              */
/*--------------------------------------------------------------------------*/
#define ERR_IDX_INVALID	0xFFFF /*!<Indicates an invalid index*/

/*--------------------------------------------------------------------------*/
/* structure/type definitions (private, not exported)                       */
/*--------------------------------------------------------------------------*/
#ifdef SYSERR_QUEUE
/*!
 * \brief Defines a system error queue entry
 */
typedef struct
{
	msg_code_t msg_code; /**< msg_code entry */
	li_can_slv_msg_priority_t priority; /*!< priority of error entry*/
	li_can_slv_module_nr_t module_nr; /*!< Logical module number that caused the error*/
	byte_t add_info;
	byte_t group_subgroup;
} error_syserr_queue_entry_t;

/*!
 * \brief Defines the system error queue
 * \remarks \arg Implements the system error cyclic queue
 * \arg Includes also queue management data entries
 */
typedef struct
{
	error_syserr_queue_entry_t error_syserr_queue[ERROR_SYSERR_QUEUE_NUMENT]; /*!< array space for system error queue entries*/
	uint16_t in_next_idx; /*!< index of next free queue entry*/
	uint16_t out_next_idx; /*!< index of next queue entry to be read*/
	uint16_t count; /*!< number of entries of array that have been used already*/
} error_syserr_queue_t;
#endif // #ifdef SYSERR_QUEUE


#ifdef ERROR_APPLICATION
#if defined(OUTER) || defined(OUTER_APP)
/**
 * @brief Defines a application error queue entry
 * @struct error_apperr_queue_entry_tag
 * @typedef struct error_apperr_queue_entry_tag error_apperr_queue_entry_t
 */
typedef struct error_apperr_queue_entry_tag
{
	li_can_slv_errorcode_t errnum; /*!< error number of queue entry*/
	err_apperr_state_t errstate; /*!< indicates state of error entry*/
	li_can_slv_module_nr_t module_nr; /*!< Logical module number that caused the error*/
} error_apperr_queue_entry_t;
#endif // #if defined(OUTER) || defined(OUTER_APP)
#endif // #ifdef ERROR_APPLICATION


#ifdef ERROR_APPLICATION
#if defined(OUTER) || defined(OUTER_APP)
/*!
 * \brief Defines the application error queue
 * \remarks \arg Implements the application error cyclic queue
 * \arg Includes also queue management data entries
 * @struct error_apperr_queue_tag
 * @typedef struct error_apperr_queue_tag error_apperr_queue_t
 */
typedef struct error_apperr_queue_tag
{
	error_apperr_queue_entry_t error_apperr_queue[ERROR_APPERR_QUEUE_NUMENT]; /*!< array space for application error queue entries*/
	uint16_t in_next_idx; /*!< index of next free queue entry*/
	uint16_t out_next_idx; /*!< index of next queue entry to be read*/
	uint16_t count; /*!< number of entries of array that have been used already*/
} error_apperr_queue_t;
#endif // #if defined(OUTER) || defined(OUTER_APP)
#endif // #ifdef ERROR_APPLICATION


#ifdef SYSERR_QUEUE
/*!
 * \brief Defines the possible states of the error system
 * @enum error_system_state_tag
 * @typedef enum error_system_state_tag error_system_state_t
*/
typedef enum error_system_state_tag
{
	ERR_SYS_NOT_INITIALIZED	= 0, /*!< error system has not been initialized */
	ERR_SYS_READY = 0xFFFF /*!< error system has been initialized already*/
} error_system_state_t;
#endif // #ifdef SYSERR_QUEUE

/*--------------------------------------------------------------------------*/
/* global (exported) variables                                              */
/*--------------------------------------------------------------------------*/

/*--------------------------------------------------------------------------*/
/* global (not exported) variables                                          */
/*--------------------------------------------------------------------------*/
#ifdef LI_CAN_SLV_SYS_MODULE_ERROR
#ifdef SYSERR_QUEUE
static volatile error_syserr_queue_t error_syserr_queue;
static volatile error_system_state_t error_system_state = ERR_SYS_NOT_INITIALIZED;
#else // #ifdef SYSERR_QUEUE
static li_can_slv_module_nr_t error_module_nr;
static msg_code_t error_msg_code;
static err_prio_t error_priority;
static uint16_t error_syserr_number = 0;
static byte_t error_add_info;
static byte_t error_group_subgroup;
#endif // #ifdef SYSERR_QUEUE
#endif // #ifdef LI_CAN_SLV_SYS_MODULE_ERROR

#ifdef ERROR_APPLICATION
static volatile error_apperr_queue_t error_apperr_queue;
#endif // #ifdef ERROR_APPLICATION

/*--------------------------------------------------------------------------*/
/* function definition (public/exported)                                    */
/*--------------------------------------------------------------------------*/
#ifdef SYSERR_QUEUE
/*!
 * \brief Initializes the error system (handler and queues)
 * \remarks Empties all error queues
 * \return #li_can_slv_errorcode_t or LI_CAN_SLV_ERR_OK if successful
 */
li_can_slv_errorcode_t error_init(void)
{
	li_can_slv_errorcode_t err = LI_CAN_SLV_ERR_OK;
	uint16_t i;

#ifdef SYSERR_QUEUE
	//reset the system error queue
	for (i = 0; i < ERROR_SYSERR_QUEUE_NUMENT; ++i)
	{
		error_syserr_queue.error_syserr_queue[i].msg_code = LI_CAN_SLV_ERR_OK;
		error_syserr_queue.error_syserr_queue[i].module_nr = 0;
		error_syserr_queue.error_syserr_queue[i].priority = ERR_LVL_UNDEFINED;
		error_syserr_queue.error_syserr_queue[i].add_info = 0x00;
		error_syserr_queue.error_syserr_queue[i].group_subgroup = 0x00;
	}
	error_syserr_queue.count = 0;
	error_syserr_queue.out_next_idx = 0;
	error_syserr_queue.in_next_idx = 0;
#endif // #ifdef SYSERR_QUEUE

#ifdef ERROR_APPLICATION
	//reset the application error queue
	for (i = 0; i < ERROR_SYSERR_QUEUE_NUMENT; ++i)
	{
		error_apperr_queue.error_apperr_queue[i].errnum = LI_CAN_SLV_ERR_OK;
	}
	error_apperr_queue.count = 0;
	error_apperr_queue.out_next_idx = 0;
	error_apperr_queue.in_next_idx = 0;
#endif // #ifdef ERROR_APPLICATION

	//mark error system as initialized
	error_system_state = ERR_SYS_READY;

	return (err);
}
#endif // #ifdef SYSERR_QUEUE

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
	err = error_syserr_send_full(msg_code, 0, priority, MSG_EXTRACT_GROUP(msg_code), MSG_EXTRACT_SUBGROUP(msg_code), module_nr);
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
	err = error_syserr_send_full(msg_code, add_info, priority, MSG_EXTRACT_GROUP(msg_code), MSG_EXTRACT_SUBGROUP(msg_code), module_nr);
	return err;
}
#endif // #if defined(ERROR_SYSERR_ADDITIONAL_INFO_BYTE) || defined(LI_CAN_SLV_SYS_MODULE_ERROR)
#endif // #ifdef LI_CAN_SLV_SYS_MODULE_ERROR

#ifdef LI_CAN_SLV_SYS_MODULE_ERROR
/**
 * @param msg_code message code
 * @param add_info additional info
 * @param priority error priority
 * @param group error group
 * @param subgroup error subgroup
 * @param module_nr module number
 * @return LI_CAN_SLV_ERR_OK if successful
 */
li_can_slv_errorcode_t error_syserr_send_full(msg_code_t msg_code, byte_t add_info, li_can_slv_msg_priority_t priority, byte_t group, byte_t subgroup, li_can_slv_module_nr_t module_nr)
{
	li_can_slv_errorcode_t err = LI_CAN_SLV_ERR_OK;
#ifdef SYSERR_QUEUE
	sint16_t free;
#endif //  #ifdef SYSERR_QUEUE
#ifdef SYSERR_QUEUE
#ifdef LI_CAN_SLV_BOOT
	irq_sysstate_t irq_old;
#else
#if defined (_KEIL) || defined (__TASKING__)
	uint16_t irq_old;
#endif // #if defined (_KEIL) || defined (__TASKING__)
#endif // #ifdef LI_CAN_SLV_BOOT
#endif // #ifdef SYSERR_QUEUE
#ifdef SYSINFO_ERR
	sysinfo_error_t sysinfoerr;
#endif // #ifdef SYSINFO_ERR
	byte_t group_subgroup;

#if defined (MAIN_MON) && defined (MAIN_MON_COMM_UART)
#ifdef MAINMON_COMM_SYSERR
	err = mainmon_comm_syserr_send_full(msg_code, add_info, priority, group, subgroup, module_nr);
#endif // #ifdef MAINMON_COMM_SYSERR
#endif // #if defined (MAIN_MON) && defined (MAIN_MON_COMM_UART)

#ifdef SYSERR_QUEUE
	//Check if error system has been initialized already and do so if not
	if (error_system_state != ERR_SYS_READY)
	{
		error_init();
	}
#endif // #ifdef SYSERR_QUEUE

	/* calculate group subgroup info */
	group_subgroup = (byte_t)(subgroup + (group << 4));

	if (priority == ERR_LVL_FATAL)
	{
#ifdef SYSINFO_ERR
		sysinfoerr.system_fatal = errnum;
#endif // #ifdef SYSINFO_ERR
#ifndef LI_CAN_SLV_BOOT
#ifdef SYSSTATE
		sysstate_change(SYSSTATE_SYSTEM_FATAL);
#endif // #ifdef SYSSTATE
#endif // #ifndef LI_CAN_SLV_BOOT
#ifdef SYSINFO
#ifdef SYSINFO_ERR
		sysinfo_set_error(&sysinfoerr);	//place error number into add_info struct...
#endif // #ifdef SYSINFO_ERR
		sysinfo_set_mode(SYSINFO_MODE_SYSTEM_FATAL);
#endif	// SYSINFO
#if defined (_KEIL) || defined (__TASKING__)
		modhw_reset(); //...and reset the module
#endif // #if defined (_KEIL) || defined (__TASKING__)
	}
#ifdef SYSERR_QUEUE
	else
	{
		//alert, critical, error or lower priority...send it to the queue (if space) and trigger CAN-system

		//check and add error to queue if space
		//if too few space available, add the last CRITICAL error to the queue
		//(one entry remains empty, a second one has to be free for last error message)
#ifdef LI_CAN_SLV_BOOT
		irq_old = irq_set_global_state(IRQ_SYS_DISABLE);
#else
#if defined (_KEIL) || defined (__TASKING__)
		irq_old = irq_set_top_locked_lvl(IRQ_OC_TOP_LOCK_LEVEL); //disable all relevant irqs/tasks
#endif // #if defined (_KEIL) || defined (__TASKING__)
#endif // #ifdef LI_CAN_SLV_BOOT

		// add the message to the sys error queue
		// check if the queue has 2 entries
		free = (sint16_t)(ERROR_SYSERR_QUEUE_NUMENT - (error_syserr_queue.count + 2));

		if (free < 0)
		{
			//we are full, not even able to add the "buffer full" message; set system to CRITICAL state
			priority = ERR_LVL_CRITICAL;
			err = ERR_MSG_CAN_SYS_ERR_QUEUE_FULL;
		}
		else
		{
			// adding the entry to the queue
			if (error_syserr_queue.count <  ERROR_SYSERR_QUEUE_NUMENT)
			{
				++error_syserr_queue.count;
#ifdef DEBUG_ERROR
				printf("\n-- count ++ -- \n");
#endif // #ifdef DEBUG_ERROR
			}
			error_syserr_queue.error_syserr_queue[error_syserr_queue.in_next_idx].msg_code = msg_code;
			error_syserr_queue.error_syserr_queue[error_syserr_queue.in_next_idx].module_nr = module_nr;
			error_syserr_queue.error_syserr_queue[error_syserr_queue.in_next_idx].priority = priority;
			error_syserr_queue.error_syserr_queue[error_syserr_queue.in_next_idx].add_info = add_info;
			error_syserr_queue.error_syserr_queue[error_syserr_queue.in_next_idx].group_subgroup = group_subgroup;
			error_syserr_queue.in_next_idx = (error_syserr_queue.in_next_idx + 1) % ERROR_SYSERR_QUEUE_NUMENT;

			// check if the error memory is full; if it is full add a CRITICAL message buffer full
			if (free == 0)
			{
				priority = ERR_LVL_CRITICAL;
				msg_code = MSG_SG_EXPAND(ERR_MSG_CAN_SYS_ERR_QUEUE_FULL) | MSG_GROUP_CAN;
				err = ERR_MSG_CAN_SYS_ERR_QUEUE_FULL;

				//enter the entry into queue
				if (error_syserr_queue.count < ERROR_SYSERR_QUEUE_NUMENT)
				{
					++error_syserr_queue.count;
#ifdef DEBUG_ERROR
					printf("\n-- count ++ -- \n");
#endif // #ifdef DEBUG_ERROR
				}

				error_syserr_queue.error_syserr_queue[error_syserr_queue.in_next_idx].msg_code = msg_code;
				error_syserr_queue.error_syserr_queue[error_syserr_queue.in_next_idx].module_nr = module_nr;
				error_syserr_queue.error_syserr_queue[error_syserr_queue.in_next_idx].priority = priority;
				error_syserr_queue.error_syserr_queue[error_syserr_queue.in_next_idx].add_info = 0;
				error_syserr_queue.error_syserr_queue[error_syserr_queue.in_next_idx].group_subgroup = MSG_EXTRACT_GROUP_SUBGROUP(msg_code);
				error_syserr_queue.in_next_idx = (error_syserr_queue.in_next_idx + 1);
			}
		}

#ifdef DEBUG_ERROR
		printf("\nsend err:%04X", errnum);
		printf(" in_idx:%d", error_syserr_queue.in_next_idx);
		printf(" out_idx:%d", error_syserr_queue.out_next_idx);
		printf(" cnt:%d", error_syserr_queue.count);
		printf("\n");
#endif // #ifdef DEBUG_ERROR

#ifdef LI_CAN_SLV_BOOT
		irq_set_global_state(irq_old);
#else
#if defined (_KEIL) || defined (__TASKING__)
		irq_set_top_locked_lvl(irq_old); //re-enable all relevant irqs/tasks
#endif // #if defined (_KEIL) || defined (__TASKING__)
#endif // #ifdef LI_CAN_SLV_BOOT

		//if there is a CRITICAL and/or ALERT level, signal it to the system (and init the CAN-subsystem if not already running)
		/*! \todo How to know if CAN subsystem should be started and how to do so?*/
		switch (priority)
		{
			case ERR_LVL_ALERT:
#ifdef SYSSTATE
#ifndef LI_CAN_SLV_BOOT
				sysstate_change(SYSSTATE_SYSTEM_ALERT);
#endif // #ifndef LI_CAN_SLV_BOOT
#endif // #ifdef SYSSTATE
				break;
			case ERR_LVL_CRITICAL:
#ifdef SYSSTATE
#ifndef LI_CAN_SLV_BOOT
				sysstate_change(SYSSTATE_SYSTEM_CRITICAL);
#endif // #ifndef LI_CAN_SLV_BOOT
#endif // #ifdef SYSSTATE
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
				//unknown priority...issue FATAL error
				err = ERR_LED_ERRSYS_PRIORTY_UNKNOWN;
#ifdef SYSINFO_ERR
				sysinfoerr.system_fatal = err;
#endif // #ifdef SYSINFO_ERR
#ifdef SYSSTATE
#ifndef LI_CAN_SLV_BOOT
				sysstate_change(SYSSTATE_SYSTEM_FATAL);
#endif // #ifndef LI_CAN_SLV_BOOT
#endif // #ifdef SYSSTATE

#ifdef SYSINFO
#ifdef SYSINFO_ERR
				sysinfo_set_error(&sysinfoerr);	//place error number into add_info struct...
#endif // #ifdef SYSINFO_ERR
				sysinfo_set_mode(SYSINFO_MODE_SYSTEM_FATAL);
#endif // SYSINFO
#if defined (_KEIL) || defined (__TASKING__)
				modhw_reset();	//...and reset the module
#endif // #if defined (_KEIL) || defined (__TASKING__)
				break;
		}
	}
	return (err);
#else
	else
	{
		/* there is no queue so the last entry wins */
		error_module_nr = module_nr;
		error_msg_code = msg_code;
		error_priority = priority;
		error_syserr_number = 1;
		error_add_info = add_info;
		error_group_subgroup = group_subgroup;
	}
	return (err);
#endif // #ifdef SYSERR_QUEUE
}
#endif // #ifdef LI_CAN_SLV_SYS_MODULE_ERROR

/**
 * @param pmodule_nr pointer to module number
 * @param perrnum pointer to error number
 * @param ppriority pointer to priority
 * @return li_can_slv_errorcode_t or LI_CAN_SLV_ERR_OK if successful
 */
li_can_slv_errorcode_t error_syserr_get(li_can_slv_module_nr_t *pmodule_nr, li_can_slv_errorcode_t *perrnum, err_prio_t *ppriority)
{
	li_can_slv_errorcode_t err = LI_CAN_SLV_ERR_OK;
	msg_code_t msg_code;
	byte_t group_subgroup;
	byte_t add_info;

	err = error_syserr_get_full(&msg_code, &add_info, ppriority, &group_subgroup, pmodule_nr);
	*perrnum = (li_can_slv_errorcode_t)((MSG_EXTRACT_SUBGROUP(msg_code) << 12) | (MSG_EXTRACT_CODE(msg_code)));
	return err;
}

/**
 * @param pmodule_nr pointer to module number
 * @param perrnum pointer to error number
 * @param ppriority pointer to priority
 * @param pinfo pointer to info
 * @return li_can_slv_errorcode_t or LI_CAN_SLV_ERR_OK if successful
 */
li_can_slv_errorcode_t error_syserr_get_with_info_byte(li_can_slv_module_nr_t *pmodule_nr, li_can_slv_errorcode_t *perrnum, err_prio_t *ppriority, byte_t *pinfo)
{
	li_can_slv_errorcode_t err = LI_CAN_SLV_ERR_OK;
	msg_code_t msg_code;
	byte_t group_subgroup;

	err = error_syserr_get_full(&msg_code, pinfo, ppriority, &group_subgroup, pmodule_nr);
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
li_can_slv_errorcode_t error_syserr_get_full(msg_code_t *pmsg_code, byte_t *padd_info, err_prio_t *ppriority, byte_t *pgroup_subgroup, li_can_slv_module_nr_t *pmodule_nr)
{
#ifdef SYSERR_QUEUE
	li_can_slv_errorcode_t err = LI_CAN_SLV_ERR_OK;
#ifdef LI_CAN_SLV_BOOT
	irq_sysstate_t irq_old;
#else
#if defined (_KEIL) || defined (__TASKING__)
	uint16_t irq_old;
#endif // #if defined (_KEIL) || defined (__TASKING__)
#endif // #ifdef LI_CAN_SLV_BOOT

	//check if there are any entries in queue
	if (error_system_state != ERR_SYS_READY)
	{
		err = ERR_MSG_CAN_ERR_NOT_INITIALIZED;
	}
	else if (error_syserr_queue.count == 0)
	{
		err = ERR_MSG_CAN_ERR_NO_ENTRY;
	}
	else
	{
		//read out the next entry and update queue management information
#ifdef LI_CAN_SLV_BOOT
		irq_old = irq_set_global_state(IRQ_SYS_DISABLE);
#else
#if defined (_KEIL) || defined (__TASKING__)
		irq_old = irq_set_top_locked_lvl(IRQ_OC_TOP_LOCK_LEVEL);
#endif // #if defined (_KEIL) || defined (__TASKING__)
#endif // #ifdef LI_CAN_SLV_BOOT
		*pmsg_code = (msg_code_t) error_syserr_queue.error_syserr_queue[error_syserr_queue.out_next_idx].msg_code;
		*pmodule_nr = (li_can_slv_module_nr_t) error_syserr_queue.error_syserr_queue[error_syserr_queue.out_next_idx].module_nr;
		*ppriority = (err_prio_t) error_syserr_queue.error_syserr_queue[error_syserr_queue.out_next_idx].priority;
		*padd_info = (byte_t) error_syserr_queue.error_syserr_queue[error_syserr_queue.out_next_idx].add_info;
		*pgroup_subgroup = (byte_t) error_syserr_queue.error_syserr_queue[error_syserr_queue.out_next_idx].group_subgroup;
		error_syserr_queue.out_next_idx = (error_syserr_queue.out_next_idx + 1) % ERROR_SYSERR_QUEUE_NUMENT;
		error_syserr_queue.count--; // decrement count
#ifdef DEBUG_ERROR
		printf("\nget err:%04X", *pmsg_code);
		printf(" in_idx:%d", error_syserr_queue.in_next_idx);
		printf(" out_idx:%d", error_syserr_queue.out_next_idx);
		printf(" cnt:%d", error_syserr_queue.count);
		printf("\n");
#endif // #ifdef DEBUG_ERROR

#ifdef LI_CAN_SLV_BOOT
		irq_set_global_state(irq_old);
#else // #ifdef LI_CAN_SLV_BOOT
#ifdef _KEIL
		irq_set_top_locked_lvl(irq_old);
#endif // #ifdef _KEIL
#endif // #ifdef LI_CAN_SLV_BOOT
	}
	return (err);
#else
	*pmodule_nr = error_module_nr;
	*pmsg_code = error_msg_code;
	*ppriority = error_priority;
	*padd_info = error_add_info;
	*pgroup_subgroup = error_group_subgroup;
	error_syserr_number = 0;
	return (LI_CAN_SLV_ERR_OK);
#endif // #ifdef SYSERR_QUEUE
}
#endif // #ifdef LI_CAN_SLV_SYS_MODULE_ERROR

#ifdef SYSERR_QUEUE
/****************************************************************************/
/*!	\brief Reads out a selected system error from app error queue
 * \remarks \arg Read entry will NOT be removed from queue
 * \arg No blocking of tasks is done while reading (can yield to temporary wrong results!)
 * \arg If no entries in queue, #li_can_slv_errorcode_t will be returned
 * \arg If the selected entry index shows no valid entry, #li_can_slv_errorcode_t is returned.
 * \param[in] idx Index number of entry (starting with 0 for first entry)
 * \param[out] perrnum Pointer to variable that receives the system error number from queue
 * \return #li_can_slv_errorcode_t or LI_CAN_SLV_ERR_OK if successful
 */
li_can_slv_errorcode_t error_syserr_get_diag(uint16_t idx, msg_code_t *pmsg_code)
{
	li_can_slv_errorcode_t err = LI_CAN_SLV_ERR_OK;
	uint16_t start;

	if (error_system_state != ERR_SYS_READY)
	{
		err = ERR_MSG_CAN_ERR_NOT_INITIALIZED;
	}
	else if (idx >= ERROR_SYSERR_QUEUE_NUMENT)
	{
		err = ERR_SYS_ERR_QUEUE_INDEX_TOO_BIG;
	}
	else if (idx >= error_syserr_queue.count)
	{
		//no valid entry found
		err = ERR_MSG_CAN_ERR_NO_ENTRY;
	}
	else
	{
		//check where the "first" entry is located in queue (either idx=0 or depending on in-pointer)
		if (error_syserr_queue.count < ERROR_SYSERR_QUEUE_NUMENT)
		{
			start = 0;
		}
		else
		{
			start = error_syserr_queue.in_next_idx; //as all entries of array have been used at least once, use the oldest entry as start point
		}

		//read out the desired entry and return value
		idx = (start + idx + ERROR_SYSERR_QUEUE_NUMENT) % ERROR_SYSERR_QUEUE_NUMENT;
		*pmsg_code = (msg_code_t)(error_syserr_queue.error_syserr_queue[idx].msg_code);
	}

	return (err);
}
#endif // #ifdef SYSERR_QUEUE

#ifdef LI_CAN_SLV_SYS_MODULE_ERROR
/**
 * @brief Returns the number of entries in system error queue
 * @return Number of entries in system error queue
 */
uint16_t error_syserr_num(void)
{
#ifdef SYSERR_QUEUE
	uint16_t num = 0;
	if (error_system_state != ERR_SYS_READY)
	{
		num = 0;
	}
	else
	{
		num = error_syserr_queue.count;
	}

#ifdef DEBUG_ERROR
	printf("\nerr_num --> state:%d num:%d", error_system_state, num);
#endif // #ifdef DEBUG_ERROR

	return (num);
#else
	return (error_syserr_number);
#endif // #ifdef SYSERR_QUEUE
}
#endif // #ifdef LI_CAN_SLV_SYS_MODULE_ERROR

#if defined(ERROR_APPLICATION) && defined(SYSERR_QUEUE)
/*!
 * \brief Adds an application error (incl. state) to the application error queue
 * \remarks \arg All tasks are blocked while queue management
 * \param[in] errnum Error number of application error issued
 * \param[in] errstate State of the application error issued
 * \param[in] module_nr logical CAN module number that caused the error, if 0 the first module number of the whole module is taken
 * \return #li_can_slv_errorcode_t or LI_CAN_SLV_ERR_OK if successful
 */
li_can_slv_errorcode_t error_apperr_send(li_can_slv_errorcode_t errnum, err_apperr_state_t errstate, li_can_slv_module_nr_t module_nr)
{
	li_can_slv_errorcode_t err = LI_CAN_SLV_ERR_OK;
	sint16_t free;
	uint16_t irq_old;

	//Check if error system has been initialized already and do so if not
	if (error_system_state != ERR_SYS_READY)
	{
		error_init();
	}

	//send it to the queue (if space) and trigger CAN-system

	//check and add error to queue if space
	//if too few space available, add "msg lost" error to the queue
	//(one entry remains empty, a second one has to be free for last error message)
	irq_old = irq_set_top_locked_lvl(IRQ_OC_TOP_LOCK_LEVEL);	//disable all relevant irqs/tasks
	free = (sint16_t)((error_apperr_queue.in_next_idx + 2) % ERROR_APPERR_QUEUE_NUMENT) - (sint16_t)error_apperr_queue.out_next_idx;
	if (free < 0)
	{
		//we are full, not even able to add the "buffer full" message; set system to CRITICAL state
		err = ERR_MSG_CAN_SYS_ERR_QUEUE_FULL;
	}
	else if (free == 0)
	{
		//we are full, we only are able to enter one additional CRITICAL message (buffer full)
		errnum = ERR_MSG_CAN_SYS_ERR_QUEUE_FULL;
		err = ERR_APPERR_BUF_TOO_SMALL;
		//enter the entry into queue
		if (error_apperr_queue.count <  ERROR_APPERR_QUEUE_NUMENT)
		{
			++error_apperr_queue.count;
		}
		error_apperr_queue.error_apperr_queue[error_apperr_queue.in_next_idx].errnum = errnum;
		error_apperr_queue.error_apperr_queue[error_apperr_queue.in_next_idx].module_nr = module_nr;
		error_apperr_queue.error_apperr_queue[error_apperr_queue.in_next_idx].errstate = ERR_STATE_COMING;
		error_apperr_queue.in_next_idx = (error_apperr_queue.in_next_idx + 1) % ERROR_APPERR_QUEUE_NUMENT;
	}
	else
	{
		//no problem adding the entry to the queue
		if (error_apperr_queue.count <  ERROR_APPERR_QUEUE_NUMENT)
		{
			++error_apperr_queue.count;
		}
		error_apperr_queue.error_apperr_queue[error_apperr_queue.in_next_idx].errnum = errnum;
		error_apperr_queue.error_apperr_queue[error_apperr_queue.in_next_idx].module_nr = module_nr;
		error_apperr_queue.error_apperr_queue[error_apperr_queue.in_next_idx].errstate = errstate;
		error_apperr_queue.in_next_idx = (error_apperr_queue.in_next_idx + 1) % ERROR_APPERR_QUEUE_NUMENT;
	}
	irq_set_top_locked_lvl(irq_old);	//re-enable all irqs/tasks

	return (err);
}
#endif // #if defined(ERROR_APPLICATION) && defined(SYSERR_QUEUE)

#if defined(ERROR_APPLICATION) && defined(SYSERR_QUEUE)
/****************************************************************************/
/*!
 * \brief Reads out the next application error from app error queue
 * \remarks \arg Read entry will be removed from queue
 * \arg All tasks are blocked while queue management
 * \arg If no entries in queue, #li_can_slv_errorcode_t will be returned
 * \param[out] perrnum Pointer to variable that receives the application error number from queue
 * \param[out] perrstate Pointer to variable that receives the application error status from queue
 * \return #li_can_slv_errorcode_t or LI_CAN_SLV_ERR_OK if successful
 */
li_can_slv_errorcode_t error_apperr_get(msg_code_t *perrnum, err_apperr_state_t *perrstate)
{
	li_can_slv_errorcode_t err = LI_CAN_SLV_ERR_OK;
	uint16_t irq_old;


	//check if there are any entries in queue
	if (error_system_state != ERR_SYS_READY)
	{
		err = ERR_MSG_CAN_ERR_NOT_INITIALIZED;
	}
	else if (error_apperr_queue.in_next_idx  == error_apperr_queue.out_next_idx)
	{
		err = ERR_APPERR_NO_ENTRY;
	}
	else
	{
		//read out the next entry and update queue management information
		irq_old = irq_set_top_locked_lvl(IRQ_OC_TOP_LOCK_LEVEL);
		*perrnum = (msg_code_t) error_apperr_queue.error_apperr_queue[error_apperr_queue.out_next_idx].errnum;
		*perrstate = error_apperr_queue.error_apperr_queue[error_apperr_queue.out_next_idx].errstate;
		error_apperr_queue.out_next_idx = (error_apperr_queue.out_next_idx + 1) % ERROR_APPERR_QUEUE_NUMENT;
		irq_set_top_locked_lvl(irq_old);
	}

	return (err);
}
#endif // #if defined(ERROR_APPLICATION) && defined(SYSERR_QUEUE)

#if defined(ERROR_APPLICATION) && defined(SYSERR_QUEUE)
/****************************************************************************/
/*!
 * \brief Returns the actual number of entries in application error queue
 * \return Number of entries in application error queue
 */
uint16_t error_apperr_num(void)
{
	uint16_t num = 0;

	if (error_system_state != ERR_SYS_READY)
	{
		num = 0;
	}
	else
	{
		num = (error_apperr_queue.in_next_idx + ERROR_APPERR_QUEUE_NUMENT - error_apperr_queue.out_next_idx) % ERROR_APPERR_QUEUE_NUMENT;
	}

	return (num);
}
#endif // #if defined(ERROR_APPLICATION) && defined(SYSERR_QUEUE)

#if defined(ERROR_APPLICATION) && defined(SYSERR_QUEUE)
/****************************************************************************/
/*!
 * \brief Reads out a selected application error from app error queue
 * \remarks \arg Read entry will NOT be removed from queue
 * \arg No blocking of tasks is done while reading (can yield to temporary wrong results!)
 * \arg If no entries in queue, #li_can_slv_errorcode_t will be returned
 * \arg If the selected entry index shows no valid entry, #li_can_slv_errorcode_t is returned.
 * \param[in] idx Index number of entry (starting with 0 for first entry)
 * \param[out] perrnum Pointer to variable that receives the application error number from queue
 * \param[out] perrstate Pointer to variable that receives the application error status from queue
 * \return #li_can_slv_errorcode_t or LI_CAN_SLV_ERR_OK if successful
 */
li_can_slv_errorcode_t error_apperr_get_diag(uint16_t idx, msg_code_t *perrnum, err_apperr_state_t *perrstate)
{
	li_can_slv_errorcode_t err = LI_CAN_SLV_ERR_OK;
	uint16_t start;

	if (error_system_state != ERR_SYS_READY)
	{
		err = ERR_MSG_CAN_ERR_NOT_INITIALIZED;
	}
	else if (idx >= ERROR_APPERR_QUEUE_NUMENT)
	{
		err = ERR_APPERR_INDEX_TOO_BIG;
	}
	else if (error_apperr_queue.count <= idx)
	{
		//no valid entry found
		err = ERR_APPERR_NO_ENTRY;
	}
	else
	{
		//check where the "first" entry is located in queue (either idx=0 or depending on in-pointer)
		if (error_apperr_queue.count < ERROR_APPERR_QUEUE_NUMENT)
		{
			start = 0;
		}
		else
		{
			start = error_apperr_queue.in_next_idx;	//as all entries of array have been used at least once, use the oldest entry as start point
		}

		//read out the desired entry and return value
		idx = (start + idx + ERROR_APPERR_QUEUE_NUMENT) % ERROR_APPERR_QUEUE_NUMENT;
		*perrnum = (msg_code_t)(error_apperr_queue.error_apperr_queue[idx].errnum);
		*perrstate = error_apperr_queue.error_apperr_queue[idx].errstate;
	}

	return (err);
}
#endif // #if defined(ERROR_APPLICATION) && defined(SYSERR_QUEUE)

#ifdef ERROR_SUBGROUP_TEXT
/****************************************************************************/
/*!
 * \brief Returns the text for a given message subgroup in CAN group
 * \remarks if subgroup is invalid, "undefined" is returned
 * \param[in] subgroup Subgroup number to search text for
 * \return pointer to the searched text
*/

const char_t *error_get_subgroup_text(uint8_t subgroup)
{
	if (subgroup >= sizeof(error_can_subgroup_text) / sizeof(char_t *))
	{
		subgroup = 0;	//take the "undefined" entry for invalid numbers
	}
	return (error_can_subgroup_text[subgroup]);
}
#endif // #ifdef ERROR_SUBGROUP_TEXT

/*--------------------------------------------------------------------------*/
/* function definition (private/not exported)                               */
/*--------------------------------------------------------------------------*/
/** @} */


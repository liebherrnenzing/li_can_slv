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
 * @file io_can_config.c
 * @addtogroup lcs_mock
 * @{
 */

/*--------------------------------------------------------------------------*/
/* include files                                                            */
/*--------------------------------------------------------------------------*/
#include "io_can_config_mock.h"
#include <li_can_slv/config/io_can_config.h>
#include <stdio.h>
#include <li_can_slv/error/io_can_errno.h>

#include "io_can_port.h"

/*--------------------------------------------------------------------------*/
/* general definitions (private/not exported)                               */
/*--------------------------------------------------------------------------*/

/*--------------------------------------------------------------------------*/
/* structure/type definitions (private/not exported)                        */
/*--------------------------------------------------------------------------*/
static char_t *can_config_mock_type;
//static crc16_t can_config_mock_crc;
static uint32_t can_config_mock_date;

/*--------------------------------------------------------------------------*/
/* global variables (public/exported)                                       */
/*--------------------------------------------------------------------------*/

/*--------------------------------------------------------------------------*/
/* function prototypes (private/not exported)                               */
/*--------------------------------------------------------------------------*/

/*--------------------------------------------------------------------------*/
/* global variables (private/not exported)                                  */
/*--------------------------------------------------------------------------*/
static byte_t can_config_can_data_rx[LI_CAN_SLV_MAX_NR_OF_LOGICAL_MODULES][CAN_CONFIG_NR_OF_MODULE_OBJS * LI_CAN_SLV_CONFIG_DLC_USED_8] = {{0}}; /**< array to save force data*/
static byte_t can_config_can_data_rx_shadow[LI_CAN_SLV_MAX_NR_OF_LOGICAL_MODULES][CAN_CONFIG_NR_OF_MODULE_OBJS * LI_CAN_SLV_CONFIG_DLC_USED_8] = {{0}};	/**< array to toggle force data*/
static byte_t can_config_can_data_tx[LI_CAN_SLV_MAX_NR_OF_LOGICAL_MODULES][CAN_CONFIG_NR_OF_MODULE_OBJS * LI_CAN_SLV_CONFIG_DLC_USED_8] = {{0}}; /**< array to read transmit data*/
static li_can_slv_errorcode_t can_config_set_ioconfig_mock_return_error = LI_CAN_SLV_ERR_OK;

/*--------------------------------------------------------------------------*/
/* public (exported) functions                                              */
/*--------------------------------------------------------------------------*/
/**
 * @brief mock function to force a channel on an module with a certain value
 * @param type module type to be forced
 * @param mod_nr module number to be forced
 * @param channel channel to be forced
 * @param value value to be forced
 * @return li_can_slv_errorcode_t or LI_CAN_SLV_ERR_OK if successful
 */
li_can_slv_errorcode_t can_config_mock_force_channel(char_t *type, li_can_slv_module_nr_t mod_nr, uint16_t channel, void *value)
{
	li_can_slv_errorcode_t err;
	uint16_t table_pos;
	li_can_slv_module_nr_t mod_nr_r;
	//	li_can_slv_config_module_t *can_module;

	// check the module type
	err = can_config_module_type_valid(type, &table_pos);

	// check the module number
	if (err == LI_CAN_SLV_ERR_OK)
	{
		err = can_config_get_module_nr(table_pos, &mod_nr_r);

		if (mod_nr_r != mod_nr)
		{
#ifdef IO_DEBUG
			LI_CAN_SLV_DEBUG_PRINT("committed and read module number are incompatible, read %i, committed %i\n", mod_nr_r, mod_nr);
#endif // #ifdef IO_DEBUG
		}
	}
	else
	{
		return -1;
	}

	// module type and module number is correct so select the current Module and fore the channel
	if (can_port_string_cmp(type, "OA16") == 0)
	{
		can_config_can_data_rx[table_pos][(channel - 1) * 2 + 0] = (byte_t)(0x00FF & (*((uint16_t *) value)) >> can_config_module_tab[table_pos].rx_dlc[(channel - 1) / 4]);
		can_config_can_data_rx[table_pos][(channel - 1) * 2 + 1] = (byte_t)(0x00FF & (*((uint16_t *) value)));

		can_port_memory_cpy((void *)can_config_can_data_rx_shadow, (void *)can_config_can_data_rx, sizeof(can_config_can_data_rx));

		//err = can_module->rx_main_sync[(channel-1)/4](&can_config_can_data_rx_shadow[table_pos][((channel-1)/4)*8], (byte_t*)can_config_module_tab[table_pos].rx[(channel-1)/4], can_config_module_tab[table_pos].rx_dlc[(channel-1)/4]);
		err = can_config_module_tab[table_pos].rx_main_sync[(channel - 1) / 4](&can_config_can_data_rx_shadow[table_pos][((channel - 1) / 4) * 8], (byte_t *)can_config_module_tab[table_pos].rx[(channel - 1) / 4], can_config_module_tab[table_pos].rx_dlc[(channel - 1) / 4]);
		if (err != LI_CAN_SLV_ERR_OK)
		{
			return -1;
		}
	}
	else if (can_port_string_cmp(type, "SFTY") == 0)
	{
		if ((channel > 0) && (channel <= 2))
		{
			can_config_can_data_rx[table_pos][(channel - 1) * 4] = (byte_t)(*((uint32_t *)value) >> 24 & 0xFF);
			can_config_can_data_rx[table_pos][(channel - 1) * 4 + 1] = (byte_t)(*((uint32_t *)value) >> 16 & 0xFF);
			can_config_can_data_rx[table_pos][(channel - 1) * 4 + 2] = (byte_t)(*((uint32_t *)value) >> 8 & 0xFF);
			can_config_can_data_rx[table_pos][(channel - 1) * 4 + 3] = (byte_t)(*((uint32_t *)value) & 0xFF);

			can_port_memory_cpy((void *)can_config_can_data_rx_shadow, (void *)can_config_can_data_rx, sizeof(can_config_can_data_rx));

			//err = can_module->rx_main_sync[0](&can_config_can_data_rx_shadow[table_pos][0], (byte_t*)can_config_module_tab[table_pos].rx[0], can_config_module_tab[table_pos].rx_dlc[0]);
			err = can_config_module_tab[table_pos].rx_main_sync[0](&can_config_can_data_rx_shadow[table_pos][0], (byte_t *)can_config_module_tab[table_pos].rx[0], can_config_module_tab[table_pos].rx_dlc[0]);
			if (err != LI_CAN_SLV_ERR_OK)
			{
				return -1;
			}
		}
		else
		{
#ifdef IO_DEBUG
			LI_CAN_SLV_DEBUG_PRINT("No valid channel chosen! Please choose channel 1 or channel 2!\n");
#endif // #ifdef IO_DEBUG
		}
	}
	else
	{
#ifdef IO_DEBUG
		LI_CAN_SLV_DEBUG_PRINT("module type not implemented: '%s'\n", type);
#endif // #ifdef IO_DEBUG
	}

	return (err);
}

/**
 * @brief mock function to read a channel on an module with a certain value
 * @param type module type to be read
 * @param mod_nr module number to be read
 * @param channel channel to be read starting at 1
 * @param value value to be read
 * @return li_can_slv_errorcode_t or LI_CAN_SLV_ERR_OK if successful
 */
li_can_slv_errorcode_t can_config_mock_read_channel(char_t *type, li_can_slv_module_nr_t mod_nr, uint16_t channel, void *value)
{
	li_can_slv_errorcode_t err = LI_CAN_SLV_ERR_OK;
	uint16_t table_pos;
	uint16_t obj_nr;
	uint16_t channel_nr;
	li_can_slv_module_nr_t mod_nr_r;
	byte_t value_tmp[4];
	//	li_can_slv_config_module_t *can_module;

	// check the module type
	err = can_config_module_type_valid(type, &table_pos);

	// check the module number
	if (err == LI_CAN_SLV_ERR_OK)
	{
		err = can_config_get_module_nr(table_pos, &mod_nr_r);

		if (mod_nr_r != mod_nr)
		{
#ifdef IO_DEBUG
			LI_CAN_SLV_DEBUG_PRINT("committed and read module number are incompatible, read %i, committed %i\n", mod_nr_r, mod_nr);
#endif // #ifdef IO_DEBUG
		}
	}
	else
	{
		return -1;
	}


	if (can_port_string_cmp(type, "OA16") == 0)
	{
		//err = can_module->tx_main_sync[0](&can_config_can_data_tx[table_pos][0], (byte_t*)can_config_module_tab[table_pos].tx[0], can_config_module_tab[table_pos].tx_dlc[0]);
		err = can_config_module_tab[table_pos].tx_main_sync[0](&can_config_can_data_tx[table_pos][0], (byte_t *)can_config_module_tab[table_pos].tx[0], can_config_module_tab[table_pos].tx_dlc[0]);
		if (err != LI_CAN_SLV_ERR_OK)
		{
			return -1;
		}

		*((uint16_t *)value) = (uint16_t)(0x000F & can_config_can_data_tx[table_pos][(channel - 1) / 2] >> ((can_config_module_tab[table_pos].tx_dlc[0] >> 1) * ((channel - 1) % 2)));
	}
	else if (can_port_string_cmp(type, "SFTY") == 0)
	{
		if ((channel > 0) && (channel <= 2))
		{
			//err = can_module->tx_main_sync[0](&can_config_can_data_tx[table_pos][0], (byte_t*)can_config_module_tab[table_pos].tx[0], can_config_module_tab[table_pos].tx_dlc[0]);
			err = can_config_module_tab[table_pos].tx_main_sync[0](&can_config_can_data_tx[table_pos][0], (byte_t *)can_config_module_tab[table_pos].tx[0], can_config_module_tab[table_pos].tx_dlc[0]);
			if (err != LI_CAN_SLV_ERR_OK)
			{
				return -1;
			}

			value_tmp[3] = can_config_can_data_tx[table_pos][(channel - 1) * 4];
			value_tmp[2] = can_config_can_data_tx[table_pos][(channel - 1) * 4 + 1];
			value_tmp[1] = can_config_can_data_tx[table_pos][(channel - 1) * 4 + 2];
			value_tmp[0] = can_config_can_data_tx[table_pos][(channel - 1) * 4 + 3];

			*(uint32_t *)value = *((uint32_t *)(value_tmp));
		}
		else
		{
#ifdef IO_DEBUG
			LI_CAN_SLV_DEBUG_PRINT("No valid channel chosen! Please choose channel 1 or channel 2!\n");
#endif // #ifdef IO_DEBUG
		}
	}
	else if (can_port_string_cmp(type, "IF08") == 0)
	{
		if ((channel > 0) && (channel <= 16))
		{
			if ((channel > 0) && (channel <= 8))
			{
				channel_nr = channel - 1;
				obj_nr = channel_nr / 2;
			}
			else if ((channel > 8) && (channel <= 16))

			{
				channel_nr = channel - 9;
				obj_nr = channel_nr / 2;
			}

			//err = can_module->tx_main_sync[obj_nr](&can_config_can_data_tx[table_pos][obj_nr * 8], (byte_t*) can_config_module_tab[table_pos].tx[obj_nr], can_config_module_tab[table_pos].tx_dlc[obj_nr]);
			err = can_config_module_tab[table_pos].tx_main_sync[obj_nr](&can_config_can_data_tx[table_pos][obj_nr * 8], (byte_t *) can_config_module_tab[table_pos].tx[obj_nr], can_config_module_tab[table_pos].tx_dlc[obj_nr]);
			if (err != LI_CAN_SLV_ERR_OK)
			{
				return -1;
			}

			value_tmp[3] = can_config_can_data_tx[table_pos][channel_nr * 4];
			value_tmp[2] = can_config_can_data_tx[table_pos][channel_nr * 4 + 1];
			value_tmp[1] = can_config_can_data_tx[table_pos][channel_nr * 4 + 2];
			value_tmp[0] = can_config_can_data_tx[table_pos][channel_nr * 4 + 3];

			// channel value
			if ((channel > 0) && (channel <= 8))
			{
				*(uint32_t *) value = (*((uint32_t *)(value_tmp))) >> 8;
			}
			else
				// diagnose value
			{
				*(uint32_t *) value = (*((uint32_t *)(value_tmp))) & 0x000000FF;
			}

		}
		else
		{
#ifdef IO_DEBUG
			LI_CAN_SLV_DEBUG_PRINT("No valid channel chosen! Please choose channel 1 or channel 2!\n");
#endif // #ifdef IO_DEBUG
		}
	}
	else
	{
#ifdef IO_DEBUG
		LI_CAN_SLV_DEBUG_PRINT("unknown module type '%s'\n", type);
#endif // #ifdef IO_DEBUG
	}

	return (err);
}

/**
 * @brief initializes a mock module
 * @return li_can_slv_errorcode_t or LI_CAN_SLV_ERR_OK if successful
 */
li_can_slv_errorcode_t can_config_mock_init(void)
{
	uint16_t i;
	can_config_nr_of_modules = 0;

	for (i = 0; i < LI_CAN_SLV_MAX_NR_OF_LOGICAL_MODULES; i++)
	{
		can_port_memory_set(&can_config_can_data_rx[i][0], 0, CAN_CONFIG_NR_OF_MODULE_OBJS * LI_CAN_SLV_CONFIG_DLC_USED_8);
		can_port_memory_set(&can_config_can_data_tx[i][0], 0, CAN_CONFIG_NR_OF_MODULE_OBJS * LI_CAN_SLV_CONFIG_DLC_USED_8);
	}

	can_config_set_ioconfig_mock_return_error = LI_CAN_SLV_ERR_OK;

	return (LI_CAN_SLV_ERR_OK);
}

/**
 * @brief get type of IO configuration
 * @return
 */
char_t *can_config_mock_get_type(void)
{
	return (can_config_mock_type);
}

/**
 * @brief get crc of IO configuration
 * @return
 */
//crc16_t can_config_mock_get_crc(void)
//{
//	return(can_config_mock_crc);
//}


/**
 * @brief get date of IO configuration
 * @return
 */
uint32_t can_config_mock_get_date(void)
{
	return (can_config_mock_date);
}

/**
 * @brief set an error for the function can_config_set_ioconfig for unittesting
 * @param err errorcode that wants to be set
 */
void can_config_set_ioconfig_set_error(li_can_slv_errorcode_t err)
{
	can_config_set_ioconfig_mock_return_error =  err;
}

/*--------------------------------------------------------------------------*/
/* private (not exported) functions                                         */
/*--------------------------------------------------------------------------*/
/** @} */


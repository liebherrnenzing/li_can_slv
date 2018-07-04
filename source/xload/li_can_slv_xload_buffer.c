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
 * @file li_can_slv_xload_buffer.c
 * @brief
 * @addtogroup lcs_xload_buffer
 * @{
 */

/*--------------------------------------------------------------------------*/
/* include files                                                            */
/*--------------------------------------------------------------------------*/
#include "li_can_slv_xload_buffer.h"
#include "li_can_slv_xload_defines.h"
#include "io_can_errno.h"
#ifdef LI_CAN_SLV_DEBUG
#include "li_can_slv_debug.h"
#endif // #ifdef LI_CAN_SLV_DEBUG

/*--------------------------------------------------------------------------*/
/* general definitions (private/not exported)                               */
/*--------------------------------------------------------------------------*/

/*--------------------------------------------------------------------------*/
/* structure/type definitions (private/not exported)                        */
/*--------------------------------------------------------------------------*/

/*--------------------------------------------------------------------------*/
/* global variables (public/exported)                                       */
/*--------------------------------------------------------------------------*/
#ifdef LI_CAN_SLV_DLOAD_BUFFER_INTERNAL
li_can_slv_xload_buffer_t dload_buffer;
#endif // #ifdef LI_CAN_SLV_DLOAD_BUFFER_INTERNAL

li_can_slv_xload_component_t xload_component; /**< details about the current component*/
li_can_slv_xload_component_t xload_component_version_request; /**< details about the current component for version request only */

/*--------------------------------------------------------------------------*/
/* function prototypes (private/not exported)                               */
/*--------------------------------------------------------------------------*/

/*--------------------------------------------------------------------------*/
/* global variables (private/not exported)                                  */
/*--------------------------------------------------------------------------*/

/*--------------------------------------------------------------------------*/
/* function prototypes (private/not exported)                               */
/*--------------------------------------------------------------------------*/
#ifdef LI_CAN_SLV_SLV_DEBUG_DUMP_BUFFER
static void li_can_slv_dload_dump_buffer(void);
#endif // #ifdef LI_CAN_SLV_SLV_DEBUG_DUMP_BUFFER

/*--------------------------------------------------------------------------*/
/* function definition (public/exported)                                    */
/*--------------------------------------------------------------------------*/
#ifdef LI_CAN_SLV_DLOAD_BUFFER_INTERNAL
/**
 * @brief adds the given byte to the download circular buffer
 * @remarks function checks the circular buffer borders and	service the buffer pointers after writing the data.
 * @param byte data byte added to circular buffer
 * @return	li_can_slv_errorcode_t or LI_CAN_SLV_ERR_OK if successful
 */
li_can_slv_errorcode_t dload_add_byte_to_buffer(byte_t byte)
{
	if (dload_buffer.nr_bytes >= LI_CAN_SLV_XLOAD_BUFFER_SIZE_DLOAD)
	{

#ifdef LI_CAN_SLV_SLV_DEBUG_BUFFER
		LI_CAN_SLV_DEBUG_PRINT("\nbuffer overflow: %d\n", dload_buffer.nr_bytes);
		li_can_slv_dload_dump_buffer();
#endif // #ifdef LI_CAN_SLV_SLV_DEBUG_BUFFER
		return (ERR_MSG_CAN_DLOAD_BUF_OVERFLOW);
	}

	dload_buffer.data[dload_buffer.write] = byte;
	dload_buffer.nr_bytes++;

	/* service write pointer */
	if (dload_buffer.write < (LI_CAN_SLV_XLOAD_BUFFER_SIZE_DLOAD - 1))
	{
		dload_buffer.write++;
	}
	else
	{
		dload_buffer.write = 0;
	}
	return (LI_CAN_SLV_ERR_OK);
}

/**
 * @brief get 1 byte from the download circular buffer
 * @remarks function checks the circular buffer borders and	service the buffer pointers after reading the data.
 * @param[out] data the data byte is returned
 * @return	li_can_slv_errorcode_t or LI_CAN_SLV_ERR_OK if successful
 */
li_can_slv_errorcode_t xload_buffer_get_byte(byte_t *data)
{
	if (dload_buffer.nr_bytes <= 0)
	{
		return (ERR_MSG_CAN_DLOAD_BUF_UNDERFLOW);
	}

	*data = dload_buffer.data[dload_buffer.read];
	dload_buffer.nr_bytes--;

	/* service read pointer */
	if (dload_buffer.read < (LI_CAN_SLV_XLOAD_BUFFER_SIZE_DLOAD - 1))
	{
		dload_buffer.read++;
	}
	else
	{
		dload_buffer.read = 0;
	}
	return (LI_CAN_SLV_ERR_OK);
}

/**
 * @brief get 8 bit value as string (1 character 0x30 --> '0') from download circular buffer
 * @remarks the function xload_buffer_get_byte is called to read the byte from the circular buffer
 * @param data pointer to the data
 * @return li_can_slv_errorcode_t or LI_CAN_SLV_ERR_OK if successful
 */
li_can_slv_errorcode_t dload_get_str_byte(byte_t *data)
{
	char_t str[2];
	uint16_t i;
	li_can_slv_errorcode_t rc = LI_CAN_SLV_ERR_OK;

	for (i = 0; i < 2; i++)
	{
		rc = xload_buffer_get_byte((byte_t *) &str[i]);
		if (rc != LI_CAN_SLV_ERR_OK)
		{
			return (rc);
		}
	}

	*data = (byte_t) dload_hexchar_to_uint32(&str[0], 2);
	return (LI_CAN_SLV_ERR_OK);
}

/**
 * @brief get 16 bit value as string (2 characters 0x30 --> '0') from download circular buffer
 * @remarks the function xload_buffer_get_byte is called to read the bytes from the circular buffer
 * @param data pointer to data
 * @return li_can_slv_errorcode_t or LI_CAN_SLV_ERR_OK if successful
 */
li_can_slv_errorcode_t dload_get_str_uint16(uint16_t *data)
{
	char_t str[2];
	uint16_t i;
	li_can_slv_errorcode_t rc = LI_CAN_SLV_ERR_OK;

	for (i = 0; i < 2; i++)
	{
		rc = xload_buffer_get_byte((byte_t *) &str[i]);
		if (rc != LI_CAN_SLV_ERR_OK)
		{
			return (rc);
		}
	}

	*data = (uint16_t) dload_hexchar_to_uint32(&str[0], 2);
	return (LI_CAN_SLV_ERR_OK);
}

/**
 * @brief get 32 bit value as string (4 characters 0x30 --> '0') from download circular buffer
 * @remarks the function xload_buffer_get_byte is called to read the bytes from the circular buffer
 * @param data pointer to data
 * @return	li_can_slv_errorcode_t or LI_CAN_SLV_ERR_OK if successful
 */
li_can_slv_errorcode_t dload_get_str_uint32(uint32_t *data)
{
	char_t str[4];
	uint16_t i;
	li_can_slv_errorcode_t rc = LI_CAN_SLV_ERR_OK;

	for (i = 0; i < 4; i++)
	{
		rc = xload_buffer_get_byte((byte_t *) &str[i]);
		if (rc != LI_CAN_SLV_ERR_OK)
		{
			return (rc);
		}
	}

	*data = dload_hexchar_to_uint32(&str[0], 4);
	return (LI_CAN_SLV_ERR_OK);
}

/**
 * @brief function returns the number of bytes to read of the circular buffer
 * @param nr_of_bytes number of bytes
 * @return li_can_slv_errorcode_t or LI_CAN_SLV_ERR_OK if successful
 */
li_can_slv_errorcode_t xload_buffer_get_len(uint16_t *nr_of_bytes)
{
	*nr_of_bytes = dload_buffer.nr_bytes;
	return (LI_CAN_SLV_ERR_OK);
}

/**
 * @brief store the circular buffer pointers (read and nr_bytes)
 * @return li_can_slv_errorcode_t or LI_CAN_SLV_ERR_OK if successful
 */
li_can_slv_errorcode_t dload_store_buf_state(void)
{
	dload_buffer.read_storage = dload_buffer.read;
	dload_buffer.nr_bytes_storage = dload_buffer.nr_bytes;
	return LI_CAN_SLV_ERR_OK;
}

/**
 * @brief restores the circular buffer pointers (read and nr_bytes)
 * @return li_can_slv_errorcode_t or LI_CAN_SLV_ERR_OK if successful
 */
li_can_slv_errorcode_t dload_restore_buf_state(void)
{
	// Storage
	dload_buffer.read = dload_buffer.read_storage;
	dload_buffer.nr_bytes = dload_buffer.nr_bytes_storage;
	return LI_CAN_SLV_ERR_OK;
}

#if defined(OUTER) || defined(OUTER_APP)
/**
 * @brief adds max_length bytes to the download circular buffer
 * @remarks it calls the function add_byte_to_buffer
 * @param src data bytes add to circular buffer
 * @param max_length number of bytes add to buffer
 * @return li_can_slv_errorcode_t or LI_CAN_SLV_ERR_OK if successful
 */
li_can_slv_errorcode_t dload_add_str_to_buffer(byte_t *src, uint16_t max_length)
{
	li_can_slv_errorcode_t rc = LI_CAN_SLV_ERR_OK;
	uint16_t icnt = 0;

	if (max_length < (LI_CAN_SLV_XLOAD_BUFFER_SIZE_DLOAD - dload_buffer.nr_bytes))
	{
		for (icnt = 0; ((icnt < max_length) && (rc == LI_CAN_SLV_ERR_OK)); icnt++)
		{
			rc = dload_add_byte_to_buffer(*(src + icnt));
			if (rc != LI_CAN_SLV_ERR_OK)
			{
				return rc;
			}
		}
	}
	else
	{
#if 1
		rc = ERR_MSG_CAN_DLOAD_BUF_OVERFLOW;
#else // #if 1
		/* upload buffer full and upload data block ready to send */
		rc = uload_smp_data();
		if (rc != LI_CAN_SLV_ERR_OK)
		{
			return rc;
		}

		/* wait till data block is sending */
		while ((dload_buffer.block_status & DLOAD_SUCCESS) != DLOAD_SUCCESS)
		{
			/** @todo: does not work because async RX task has the same priority as the calling background command task */
		}
		rc = dload_add_str_to_buffer(src, max_length);
#endif // #if 1
	}
	return rc;
}
#endif // #if defined(OUTER) || defined(OUTER_APP)

uint32_t dload_hexchar_to_uint32_internal(const char *hex_string, uint16_t nibbles)
{
	uint32_t value = 0;
	uint16_t i;

	for (i = 0; i < nibbles; i++)
	{
		switch (hex_string[i])
		{
			case '0':
			case '1':
			case '2':
			case '3':
			case '4':
			case '5':
			case '6':
			case '7':
			case '8':
			case '9':
				value = value << 4;
				value += hex_string[i] - 0x30;
				break;

			case 'A':
			case 'B':
			case 'C':
			case 'D':
			case 'E':
			case 'F':
				value = value << 4;
				value += hex_string[i] - 55;
				break;

			case 'a':
			case 'b':
			case 'c':
			case 'd':
			case 'e':
			case 'f':
				value = value << 4;
				value += hex_string[i] - 87;
				break;

			default:
				return (value);
				break;
		}
	}

	return (value);
}

/**
 * @brief remove last n trailing bytes from the download circular buffer
 * @param[in] num number of trailing bytes to be removed
 * @return li_can_slv_errorcode_t or LI_CAN_SLV_ERR_OK if successful
 */
li_can_slv_errorcode_t dload_remove_bytes(uint16_t num)
{
	if (dload_buffer.nr_bytes < num)
	{
		return (ERR_MSG_CAN_DLOAD_BUF_UNDERFLOW);
	}

	dload_buffer.nr_bytes -= num;

	/* adjust write pointer */
	if (dload_buffer.write >= num) // read >= LI_CAN_SLV_XLOAD_BUFFER_SIZE_DLOAD)
	{
		dload_buffer.write -= num;
	}
	else
	{
		dload_buffer.write = LI_CAN_SLV_XLOAD_BUFFER_SIZE_DLOAD - num + dload_buffer.write;
	}

	return (LI_CAN_SLV_ERR_OK);
}

/*--------------------------------------------------------------------------*/
/* function definition (private/not exported)                               */
/*--------------------------------------------------------------------------*/
#ifdef LI_CAN_SLV_SLV_DEBUG_DUMP_BUFFER
/**
 * @brief dload_show_buffer
 */
static void li_can_slv_dload_dump_buffer(void)
{
	uint16_t read = dload_buffer.read;
	uint16_t i;
	/* show buffer */
	LI_CAN_SLV_DEBUG_PRINT("\nbuffer: %u\n", dload_buffer.nr_bytes);
	for (i = 0; i < dload_buffer.nr_bytes; i++)
	{
		LI_CAN_SLV_DEBUG_PRINT("%c", dload_buffer.data[read]);
		/* service read pointer */
		if (read > LI_CAN_SLV_XLOAD_BUFFER_SIZE_DLOAD)
		{
			read = 0;
		}
		else
		{
			read++;
		}
	}
}
#endif // #ifdef LI_CAN_SLV_SLV_DEBUG_DUMP_BUFFER
#endif // #ifdef LI_CAN_SLV_DLOAD_BUFFER_INTERNAL

/** @} */


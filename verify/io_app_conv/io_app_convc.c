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
 * @file io_app_convc.c
 * @addtogroup app_convc
 * @{
 */

/*--------------------------------------------------------------------------*/
/* include files                                                            */
/*--------------------------------------------------------------------------*/
#include "io_app_convc.h"

/*--------------------------------------------------------------------------*/
/* general definitions (private/not exported)                               */
/*--------------------------------------------------------------------------*/

/*--------------------------------------------------------------------------*/
/* structure/type definitions (private/not exported)                        */
/*--------------------------------------------------------------------------*/

/*--------------------------------------------------------------------------*/
/* global variables (public/exported)                                       */
/*--------------------------------------------------------------------------*/

/*--------------------------------------------------------------------------*/
/* global variables (private/not exported)                                 */
/*--------------------------------------------------------------------------*/

/*--------------------------------------------------------------------------*/
/* function prototypes (private/not exported)                               */
/*--------------------------------------------------------------------------*/

/*--------------------------------------------------------------------------*/
/* function definition (public/exported)                                    */
/*--------------------------------------------------------------------------*/
/**
 * @brief receive conversion code main CAN-controller bit
 * @param[in] can is the pointer to the data register of the used CAN message object
 * @param[in] app is the pointer to the application data
 * @param dlc is the data length code of transmitted message (1 DLC means 1 Byte)
 * @return LCSA_ERROR_OK if successful, error otherwise (see io_errno.h for details)
 */
lcsa_errorcode_t app_convc_main_rx_bit(byte_t *can, void *app, uint16_t dlc)
{
#if defined(APP_PLATFORM_BYTE_ORDER_LITTLE_ENDIAN)
	uint16_t i;
	uint8_t *w;
	uint8_t *r;

	w = (uint8_t *) app;
	r = (uint8_t *) can;
	for (i = 0; i < dlc; i++)
	{
		*w = *r;
		w++;
		r++;
	}
#else // #if defined(APP_PLATFORM_BYTE_ORDER_LITTLE_ENDIAN)
	uint16_t i;
	uint8_t *w;
	uint8_t *r;

	w = (uint8_t *) app;
	r = (uint8_t *) can;

	// start from the end add dlc-1 because we are on the first element
	w = w + (dlc - 1);

	for (i = dlc; i > 0; i--)
	{
		*w = *r;
		w--;
		r++;
	}
#endif // #if defined(APP_PLATFORM_BYTE_ORDER_LITTLE_ENDIAN)
	return (LCSA_ERROR_OK);
}

/**
 * @brief receive conversion code monitor CAN-controller bit
 * @param[in] can is the pointer to the data register of the used CAN message object
 * @param[in] app is the pointer to the application data
 * @param dlc is the data length code of transmitted message
 * @return lcsa_errorcode_t or LCSA_ERROR_OK if successful
 */
lcsa_errorcode_t app_convc_mon_rx_bit(byte_t *can, void *app, uint16_t dlc)
{
#if defined(APP_PLATFORM_BYTE_ORDER_LITTLE_ENDIAN)
	uint16_t i;
	uint8_t *w;
	uint8_t *r;

	w = (uint8_t *) app;
	r = (uint8_t *) can;
	for (i = 0; i < dlc; i++)
	{
		*w = *r;
		w++;
		r++;
	}
#else // #if defined(APP_PLATFORM_BYTE_ORDER_LITTLE_ENDIAN)
	uint16_t i;
	uint8_t *w;
	uint8_t *r;

	w = (uint8_t *) app;
	r = (uint8_t *) can;

	// start from the end add dlc-1 because we are on the first element
	w = w + (dlc - 1);

	for (i = dlc; i > 0; i--)
	{
		*w = *r;
		w--;
		r++;
	}
#endif // #if defined(APP_PLATFORM_BYTE_ORDER_LITTLE_ENDIAN)
	return (LCSA_ERROR_OK);
}

/**
 * @brief transmit conversion code bit
 * @param[in] can is the pointer to the data register of the used CAN message object
 * @param[in] app is the pointer to the application data
 * @param dlc is the data length code of transmitted message
 * @return lcsa_errorcode_t or LCSA_ERROR_OK if successful
 */
lcsa_errorcode_t app_convc_tx_bit(byte_t *can, void *app, uint16_t dlc)
{
#if defined(APP_PLATFORM_BYTE_ORDER_LITTLE_ENDIAN)
	uint16_t i;
	uint8_t *w;
	uint8_t *r;

	w = (uint8_t *) can;
	r = (uint8_t *) app;

	for (i = 0; i < dlc; i++)
	{
		*w = *r;
		w++;
		r++;
	}
#else
	uint16_t i;
	uint8_t *w;
	uint8_t *r;

	w = (uint8_t *) can;
	r = (uint8_t *) app;

	// start from the end add dlc-1 because we are on the first element
	w = w + (dlc - 1);

	for (i = dlc; i > 0; i--)
	{
		*w = *r;
		w--;
		r++;
	}
#endif
	return (LCSA_ERROR_OK);
}

/**
 * @brief receive conversion code main CAN-controller nibble
 * @param[in] can is the pointer to the data register of the used CAN message object
 * @param[in] app is the pointer to the application data
 * @param dlc is the data length code of transmitted message
 * @return lcsa_errorcode_t or LCSA_ERROR_OK if successful
 */
lcsa_errorcode_t app_convc_main_rx_nibble(byte_t *can, void *app, uint16_t dlc)
{
#if defined(APP_PLATFORM_BYTE_ORDER_LITTLE_ENDIAN)
	uint16_t i;
	uint16_t *w;
	uint16_t *r;
	uint16_t nr;

	nr = dlc >> 1;
	w = (uint16_t *) app;
	r = (uint16_t *) can;
	for (i = 0; i < nr; i++)
	{
		*w = *r;
		w++;
		r++;
	}
#else // #if defined(APP_PLATFORM_BYTE_ORDER_LITTLE_ENDIAN)
	uint16_t i;
	word_t *w;
	word_t *r;
	uint16_t nr;

	nr = dlc >> 1;
	r = (uint16_t *) can;
	w = (uint16_t *) app;
	for (i = 0; i < nr; i++)
	{
		*w = *r;
		w++;
		r++;
	}
#endif // #if defined(APP_PLATFORM_BYTE_ORDER_LITTLE_ENDIAN)
	return (LCSA_ERROR_OK);
}

/**
 * @brief receive conversion code monitor CAN-controller nibble
 * @param[in] can is the pointer to the data register of the used CAN message object
 * @param[in] app is the pointer to the application data
 * @param dlc is the data length code of transmitted message
 * @return lcsa_errorcode_t or LCSA_ERROR_OK if successful
 */
lcsa_errorcode_t app_convc_mon_rx_nibble(byte_t *can, void *app, uint16_t dlc)
{
#if defined(APP_PLATFORM_BYTE_ORDER_LITTLE_ENDIAN)
	uint16_t i;
	uint16_t *w;
	uint16_t *r;
	uint16_t nr;

	nr = dlc >> 1;
	w = (uint16_t *) app;
	r = (uint16_t *) can;
	for (i = 0; i < nr; i++)
	{
		*w = *r;
		w++;
		r++;
	}
#else // #if defined(APP_PLATFORM_BYTE_ORDER_LITTLE_ENDIAN)
	uint16_t i;
	word_t *w;
	word_t *r;
	uint16_t nr;

	nr = dlc >> 1;
	r = (uint16_t *) can;
	w = (uint16_t *) app;
	for (i = 0; i < nr; i++)
	{
		*w = *r;
		w++;
		r++;
	}
#endif // #if defined(APP_PLATFORM_BYTE_ORDER_LITTLE_ENDIAN)
	return (LCSA_ERROR_OK);
}

/**
 * @brief transmit conversion code nibble
 * @param[in] can is the pointer to the data register of the used CAN message object
 * @param[in] app is the pointer to the application data
 * @param dlc is the data length code of transmitted message
 * @return lcsa_errorcode_t or LCSA_ERROR_OK if successful
 */
lcsa_errorcode_t app_convc_tx_nibble(byte_t *can, void *app, uint16_t dlc)
{
#if defined(APP_PLATFORM_BYTE_ORDER_LITTLE_ENDIAN)
	uint16_t i;
	uint16_t *w;
	uint16_t *r;
	uint16_t nr;

	nr = dlc >> 1;
	r = (uint16_t *) app;
	w = (uint16_t *) can;
	for (i = 0; i < nr; i++)
	{
		*w = *r;
		w++;
		r++;
	}
#else // #if defined(APP_PLATFORM_BYTE_ORDER_LITTLE_ENDIAN)
	uint16_t i;
	word_t *w;
	word_t *r;
	uint16_t nr;

	nr = dlc >> 1;
	r = (uint16_t *) app;
	w = (uint16_t *) can;
	for (i = 0; i < nr; i++)
	{
		*w = *r;
		w++;
		r++;
	}
#endif // #if defined(APP_PLATFORM_BYTE_ORDER_LITTLE_ENDIAN)
	return (LCSA_ERROR_OK);
}

/**
 * @brief receive conversion code main CAN-controller uint16
 * @param[in] can is the pointer to the data register of the used CAN message object
 * @param[in] app is the pointer to the application data
 * @param dlc is the data length code of transmitted message
 * @return lcsa_errorcode_t or LCSA_ERROR_OK if successful
 */
lcsa_errorcode_t app_convc_main_rx_uint16(byte_t *can, void *app, uint16_t dlc)
{
	uint16_t i;
	uint16_t *w;
	uint16_t *r;
	uint16_t nr;

	nr = dlc >> 1;
	w = (uint16_t *) app;
	r = (uint16_t *) can;
	for (i = 0; i < nr; i++)
	{
#if defined(APP_PLATFORM_BYTE_ORDER_LITTLE_ENDIAN)
		*w = APP_CONV_ROTL16(*r, 8);
#else // #if defined(APP_PLATFORM_BYTE_ORDER_LITTLE_ENDIAN)
		*w = *r;
#endif // #if defined(APP_PLATFORM_BYTE_ORDER_LITTLE_ENDIAN)
		w++;
		r++;
	}
	return (LCSA_ERROR_OK);
}

/**
 * @brief receive conversion code monitor CAN-controller uint16
 * @param[in] can	is the pointer to the data register of the used CAN message object
 * @param[in] app is the pointer to the application data
 * @param dlc is the data length code of transmitted message
 * @return lcsa_errorcode_t or LCSA_ERROR_OK if successful
 */
lcsa_errorcode_t app_convc_mon_rx_uint16(byte_t *can, void *app, uint16_t dlc)
{
	uint16_t i;
	uint16_t *w;
	uint16_t *r;
	uint16_t nr;

	nr = dlc >> 1;
	w = (uint16_t *) app;
	r = (uint16_t *) can;
	for (i = 0; i < nr; i++)
	{
#if defined(APP_PLATFORM_BYTE_ORDER_LITTLE_ENDIAN)
		*w = APP_CONV_ROTL16(*r, 8);
#else // #if defined(APP_PLATFORM_BYTE_ORDER_LITTLE_ENDIAN)
		*w = *r;
#endif // #if defined(APP_PLATFORM_BYTE_ORDER_LITTLE_ENDIAN)
		w++;
		r++;
	}
	return (LCSA_ERROR_OK);
}

/**
 * @brief transmit conversion code uint16
 * @param[in] can is the pointer to the data register of the used CAN message object
 * @param[in] app is the pointer to the application data
 * @param dlc is the data length code of transmitted message
 * @return lcsa_errorcode_t or LCSA_ERROR_OK if successful
 */
lcsa_errorcode_t app_convc_tx_uint16(byte_t *can, void *app, uint16_t dlc)
{
	uint16_t i;
	uint16_t *w;
	uint16_t *r;
	uint16_t nr;

	nr = dlc >> 1;
	r = (uint16_t *) app;
	w = (uint16_t *) can;
	for (i = 0; i < nr; i++)
	{
#if defined(APP_PLATFORM_BYTE_ORDER_LITTLE_ENDIAN)
		*w = APP_CONV_ROTL16(*r, 8);
#else // #if defined(APP_PLATFORM_BYTE_ORDER_LITTLE_ENDIAN)
		*w = *r;
#endif // #if defined(APP_PLATFORM_BYTE_ORDER_LITTLE_ENDIAN)
		w++;
		r++;
	}
	return (LCSA_ERROR_OK);
}

/**
 * @brief receive conversion code main CAN-controller uint32
 * @param[in] can is the pointer to the data register of the used CAN message object
 * @param[in] app is the pointer to the application data
 * @param dlc is the data length code of transmitted message
 * @return lcsa_errorcode_t or LCSA_ERROR_OK if successful
 */
lcsa_errorcode_t app_convc_main_rx_uint32(byte_t *can, void *app, uint16_t dlc)
{
	uint16_t i;
	uint32_t *w32;
	uint32_t *r32;
	uint16_t *r;
	uint16_t nr;
	uint32_t tmp;

	nr = dlc >> 2;
	r = (uint16_t *) can;
	r32 = (uint32_t *) r;

#if defined(APP_PLATFORM_BYTE_ORDER_LITTLE_ENDIAN)
	for (i = 0; i < (2 * nr); i++)
	{
		*r = APP_CONV_ROTL16(*r, 8);
		r++;
	}
#endif // #if defined(APP_PLATFORM_BYTE_ORDER_LITTLE_ENDIAN)

	do
	{
		w32 = (uint32_t *) app;
	}
	while (w32 != (uint32_t *) app);

	for (i = 0; i < nr; i++)
	{
#if defined(APP_PLATFORM_BYTE_ORDER_LITTLE_ENDIAN)
		tmp = APP_CONV_ROTL32(*r32, 16);
#else // #if defined(APP_PLATFORM_BYTE_ORDER_LITTLE_ENDIAN)
		tmp = *r32;
#endif // #if defined(APP_PLATFORM_BYTE_ORDER_LITTLE_ENDIAN)
		do
		{
			*w32 = tmp;
		}
		while (*w32 != tmp);
		r32++;
		w32++;
	}
	return (LCSA_ERROR_OK);
}

/**
 * @brief receive conversion code monitor CAN-controller uint32
 * @param[in] can is the pointer to the data register of the used CAN message object
 * @param[in] app is the pointer to the application data
 * @param dlc is the data length code of transmitted message
 * @return lcsa_errorcode_t or LCSA_ERROR_OK if successful
 */
lcsa_errorcode_t app_convc_mon_rx_uint32(byte_t *can, void *app, uint16_t dlc)
{
	uint16_t i;
	uint32_t *w32;
	uint32_t *r32;
	uint16_t *r;
	uint16_t nr;
	uint32_t tmp;

	nr = dlc >> 2;
	r = (uint16_t *) can;
	r32 = (uint32_t *) r;

#if defined(APP_PLATFORM_BYTE_ORDER_LITTLE_ENDIAN)
	for (i = 0; i < (2 * nr); i++)
	{
		*r = APP_CONV_ROTL16(*r, 8);
		r++;
	}
#endif // #if defined(APP_PLATFORM_BYTE_ORDER_LITTLE_ENDIAN)

	do
	{
		w32 = (uint32_t *) app;
	}
	while (w32 != (uint32_t *) app);

	for (i = 0; i < nr; i++)
	{
#if defined(APP_PLATFORM_BYTE_ORDER_LITTLE_ENDIAN)
		tmp = APP_CONV_ROTL32(*r32, 16);
#else // #if defined(APP_PLATFORM_BYTE_ORDER_LITTLE_ENDIAN)
		tmp = *r32;
#endif // #if defined(APP_PLATFORM_BYTE_ORDER_LITTLE_ENDIAN)
		do
		{
			*w32 = tmp;
		}
		while (*w32 != tmp);
		r32++;
		w32++;
	}
	return (LCSA_ERROR_OK);
}

/**
 * @brief transmit conversion code uint32
 * @param[in] can is the pointer to the data register of the used CAN message object
 * @param[in] app is the pointer to the application data
 * @param dlc is the data length code of transmitted message
 * @return lcsa_errorcode_t or LCSA_ERROR_OK if successful
 */
lcsa_errorcode_t app_convc_tx_uint32(byte_t *can, void *app, uint16_t dlc)
{
	uint16_t i;
	uint16_t *w;
	uint32_t *w32;
	uint16_t *r;
	uint16_t nr;

	nr = dlc >> 2;
	r = (uint16_t *) app;
	w = (uint16_t *) can;

	for (i = 0; i < (2 * nr); i++)
	{
#if defined(APP_PLATFORM_BYTE_ORDER_LITTLE_ENDIAN)
		*w = APP_CONV_ROTL16(*r, 8);
#else // #if defined(APP_PLATFORM_BYTE_ORDER_LITTLE_ENDIAN)
		*w = *r;
#endif // #if defined(APP_PLATFORM_BYTE_ORDER_LITTLE_ENDIAN)
		w++;
		r++;
	}
#if defined(APP_PLATFORM_BYTE_ORDER_LITTLE_ENDIAN)
	w32 = (uint32_t *) can;

	for (i = 0; i < nr; i++)
	{
		*w32 = APP_CONV_ROTL32(*w32, 16);
		w32++;
	}
#endif // #if defined(APP_PLATFORM_BYTE_ORDER_LITTLE_ENDIAN)

	return (LCSA_ERROR_OK);
}

#if defined(APP_PLATFORM_BYTE_ORDER_LITTLE_ENDIAN)
/**
 * @brief conversion code sint32 to sint20
 * @param[in] can is the pointer to the 8 byte array
 * @param[in] app is the pointer to the 3 sint32_t array
 * @return lcsa_errorcode_t or LCSA_ERROR_OK if successful
 */
lcsa_errorcode_t app_convc_sint32_to_sint20(byte_t *can, void *app)
{

	byte_t *w;
	dword_t *r;

	r = (dword_t *) app;
	w = (byte_t *) can;

	*w = (byte_t)((*r) >> 12); /* 19..12 bits channel 1 */
	w++;
	*w = (byte_t)((*r) >> 4); /* 11..04 bits channel 1 */
	w++;
	*w = (byte_t)((*r) << 4); /* 03..00 bits channel 1 */

	r++;
	*w |= (byte_t)(0x0F & ((*r) >> 16)); /* 19..16 bits channel 2 */
	w++;
	*w = (byte_t)((*r) >> 8); /* 15..08 bits channel 2 */
	w++;
	*w = (byte_t)((*r)); /* 07..00 bits channel 2 */

	w++;
	r++;
	*w = (byte_t)((*r) >> 12); /* 19..12 bits channel 3 */
	w++;
	*w = (byte_t)((*r) >> 4); /* 11..04 bits channel 3 */
	w++;
	*w = (byte_t)((*r) << 4); /* 03..00 bits channel 3 */

	return (LCSA_ERROR_OK);
}

/**
 * @brief conversion code uint12 for msg_object 1 of 3
 * @param[in] can is the pointer to the 8 byte array
 * @param[in] app is the pointer to the application data
 * @return lcsa_errorcode_t or LCSA_ERROR_OK if successful
 */
lcsa_errorcode_t app_convc_main_rx_uint12_msg_obj_1(byte_t *can, void *app, uint16_t dlc)
{
	word_t *w;
	byte_t *r;
	uint8_t i;

	r = (byte_t *) can;
	w = (word_t *) app;

	for (i = 0; i < dlc; ++i)
	{
		// reset the actual stored value
		*w &= 0x000F;
		// set the new value
		*w |= *r << 4;

		w++;
		r++;
	}
	return (LCSA_ERROR_OK);
}

/**
 * @brief conversion code uint12 for msg_object 2 of 3
 * @param[in] can is the pointer to the 8 byte array
 * @param[in] app is the pointer to the application data
 * @return lcsa_errorcode_t or LCSA_ERROR_OK if successful
 */
lcsa_errorcode_t app_convc_main_rx_uint12_msg_obj_2(byte_t *can, void *app, uint16_t dlc)
{
	word_t *w;
	byte_t *r;
	uint8_t i;

	r = (byte_t *) can;
	w = (uint16_t *) app;

	w += 4;

	for (i = 0; i < dlc; ++i)
	{
		// reset the actual stored value
		*w &= 0x000F;
		// set the new value
		*w |= *r << 4;

		w++;
		r++;
	}
	return (LCSA_ERROR_OK);
}

/**
 * @brief conversion code uint12 for msg_object 3 of 3
 * @param[in] can is the pointer to the 8 byte array
 * @param[in] app is the pointer to the application data
 * @return lcsa_errorcode_t or LCSA_ERROR_OK if successful
 */
lcsa_errorcode_t app_convc_main_rx_uint12_msg_obj_3(byte_t *can, void *app, uint16_t dlc)
{
	word_t *w;
	byte_t *r;
	byte_t low_nibble;
	byte_t high_nibble;
	uint8_t i;

	r = (byte_t *) can;
	w = (word_t *) app;

	w -= 8;

	for (i = 0; i < dlc; ++i)
	{

		low_nibble = (byte_t)((*r) & 0x0F);
		high_nibble = (byte_t)(((*r) & 0xF0) >> 4);

		// reset the actual stored value
		*w &= 0xFFF0;
		// set the nibble
		*w |= high_nibble;

		w++;

		// reset the actual stored value
		*w &= 0xFFF0;
		// set the nibble
		*w &= 0xFFF0;
		*w |= low_nibble;

		w++;
		r++;
	}
	return (LCSA_ERROR_OK);
}

/**
 * @brief conversion code uint12 for msg_object 1 of 3
 * @param[in] can is the pointer to the 8 byte array
 * @param[in] app is the pointer to the application data
 * @return lcsa_errorcode_t or LCSA_ERROR_OK if successful
 */
lcsa_errorcode_t app_convc_main_tx_uint12_msg_obj_1(byte_t *can, void *app, uint16_t dlc)
{
	byte_t *w;
	word_t *r;
	uint8_t i;

	w = (byte_t *) can;
	r = (word_t *) app;

	for (i = 0; i < dlc; ++i)
	{
		*w = (byte_t)((*r) >> 4);

		w++;
		r++;
	}
	return (LCSA_ERROR_OK);
}

/**
 * @brief conversion code uint12 for msg_object 2 of 3
 * @param[in] can is the pointer to the 8 byte array
 * @param[in] app is the pointer to the application data
 * @return lcsa_errorcode_t or LCSA_ERROR_OK if successful
 */
lcsa_errorcode_t app_convc_main_tx_uint12_msg_obj_2(byte_t *can, void *app, uint16_t dlc)
{
	byte_t *w;
	word_t *r;
	uint8_t i;

	w = (byte_t *) can;
	r = (word_t *) app;

	r += 4;

	for (i = 0; i < dlc; ++i)
	{
		*w = (byte_t)((*r) >> 4);

		w++;
		r++;
	}
	return (LCSA_ERROR_OK);
}

/**
 * @brief conversion code uint12 for msg_object 3 of 3
 * @param[in] can is the pointer to the 8 byte array
 * @param[in] app is the pointer to the application data
 * @return lcsa_errorcode_t or LCSA_ERROR_OK if successful
 */
lcsa_errorcode_t app_convc_main_tx_uint12_msg_obj_3(byte_t *can, void *app, uint16_t dlc)
{
	byte_t *w;
	word_t *r;
	byte_t low_nibble;
	byte_t high_nibble;
	uint8_t i;

	w = (byte_t *) can;
	r = (word_t *) app;

	r -= 8;

	for (i = 0; i < dlc; ++i)
	{

		high_nibble = (byte_t)((*r) & 0x000F);

		r++;

		low_nibble = (byte_t)((*r) & 0x000F);

		// reset the actual stored value
		*w &= 0x00;

		// set the new nibbles per channel
		*w |= high_nibble << 4;
		*w |= low_nibble;

		w++;
		r++;
	}
	return (LCSA_ERROR_OK);
}
#endif // #if defined(APP_PLATFORM_BYTE_ORDER_LITTLE_ENDIAN)

/*--------------------------------------------------------------------------*/
/* function definition (private/not exported)                               */
/*--------------------------------------------------------------------------*/

/** @} */

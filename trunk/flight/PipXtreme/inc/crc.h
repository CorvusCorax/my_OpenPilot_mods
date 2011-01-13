/**
 ******************************************************************************
 *
 * @file       crc.h
 * @author     The OpenPilot Team, http://www.openpilot.org Copyright (C) 2010.
 * @brief      Serial communication port handling routines
 * @see        The GNU Public License (GPL) Version 3
 *
 *****************************************************************************/
/*
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY
 * or FITNESS FOR A PARTICULAR PURPOSE. See the GNU General Public License
 * for more details.
 *
 * You should have received a copy of the GNU General Public License along
 * with this program; if not, write to the Free Software Foundation, Inc.,
 * 59 Temple Place, Suite 330, Boston, MA 02111-1307 USA
 */

#ifndef _CRC_H_
#define _CRC_H_

#include "stm32f10x.h"

// ********************************************************************

uint16_t UpdateCRC16(uint16_t crc, uint8_t b);
uint16_t UpdateCRC16Data(uint16_t crc, void *data, uint32_t len);

uint32_t UpdateCRC32(uint32_t crc, uint8_t b);
uint32_t UpdateCRC32Data(uint32_t crc, void *data, uint32_t len);

void CRC_init(void);

// ********************************************************************

#endif

/**
 ******************************************************************************
 * @addtogroup OpenPilotBL OpenPilot BootLoader
 * @{
 *
 * @file       stopwatch.c
 * @author     The OpenPilot Team, http://www.openpilot.org Copyright (C) 2010.
 * @brief      Timer functions for the LED PWM.
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


/////////////////////////////////////////////////////////////////////////////
// Include files
/////////////////////////////////////////////////////////////////////////////

#include "stm32f10x_tim.h"





/////////////////////////////////////////////////////////////////////////////
// Local definitions
/////////////////////////////////////////////////////////////////////////////

#define STOPWATCH_TIMER_BASE                 TIM6
#define STOPWATCH_TIMER_RCC   RCC_APB1Periph_TIM6

uint32_t STOPWATCH_Init(u32 resolution)
{
  // enable timer clock
  if( STOPWATCH_TIMER_RCC == RCC_APB2Periph_TIM1 || STOPWATCH_TIMER_RCC == RCC_APB2Periph_TIM8 )
    RCC_APB2PeriphClockCmd(STOPWATCH_TIMER_RCC, ENABLE);
  else
    RCC_APB1PeriphClockCmd(STOPWATCH_TIMER_RCC, ENABLE);

  // time base configuration
  TIM_TimeBaseInitTypeDef TIM_TimeBaseStructure;
  TIM_TimeBaseStructure.TIM_Period = 0xffff; // max period
  TIM_TimeBaseStructure.TIM_Prescaler = (72 * resolution)-1; // <resolution> uS accuracy @ 72 MHz
  TIM_TimeBaseStructure.TIM_ClockDivision = 0;
  TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;
  TIM_TimeBaseInit(STOPWATCH_TIMER_BASE, &TIM_TimeBaseStructure);

  // enable interrupt request
  TIM_ITConfig(STOPWATCH_TIMER_BASE, TIM_IT_Update, ENABLE);

  // start counter
  TIM_Cmd(STOPWATCH_TIMER_BASE, ENABLE);

  return 0; // no error
}


/////////////////////////////////////////////////////////////////////////////
//! Resets the stopwatch
//! \return < 0 on errors
/////////////////////////////////////////////////////////////////////////////
uint32_t STOPWATCH_Reset(void)
{
  // reset counter
  STOPWATCH_TIMER_BASE->CNT = 1; // set to 1 instead of 0 to avoid new IRQ request
  TIM_ClearITPendingBit(STOPWATCH_TIMER_BASE, TIM_IT_Update);

  return 0; // no error
}


/////////////////////////////////////////////////////////////////////////////
//! Returns current value of stopwatch
//! \return 1..65535: valid stopwatch value
//! \return 0xffffffff: counter overrun
/////////////////////////////////////////////////////////////////////////////
uint32_t STOPWATCH_ValueGet(void)
{
	uint32_t value = STOPWATCH_TIMER_BASE->CNT;

  if( TIM_GetITStatus(STOPWATCH_TIMER_BASE, TIM_IT_Update) != RESET )
    value = 0xffffffff;

  return value;
}


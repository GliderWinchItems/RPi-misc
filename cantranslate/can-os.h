/*******************************************************************************
* File Name          : can-os.h
* Date First Issued  : 06/27/2021
* Board              : Seeed CAN hat
* Description        : Convert our Original format (ascii-hex) to Can Socket frame
*******************************************************************************/

#ifndef __CAN_OS
#define __CAN_OS
#define CANBINSIZE 16 // Max size of binary array + 1
#include "can-so.h"

/* **************************************************************************************/
 int can_os_cnvt(struct can_frame *pframe,struct CANALL *pall, char* p);
/* @brief	: Convert binary CAN msg in can socket to legacy format
 * @param	: pframe = points to can socket frame (see can.h)
 * @param	: pall = points to various forms of CAN msg (see can-so.h)
 * @param	: p = points to string with incoming ascii-hex
 * @return	:  0 = OK; 
 *			: -1 = Input string too long (>31)
 *			: -2 = Input string too short (<15)
 *			: -3 = Illegal hex char in input string
 *			: -4 = Illegal CAN id: 29b low ord bits present with 11b IDE flag off
 			: -5 = Illegal DLC: (low four bits greater than 8) [Time Stamping ignored]
 *			: -6 = Checksum error
 * ************************************************************************************** */
#endif

/*******************************************************************************
* File Name          : can_so.h
* Date First Issued  : 06/25/2021
* Board              : Seeed CAN hat
* Description        : Convert Can Socket frame to our Original format
*******************************************************************************/

#ifndef __CAN_SO
#define __CAN_SO
#define CANBINSIZE 16 // Max size of binary array + 1

#include "common_can.h"
/* 
cba array (binary)
0 sequence byte (0-255)
1 low ord CAN id
2 ...
3 ...
4 high ord CAN id
5 dlc payload size (0-8; masked off hi-ord timestamp bits of dlc)
6-13 payload bytes // Variable length
last byte (checksum)

 0  1 sequence
 2  3 lo-ord CAN id
 4  5 ...
 6  7 ...
 8  9 hi-ord CAN id
10 11 dlc
12 13 payload[0] // Variable length
14 25 ...
26 27 payload[7]
28 29 checksum
30    '\n'
31    '\0'
*/

struct CANALL {
    struct CANRCVBUF can; // Legacy binary, i.e. "our binary format"
    char caa[CANBINSIZE*2]; // cba array converted to hex plus '\n' and '\0'
    uint8_t seq;    // First byte of line sequence number   
};

/* **************************************************************************************/
  int can_so_cnvt(struct CANALL* pall, struct canfd_frame* pframe);
/* @brief	: Convert binary CAN msg in can socket to "old" format
 * @param	: pall = points to various forms of CAN msg
 * @param	: pframe = points to can socket frame (see can.h)
 * @return	: 0 = OK; -1 = dlc > 8;
 * ************************************************************************************** */
#endif

/******************************************************************************
* File Name          : can_so.c
* Date First Issued  : 06/25/2021
* Board              : Seeed CAN hat
* Description        : Convert Can Socket frame to our Original format
*******************************************************************************/

#include "common_can.h"
#include "can-so.h"
#include "/usr/include/linux/can.h"

/* bin to ascii lookup table */
static const char h[16] = {'0', '1', '2', '3', '4', '5', '6', '7', '8', '9', 'A', 'B', 'C', 'D', 'E', 'F'};

/* **************************************************************************************
 * int can_so_cnvt(struct CANALL* pall, struct can_frame* pframe);
 * @brief	: Convert binary CAN msg in can socket to legacy format
 * @param	: pall = points to various forms of CAN msg
 * @param	: pframe = points to can socket frame (see can.h)
 * @return	: 0 = OK; -1 = dlc > 8;
 * ************************************************************************************** */
int can_so_cnvt(struct CANALL *pall, struct can_frame *pframe)
{
    uint32_t x = CHECKSUM_INITIAL;
    uint32_t *pid = &pall->can.id; // Convience pointer to CAN id in CANRCVBUF struct
    char *pa = &pall->caa[0];   // Ptr to hex output array beginning
    uint8_t *pi; 
    uint8_t b;
    int err = 0;

    /* Sequence number */
    pall.seq += 1;                 // Increment (binary) sequence number
    x += pall.seq;                 // Add to checksum
    *pa++ = h[((pall->seq >> 4) & 0x0f)]; // Hi order nibble
    *pa++ = h[(pall->seq & 0x0f)];      // Lo order nibble

    /* frame CAN id unwinding. Left justify. */
    if ((pframe->can_id & 0x80000000U) == 0)
    {
        *pid = pframe->can_id << 21; // 11b left justify
    }
    else
    { // Here, left justify 29b. Add IDE bit
        *pid = (pframe->can_id << 3) | (0x4);
    }
    // Reposition and add RTR bit
    *pid |= (pframe->can_id & 0x40000000U) >> 29;

    // Load STM32 format CAN id into binary output array,
    //  continue building checksum
    //  continue converting ascii-hex
    b = (*pid >> 0);
        x += b;  *pa++ = h[((b >> 4) & 0x0f)]; *pa++ = h[(b & 0x0f)];
    b = (*pid >> 8); 
        x += b;  *pa++ = h[((b >> 4) & 0x0f)]; *pa++ = h[(b & 0x0f)];
    b = (*pid >> 16);
        x += b;  *pa++ = h[((b >> 4) & 0x0f)]; *pa++ = h[(b & 0x0f)];
    b = (*pid >> 24);
        x += b;  *pa++ = h[((b >> 4) & 0x0f)]; *pa++ = h[(b & 0x0f)];

    /* Set DLC */
    b = (pframe->len & 0xf);
    if (b > 8)
    {
        err = -1;
        b = 8;
    }
    pall->can.dlc = b; // Set dlc in Our struct
    x += b;  *pa++ = h[((b >> 4) & 0x0f)]; *pa++ = h[(b & 0x0f)];

    /* Copy payload from socket frame */
    // Alignment OK for copying old CANRCVBUF payload as a unsigned longlong
    pall->can.cd.ul = (uint64_t)pframe->data[0]; // Our struct

    // Copy to variable length payload in frame to binary array
    //      continue building checksum
    //      continue converting ascii-hex
    pi = &pframe->data[0];
    switch (b)
    {
    case 8:
        b= (*pi++ >> 0);
        x += b;
        *pa++ = h[((b>> 4) & 0x0f)];
        *pa++ = h[(b & 0x0f)];
    case 7:
        b= (*pi++ >> 0);
        x += b;
        *pa++ = h[((b>> 4) & 0x0f)];
        *pa++ = h[(b & 0x0f)];
    case 6:
        b= (*pi++ >> 0);
        x += b;
        *pa++ = h[((b>> 4) & 0x0f)];
        *pa++ = h[(b & 0x0f)];
    case 5:
        b= (*pi++ >> 0);
        x += b;
        *pa++ = h[((b>> 4) & 0x0f)];
        *pa++ = h[(b & 0x0f)];
    case 4:
        b= (*pi++ >> 0);
        x += b;
        *pa++ = h[((b>> 4) & 0x0f)];
        *pa++ = h[(b & 0x0f)];
    case 3:
        b= (*pi++ >> 0);
        x += b;
        *pa++ = h[((b>> 4) & 0x0f)];
        *pa++ = h[(b & 0x0f)];
    case 2:
        b= (*pi++ >> 0);
        x += b;
        *pa++ = h[((b>> 4) & 0x0f)];
        *pa++ = h[(b & 0x0f)];
    case 1:
        b= (*pi++ >> 0);
        x += b;
        *pa++ = h[((b>> 4) & 0x0f)];
        *pa++ = h[(b & 0x0f)];
    case 0:
        break;
    }

    // Complete checksum and add to binary output array
    x += (x >> 16); // Add carries into high half word
    x += (x >> 16); // Add carry if previous add generated a carry
    x += (x >> 8);  // Add high byte of low half word
    x += (x >> 8);  // Add carry if previous add generated a carry
    b = (uint8_t)x;
    *pa++ = h[((b >> 4) & 0x0f)]; *pa++ = h[(b & 0x0f)];

    /* Complete ascii-hex string */
    *pa++ = '\n'; // Line terminator
    *pa = '\0';   // String terminator
    return err;
}
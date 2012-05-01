/*
 * uartregistermasks.h
 *
 *  Created on: Dec 17, 2011
 *      Author: daniel
 */

#ifndef UARTREGISTERMASKS_H_
#define UARTREGISTERMASKS_H_

#define INTERRUPTENABLE_ERBFI 0x01
#define INTERRUPTENALBE_ETBEI 0x02

#define INTERRUPTIDENTIFY_PENDING 0x01
#define INTERRUPTIDENTIFY_ID 0x06
#define INTERRUPTIDENTIFY_FIFOMODE 0x08

#define INTERRUPTIDENTIFY_LEVEL_FORTH 0
#define INTERRUPTIDENTIFY_LEVEL_THIRD (0x01 << 1)
#define INTERRUPTIDENTIFY_LEVEL_SECOND (0x02 << 1)
#define INTERRUPTIDENTIFY_LEVEL_HIGHEST (0x03 << 1)

#define FIFOCONTROL_ENABLE 0x01
#define FIFOCONTROL_RCVRFIFORESET 0x02
#define FIFOCONTROL_XMITFIFORESET 0x04
#define FIFOCONTROL_DMAMODESELECT 0x08
#define FIFOCONTROL_RCVRTRIGGER 0xC0 // two bits
#define LINECONTROL_DLAB 0x80
#define LINECONTROL_SETBREAK 0x40
#define LINECONTROL_STICKPARITY 0x20
#define LINECONTROL_EVENPARITYSELECT 0x10
#define LINECONTROL_PARITYENABLE 0x08
#define LINECONTROL_NUMBEROFSTOPBITS 0x04
#define LINECONTROL_WORDLENGTHSELECT 0x03 // two bits
#define LINESTATUS_DATAREADY 0x01
#define LINESTATUS_OVERRUNERROR 0x02
#define LINESTATUS_PARITYERROR 0x04
#define LINESTATUS_FRAMINGERROR 0x08
#define LINESTATUS_BREAKINTERRUPT 0x20
#define LINESTATUS_TRANSMITTERHOLDINGREGISTEREMPTY 0x20
#define LINESTATUS_TRANSMITTEREMPTY 0x40
#define LINESTATUS_ERRORINRCVRFIFO 0x80

#endif /* UARTREGISTERMASKS_H_ */

/*
 * event.cpp
 *
 *  Created on: May 21, 2015
 *      Author: OS1
 */


#include "event.h"
#include "pcb.h"
#include "scparams.h"
#include <dos.h>
#include "kernel.h"
#include "kernelev.h"
#include "system.h"
extern int lockFlag;
extern int SysMode;

Event::Event(IVTNo ivtNo) {
	myKernelEv = PCB::lastID++;
	SCParams* callParams = new SCParams();
	unsigned id = myKernelEv;
	callParams->firstInt = ivtNo;
	callParams->firstPtr = PCB::running;
	callParams->secondPtr = this;
	#ifndef BCC_BLOCK_IGNORE
	unsigned addrSeg = FP_SEG(callParams);
	unsigned addrOff = FP_OFF(callParams);
	asm {
		push ax
		push bx
		push cx
		push dx
		mov ax, 12
		mov bx, id
		mov cx, addrSeg
		mov dx, addrOff
		int 61h
		pop dx
		pop cx
		pop bx
		pop ax
	}
	#endif
	delete callParams;
}

Event::~Event() {
	SCParams* callParams = new SCParams();
	unsigned id = myKernelEv;
	#ifndef BCC_BLOCK_IGNORE
	unsigned addrSeg = FP_SEG(callParams);
	unsigned addrOff = FP_OFF(callParams);
	asm {
		push ax
		push bx
		push cx
		push dx
		mov ax, 13
		mov bx, id
		mov cx, addrSeg
		mov dx, addrOff
		int 61h
		pop dx
		pop cx
		pop bx
		pop ax
	}
	#endif
	delete callParams;
}

void Event::wait() {
	SCParams* callParams = new SCParams();
	unsigned id = myKernelEv;
	callParams->firstPtr = PCB::running;
	#ifndef BCC_BLOCK_IGNORE
	unsigned addrSeg = FP_SEG(callParams);
	unsigned addrOff = FP_OFF(callParams);
	asm {
		push ax
		push bx
		push cx
		push dx
		mov ax, 14
		mov bx, id
		mov cx, addrSeg
		mov dx, addrOff
		int 61h
		pop dx
		pop cx
		pop bx
		pop ax
	}
	#endif
	delete callParams;
}

void Event::signal() {
	SysMode = 1;
	lockFlag = 1;
	((KernelEv*)Kernel::kernelList->find(myKernelEv))->signal();
	SysMode = 0;
	lockFlag = 0;
}

/*
 * semaphor.cpp
 *
 *  Created on: May 19, 2015
 *      Author: OS1
 */



#include "semaphor.h"
#include "pcb.h"
#include "scparams.h"
#include <dos.h>

Semaphore::Semaphore(int val) {
	myKernelSem = PCB::lastID++;
	unsigned id = myKernelSem;
	SCParams* callParams = new SCParams();
	callParams->firstInt = val;
#ifndef BCC_BLOCK_IGNORE
	unsigned addrSeg = FP_SEG(callParams);
	unsigned addrOff = FP_OFF(callParams);
	asm {
		push ax
		push bx
		push cx
		push dx
		mov ax, 7
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

Semaphore::~Semaphore() {
	SCParams* callParams = new SCParams();
	unsigned id = myKernelSem;
#ifndef BCC_BLOCK_IGNORE
	unsigned addrSeg = FP_SEG(callParams);
	unsigned addrOff = FP_OFF(callParams);
	asm {
		push ax
		push bx
		push cx
		push dx
		mov ax, 8
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


int Semaphore::val() const {
	SCParams* callParams = new SCParams();
	unsigned id = myKernelSem;
#ifndef BCC_BLOCK_IGNORE
	unsigned addrSeg = FP_SEG(callParams);
	unsigned addrOff = FP_OFF(callParams);
	asm {
		push ax
		push bx
		push cx
		push dx
		mov ax, 9
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
	int ret = callParams->firstInt;
	delete callParams;
	return ret;
}

void Semaphore::wait() {
	SCParams* callParams = new SCParams();
	unsigned id = myKernelSem;
#ifndef BCC_BLOCK_IGNORE
	unsigned addrSeg = FP_SEG(callParams);
	unsigned addrOff = FP_OFF(callParams);
	asm {
		push ax
		push bx
		push cx
		push dx
		mov ax, 10
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


void Semaphore::signal() {
	SCParams* callParams = new SCParams();
	unsigned id = myKernelSem;
#ifndef BCC_BLOCK_IGNORE
	unsigned addrSeg = FP_SEG(callParams);
	unsigned addrOff = FP_OFF(callParams);
	asm {
		push ax
		push bx
		push cx
		push dx
		mov ax, 11
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


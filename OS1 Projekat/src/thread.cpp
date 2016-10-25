/*
 * thread.cpp
 *
 *  Created on: May 19, 2015
 *      Author: OS1
 */

#include "thread.h"
#include "scparams.h"
#include "pcb.h"
#include <stdlib.h>
#include <dos.h>
#include <iostream.h>
#include "schedule.h"
#include "kernel.h"
#include "system.h"
extern int lockFlag;

Thread::Thread(StackSize stackSize, Time timeSlice) {
	SCParams* callParams = new SCParams();
	callParams->firstInt = stackSize;
	callParams->secondInt = timeSlice;
	unsigned id = PCB::lastID++;
	myPCB = id;
	#ifndef BCC_BLOCK_IGNORE
	unsigned addrSeg = FP_SEG(callParams);
	unsigned addrOff = FP_OFF(callParams);
	asm {
		push ax
		push bx
		push cx
		push dx
		mov ax, 1
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


void Thread::start() {
	SCParams* callParams = new SCParams();
	callParams->firstPtr = wrapper;
	callParams->secondPtr = this;
	unsigned tmpID = myPCB;

#ifndef BCC_BLOCK_IGNORE
	unsigned addrSeg = FP_SEG(callParams);
	unsigned addrOff = FP_OFF(callParams);
	asm {
		push ax
		push bx
		push cx
		push dx
		mov ax, 2
		mov bx, tmpID
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

Thread::~Thread() {
	waitToComplete();
}

void Thread::waitToComplete() {
	lockFlag = 1;
	SCParams* callParams = new SCParams();
	unsigned tmpID = myPCB;
	callParams->firstPtr = PCB::running;
#ifndef BCC_BLOCK_IGNORE
	unsigned addrSeg = FP_SEG(callParams);
	unsigned addrOff = FP_OFF(callParams);
	asm {
		push ax
		push bx
		push cx
		push dx
		mov ax, 4
		mov bx, tmpID
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


void Thread::wrapper(Thread* thread) {

	thread->run();
	lockFlag = 1;
	SCParams* callParams = new SCParams();
	unsigned tmpID = thread->myPCB;
#ifndef BCC_BLOCK_IGNORE
	unsigned addrSeg = FP_SEG(callParams);
	unsigned addrOff = FP_OFF(callParams);
	asm {
		push ax
		push bx
		push cx
		push dx
		mov ax, 5
		mov bx, tmpID
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

void dispatch() {
	SCParams* callParams = new SCParams();
#ifndef BCC_BLOCK_IGNORE
	unsigned addrSeg = FP_SEG(callParams);
	unsigned addrOff = FP_OFF(callParams);
	asm {
		push ax
		push bx
		push cx
		push dx
		mov ax, 3
		mov bx, 0
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


void Thread::sleep(Time timeToSleep) {
	SCParams* callParams = new SCParams();
	callParams->firstInt = timeToSleep;
	#ifndef BCC_BLOCK_IGNORE
	unsigned addrSeg = FP_SEG(callParams);
	unsigned addrOff = FP_OFF(callParams);
	asm {
		push ax
		push bx
		push cx
		push dx
		mov ax, 6
		mov bx, -1
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

void tick();

void interrupt Kernel::newTimer(...) {

	PCB::timePassed++;
	tick();
	Kernel::sleepList->decreaseTime();

#ifndef BCC_BLOCK_IGNORE
	asm int 60h;
#endif

	if (!Kernel::sleepList->timeLeft()) {
		Kernel::sleepList->wakeUp();
		PCB::timeSince = 0;
	}

	if (SysMode && (PCB::timePassed == PCB::running->myTimeSlice) && (PCB::running->myTimeSlice != 0)) {
		PCB::contextSwitchRequest = 1;
	}

	if (!SysMode && !lockFlag) {
		if ((PCB::timePassed >= PCB::running->myTimeSlice) && (PCB::running->myTimeSlice != 0)) {
			dispatch();
		}
	}
}




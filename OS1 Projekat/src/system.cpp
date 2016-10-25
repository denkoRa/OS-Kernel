/*
 * system.cpp
 *
 *  Created on: May 19, 2015
 *      Author: OS1
 */




#include <dos.h>
#include "scparams.h"
#include "thread.h"
#include "kernel.h"
#include "pcb.h"
#include "schedule.h"
#include "system.h"
#include <iostream.h>
#include <stdlib.h>
#include "kernelev.h"
#include "kernsem.h"
#include "ivtentry.h"
extern int lockFlag;

void doTheCall(int idCall, int objID, SCParams* callParams){


	switch (idCall) {

	case 1:	//Thread::Thread
		PCB* pcb = new PCB((StackSize)callParams->firstInt, (Time)callParams->secondInt);
		Kernel::kernelList->add(pcb, objID);
		Kernel::doScheduling = 0;
		Kernel::putIntoScheduler = 1;
		PCB::running->blocked = 0;
		break;

	case 2:	//Thread::start
		((PCB*)Kernel::kernelList->find(objID))->start((void (*)(Thread*))callParams->firstPtr, (Thread*)callParams->secondPtr);
		Kernel::putIntoScheduler = 1;
		Kernel::doScheduling = 0;
		PCB::running->blocked = 0;
		break;

	case 3:	//dispatch call
		PCB::running->blocked = 0;
		Kernel::doScheduling = 1;
		Kernel::putIntoScheduler = 1;
		break;

	case 4:	//Thread::waitToComplete
		lockFlag = 0;
		PCB* tempPCB = (PCB*)Kernel::kernelList->find(objID);
		if (!tempPCB->finished) {
			tempPCB->put((PCB*)callParams->firstPtr);
			((PCB*)callParams->firstPtr)->blocked = 1;
			Kernel::putIntoScheduler = 0;
			Kernel::doScheduling = 1;
		} else {
			tempPCB->blocked = 0;
			Kernel::doScheduling = 0;
			Kernel::putIntoScheduler = 0;
		}
		break;

	case 5:	//Thread::wrapper after run() method
		lockFlag = 0;
		tempPCB = (PCB*)Kernel::kernelList->find(objID);
		tempPCB->finished = 1;
		tempPCB->signal();
		delete tempPCB;
		Kernel::kernelList->remove(objID);
		Kernel::putIntoScheduler = 0;
		Kernel::doScheduling = 1;
		break;

	case 6:	//Thread::sleep
		(Kernel::sleepList)->putInDreams(PCB::running, (Time)callParams->firstInt);
		Kernel::putIntoScheduler = 0;
		Kernel::doScheduling = 1;
		PCB::running->blocked = 0;
		break;

	case 7:	//Semaphore::Semaphore
		KernelSem* kernelSem = new KernelSem((int)callParams->firstInt);
		Kernel::kernelList->add(kernelSem, objID);
		Kernel::doScheduling = 0;
		PCB::running->blocked = 0;
		break;

	case 8:	//Semaphore::~Semaphore
		kernelSem = (KernelSem*)Kernel::kernelList->find(objID);
		delete kernelSem;
		Kernel::kernelList->remove(objID);
		Kernel::doScheduling = 0;
		Kernel::putIntoScheduler = 0;
		PCB::running->blocked = 0;
		break;

	case 9:	//Semaphore::val
		callParams->firstInt = ((KernelSem*)Kernel::kernelList->find(objID))->val();
		Kernel::doScheduling = 0;
		PCB::running->blocked = 0;
		break;

	case 10: //Semaphore::wait
		((KernelSem*)Kernel::kernelList->find(objID))->wait();
		break;

	case 11: //Semaphore::signal
		((KernelSem*)Kernel::kernelList->find(objID))->signal();
		Kernel::doScheduling = 0;
		Kernel::putIntoScheduler = 0;
		PCB::running->blocked = 0;
		break;

	case 12: //Event::Event
		KernelEv* kernelEv = new KernelEv(callParams->firstInt, (PCB*)callParams->firstPtr);
		Kernel::kernelList->add(kernelEv, objID);
		IVTEntry::entries[callParams->firstInt]->setEvent((Event*)callParams->secondPtr);
		Kernel::doScheduling = 0;
		Kernel::putIntoScheduler = 0;
		PCB::running->blocked = 0;
		break;

	case 13: //Event::~Event
		kernelEv = (KernelEv*)Kernel::kernelList->find(objID);
		Kernel::kernelList->remove(objID);
		delete kernelEv;
		Kernel::doScheduling = 0;
		PCB::running->blocked = 0;
		Kernel::putIntoScheduler = 0;
		break;

	case 14: //Event::wait
		((KernelEv*)Kernel::kernelList->find(objID))->wait((PCB*)callParams->firstPtr);
		break;

	}

	returnFromSysCall = 1;
	#ifndef BCC_BLOCK_IGNORE
	asm int 61h;
	#endif
}

unsigned tsp, tss, tbp;
unsigned addrSeg, addrOff, systsp, systss, systbp;
unsigned idCall, objID;


void interrupt Kernel::SysCall(...) {

	#ifndef BCC_BLOCK_IGNORE
		asm {
			mov idCall,  ax
			mov objID,   bx
			mov addrSeg, cx
			mov addrOff, dx
			mov tsp, sp
			mov tss, ss
			mov tbp, bp
		}
	#endif

	SysMode = 1;
	if (!returnFromSysCall) {
		Kernel::kernelStack[KERNELSIZE - 1] = addrSeg;
		Kernel::kernelStack[KERNELSIZE - 2] = addrOff;
		Kernel::kernelStack[KERNELSIZE - 3] = objID;
		Kernel::kernelStack[KERNELSIZE - 4] = idCall;
		Kernel::kernelStack[KERNELSIZE - 7] = 0x200;
		#ifndef BCC_BLOCK_IGNORE
		Kernel::kernelStack[KERNELSIZE - 8] = FP_SEG(doTheCall);
		Kernel::kernelStack[KERNELSIZE - 9] = FP_OFF(doTheCall);

		PCB::running->sp = tsp;
		PCB::running->ss = tss;
		PCB::running->bp = tbp;


		systsp = FP_OFF(Kernel::kernelStack + KERNELSIZE - 18);
		systss = FP_SEG(Kernel::kernelStack + KERNELSIZE - 18);
		systbp = FP_OFF(Kernel::kernelStack + KERNELSIZE - 18);

		asm {
			mov sp, systsp
			mov ss, systss
			mov bp, systbp
		}
		#endif
	}

	if (returnFromSysCall) {
		returnFromSysCall = 0;

		if (Kernel::doScheduling || PCB::contextSwitchRequest) {
			if (Kernel::putIntoScheduler || PCB::contextSwitchRequest)
				if (PCB::running != Kernel::Idle && !PCB::running->blocked && !PCB::running->finished) {
					Scheduler::put(PCB::running);
				}
			PCB::timePassed = 0;
			PCB* temp = Scheduler::get();
			if ((PCB::running = temp) == NULL) {
				PCB::running = Kernel::Idle;
			}
		}

		tsp = PCB::running->sp;
		tss = PCB::running->ss;
		tbp = PCB::running->bp;

		#ifndef BCC_BLOCK_IGNORE
		asm {
			mov sp, tsp
			mov ss, tss
			mov bp, tbp
		}
		#endif
		SysMode = 0;
	}
}




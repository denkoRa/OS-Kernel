/*
 * kernelev.cpp
 *
 *  Created on: May 21, 2015
 *      Author: OS1
 */

#include "kernelev.h"
#include "schedule.h"
#include "kernel.h"
#include "pcb.h"
#include "ivtentry.h"
KernelEv::KernelEv(IVTNo ivtno, PCB* m) {
	master = m;
	ivtNo = ivtno;
	value = 0;
}


KernelEv::~KernelEv() {
	if (master->blocked) {
		master->blocked = 0;
		Scheduler::put(master);
	}
	IVTEntry::entries[ivtNo] = 0;
}


void KernelEv::wait(PCB* blockMe) {
	if (blockMe != master) {
		Kernel::doScheduling = 0;
		Kernel::putIntoScheduler = 0;
		blockMe->blocked = 0;
		return;
	}
	else {
		master->blocked = 1;
		Kernel::doScheduling = 1;
		Kernel::putIntoScheduler = 0;
	}
}

void KernelEv::signal() {
	if (master != 0 && master->blocked) {
		master->blocked = 0;
		Scheduler::put(master);
	}
}

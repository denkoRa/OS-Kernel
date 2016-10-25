/*
 * kernsem.cpp
 *
 *  Created on: May 19, 2015
 *      Author: OS1
 */


#include "kernsem.h"
#include "schedule.h"
#include "kernel.h"
#include <stdlib.h>
#include <iostream.h>
extern int lockFlag;

KernelSem::KernelSem(int init) {
	value = init;
	first = last = 0;
}

void KernelSem::wait() {
	if (--value < 0) {
		Kernel::doScheduling = 1;
		Kernel::putIntoScheduler = 0;
		PCB::running->blocked = 1;
		if (first) last = last->next = new Elem(PCB::running, 0);
		else {
			first = last = new Elem(PCB::running, 0);
		}
	} else {
		PCB::running->blocked = 0;
		Kernel::doScheduling = 0;
		Kernel::putIntoScheduler = 0;
	}
}

void KernelSem::signal() {
	if (value++ < 0) {
		if (first) {
			first->pcb->blocked = 0;
			Scheduler::put(first->pcb);
			Elem* old = first;
			if (first->next) first = first->next;
			else first = last = 0;
			delete old;
		}
		Kernel::doScheduling = 0;
	}
}


KernelSem::~KernelSem() {
	Elem* cur = first;
	while (cur != 0) {
		cur->pcb->blocked = 0;
		Scheduler::put(cur->pcb);
		Elem* old = cur;
		cur = cur->next;
		delete old;
	}
}

int KernelSem::val() {
	return value;
}




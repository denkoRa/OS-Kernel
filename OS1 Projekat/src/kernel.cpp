/*
 * kernel.cpp
 *
 *  Created on: May 19, 2015
 *      Author: OS1
 */



#include "kernel.h"
#include <iostream.h>
#include <schedule.h>
#include <dos.h>
#include <stdlib.h>

typedef void interrupt (*InterPtr)(...);
InterPtr Kernel::oldTimer;
unsigned* Kernel::kernelStack = new unsigned [4096];
KernelList* Kernel::kernelList = new KernelList();
SleepList* Kernel::sleepList = new SleepList();
int Kernel::doScheduling = 0;
int Kernel::putIntoScheduler = 0;
PCB* Kernel::Idle = new PCB(1024, 1);

KernelList::KernelList() {
	first = last = 0;
}


void Kernel::initSystem() {
	#ifndef BCC_BLOCK_IGNORE
	oldTimer = getvect(0x08);

	setvect(0x60, oldTimer);
	setvect(0x08, newTimer);
	setvect(0x61, SysCall);
	#endif
	Kernel::Idle->startIdle(Kernel::emptyRun);
	PCB::running = new PCB(2048, 2);
}

void Kernel::restoreSystemAndClear() {
	#ifndef BCC_BLOCK_IGNORE
	setvect(0x08, oldTimer);
	#endif
	delete Kernel::kernelStack;
	delete Kernel::kernelList;
	delete Kernel::sleepList;
}

void Kernel::emptyRun() {
	while (1) {

	}
}

int KernelList::size() {
	Elem * cur = first;
	int ret = 0;
	while (cur != 0) {
		ret++;
		cur = cur->next;
	}
	return ret;
}

void KernelList::write(int id) {
	Elem * cur = first;
	while (cur != 0 && cur->id != id) cur = cur->next;
}

void KernelList::add(void* ptr, int id) {
	Elem* newElem = new Elem(ptr, id, 0);
	if (first) {
		last->next = newElem;
		last = newElem;
	} else {
		first = last = newElem;
	}
}

void KernelList::remove(int id) {
	Elem * cur = first;
	if (cur->id == id) {
		first = first->next;
		delete cur;
		if (first == 0) last = 0;
		return;
	}
	Elem * prev = first;
	cur = cur->next;
	while (cur->id != id) {
		cur = cur->next;
		prev = prev->next;
	}
	prev->next = cur->next;
	delete cur;
}

void* KernelList::find(int id) {
	Elem * cur = first;
	while (cur != 0 && cur->id != id) cur = cur->next;
	return cur->genPtr;
}


KernelList::~KernelList() { //To be done
	Elem * cur = first;
	while (cur != 0) {
		Elem * old = cur;
		cur = cur->next;
		delete old;
	}
}


SleepList::SleepList() {
	first = last = 0;
}

SleepList::~SleepList() {
	Elem* cur = first;
	while (cur != 0) {
		Elem* old = cur;
		cur = cur->next;
		delete old;
	}
}

int SleepList::timeLeft() {
	return first->timeToSleep;
}

void SleepList::decreaseTime() {
	first->timeToSleep--;
}

void SleepList::putInDreams(PCB* pcb, int timeToSleep) {
	if (first){
		if (timeToSleep < timeLeft()) {
			Elem* newElem = new Elem(pcb, timeToSleep, 0, 0);
			newElem->next = first;
			first->timeBehind = timeLeft() - timeToSleep;
			first = newElem;
		} else {
			Elem* newElem = new Elem(pcb, timeToSleep, 0, 0);
			Elem *cur, *prev;
			prev = first;
			timeToSleep -= first->timeToSleep;
			for (cur = first->next; cur != 0; cur = cur->next) {
				if (timeToSleep - cur->timeBehind <= 0) break;
				else timeToSleep -= cur->timeBehind;
				prev = prev->next;
			}
			newElem->timeBehind = timeToSleep;
			cur->timeBehind -= newElem->timeBehind;
			newElem->next = cur;
			prev->next = newElem;
		}
	} else {
		first = new Elem(pcb, timeToSleep, 0, 0);
	}
}

void SleepList::wakeUp() {
	Elem* cur = first;
	while (cur != 0 && cur->timeBehind == 0) {
		Scheduler::put(cur->pcb);
		Elem* old = cur;
		cur = cur->next;
		delete old;
	}
	first = cur;
	first->timeToSleep = first->timeBehind;
	first->timeBehind = 0;
}






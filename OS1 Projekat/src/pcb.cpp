/*
 * pcb.cpp
 *
 *  Created on: May 19, 2015
 *      Author: OS1
 */


#include "pcb.h"
#include <dos.h>
#include <iostream.h>
#include "schedule.h"
#include <stdlib.h>



int PCB::contextSwitchRequest = 0;
int PCB::lastID = 0;
int PCB::timePassed = 0;
int PCB::timeSince = 0;
PCB* PCB::running = NULL;

extern int lockFlag;



PCB::PCB(StackSize _myStackSize, Time _timeSlice) {
	myStackSize = _myStackSize;
	myTimeSlice = _timeSlice;
	stack = NULL;
	ss = sp = bp = 0;
	finished = 1;
	blocked = 0;
	first = last = 0;
}

void PCB::put(PCB* pcb) {
	Elem* newElem = new Elem(pcb, 0);
	if (!first) first = last = newElem;
	else last = last->next = newElem;
}

void PCB::signal() {
	Elem * cur = first;
	while (cur != 0) {
		cur->pcb->blocked = 0;
		Scheduler::put(cur->pcb);
		Elem * old = cur;
		cur = cur->next;
		delete old;
	}
	delete [] stack;
}

PCB::~PCB() {
}

void PCB::start(void (*body)(Thread*), Thread * myThread) {
#ifndef BCC_BLOCK_IGNORE
	stack = new unsigned [myStackSize];
	stack[myStackSize - 1] = FP_SEG(myThread);
	stack[myStackSize - 2] = FP_OFF(myThread);
	stack[myStackSize - 5] = 0x200;
	stack[myStackSize - 6] = FP_SEG(body);
	stack[myStackSize - 7] = FP_OFF(body);
	this->ss = FP_SEG(&stack[myStackSize - 16]);
	this->sp = FP_OFF(&stack[myStackSize - 16]);
	this->bp = FP_OFF(&stack[myStackSize - 16]);
#endif
	this->finished = 0;
	Scheduler::put(this);
}

void PCB::startIdle(void (*body)()) {
#ifndef BCC_BLOCK_IGNORE
	stack = new unsigned [myStackSize];
	stack[myStackSize - 3] = 0x200;
	stack[myStackSize - 4] = FP_SEG(body);
	stack[myStackSize - 5] = FP_OFF(body);
	this->ss = FP_SEG(&stack[myStackSize - 14]);
	this->sp = FP_OFF(&stack[myStackSize - 14]);
	this->bp = FP_OFF(&stack[myStackSize - 14]);
#endif
}


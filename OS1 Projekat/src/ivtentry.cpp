/*
 * ivtentry.cpp
 *
 *  Created on: May 21, 2015
 *      Author: OS1
 */

#include "ivtentry.h"
#include "dos.h"
#include "event.h"

IVTEntry* IVTEntry::entries[256];

IVTEntry::IVTEntry(unsigned char ivtNo, InterPtr newRoutine) {
	#ifndef BCC_BLOCK_IGNORE
	oldRoutine = getvect(ivtNo);
	setvect(ivtNo, newRoutine);
	#endif
	ivtno = ivtNo;
	entries[ivtNo] = this;
}

IVTEntry::~IVTEntry() {
	#ifndef BCC_BLOCK_IGNORE
	setvect(ivtno, oldRoutine);
	oldRoutine();
	#endif
}

void IVTEntry::signalEvent() {
	if (myEvent != 0) myEvent->signal();
}

void IVTEntry::setEvent(Event* e) {
	myEvent = e;
}

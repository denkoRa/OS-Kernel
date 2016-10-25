/*
 * ivtentry.h
 *
 *  Created on: May 21, 2015
 *      Author: OS1
 */

#ifndef IVTENTRY_H_
#define IVTENTRY_H_


#include "pcb.h"
class Event;

class IVTEntry {
public:
	IVTEntry(unsigned char ivtNo, InterPtr newRoutine);
	~IVTEntry();
	void signalEvent();
	void setEvent(Event*);
	static IVTEntry* entries[256];
	InterPtr oldRoutine;
	friend class Event;
private:
	Event* myEvent;

	unsigned char ivtno;
};



#endif /* IVTENTRY_H_ */

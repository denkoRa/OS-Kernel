/*
 * pcb.h
 *
 *  Created on: May 19, 2015
 *      Author: OS1
 */

#ifndef PCB_H_
#define PCB_H_


#include "Thread.h"

typedef void interrupt (*InterPtr)(...);

class PCB {
private:
	struct Elem {
		PCB* pcb;
		Elem* next;
		Elem(PCB* p, Elem* n) {
			pcb = p;
			next = n;
		}
	};
	Elem *first, *last;
public:
	PCB(StackSize myStackSize = 4096, Time timeSlice = 2);
	~PCB();
	void start(void (*)(Thread*), Thread*);
	void put(PCB*);
	void signal();
	void startIdle(void (*)());
	StackSize myStackSize;
	unsigned int* stack;
	unsigned sp;
	unsigned ss;
	unsigned bp;
	int finished, blocked;
	Time myTimeSlice;
	static PCB* running;

	static int timeSince;
	static int lastID;
	static int timePassed;
	static int contextSwitchRequest;

};




#endif /* PCB_H_ */

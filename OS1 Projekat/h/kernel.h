/*
 * kernel.h
 *
 *  Created on: May 19, 2015
 *      Author: OS1
 */

#ifndef KERNEL_H_
#define KERNEL_H_

#include "pcb.h"


class KernelList {
private:
	struct Elem {
		void* genPtr;
		int id;
		Elem *next;
		Elem(void* gP, int _id, Elem* n) {
			genPtr = gP;
			id = _id;
			next = n;
		}
	};
	Elem *first, *last;
public:
	KernelList();
	int size();
	void add(void* ptr, int id);
	void remove(int id);
	void* find(int id);
	void write(int id);
	~KernelList();
};


class SleepList {
private:
	struct Elem {
		PCB* pcb;
		int timeToSleep;
		int timeBehind;
		Elem* next;
		Elem(PCB* p, int tts, int tbf, Elem* n) {
			pcb = p;
			next = n;
			timeToSleep = tts;
			timeBehind = tbf;
		}
	};
	Elem *first, *last;
public:
	SleepList();
	~SleepList();
	void putInDreams(PCB*, int);
	int timeLeft();
	void decreaseTime();
	void wakeUp();

};


class Kernel {
public:
	static unsigned * kernelStack;
	static KernelList * kernelList;
	static SleepList * sleepList;
	static int doScheduling;
	static int putIntoScheduler;
	static PCB* Idle;
	static void emptyRun();
	static void initSystem();
	static void restoreSystemAndClear();
	static void interrupt SysCall(...);
	static void interrupt newTimer(...);
	static InterPtr oldTimer;
};




#endif /* KERNEL_H_ */

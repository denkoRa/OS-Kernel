/*
 * kernsem.h
 *
 *  Created on: May 19, 2015
 *      Author: OS1
 */

#ifndef KERNSEM_H_
#define KERNSEM_H_

#include "pcb.h"

class KernelSem {
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
	KernelSem(int init);
	void signal();
	void wait();
	int val();
	~KernelSem();
private:
	int value;
};


#endif /* KERNSEM_H_ */

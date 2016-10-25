/*
 * kernelev.h
 *
 *  Created on: May 21, 2015
 *      Author: OS1
 */

#ifndef KERNELEV_H_
#define KERNELEV_H_

#include "pcb.h"

typedef unsigned char IVTNo;

class KernelEv {
public:
	KernelEv(IVTNo, PCB*);
	~KernelEv();
	void signal();
	void wait(PCB*);
private:
	PCB* master;
	IVTNo ivtNo;
	int value;
};



#endif /* KERNELEV_H_ */

/*
 * semaphor.h
 *
 *  Created on: May 19, 2015
 *      Author: OS1
 */

#ifndef SEMAPHOR_H_
#define SEMAPHOR_H_



class KernelSem;


class Semaphore {
public:
	Semaphore (int init=1);
	virtual ~Semaphore();
	virtual void wait();
	virtual void signal();
	int val () const;
private:
	int myKernelSem;
};



#endif /* SEMAPHOR_H_ */

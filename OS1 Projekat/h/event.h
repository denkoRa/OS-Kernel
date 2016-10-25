/*
 * event.h
 *
 *  Created on: May 21, 2015
 *      Author: OS1
 */

#ifndef EVENT_H_
#define EVENT_H_

#include "ivtentry.h"

typedef unsigned char IVTNo;



class KernelEv;
class IVTEntry;



class Event {
public:
	Event (IVTNo ivtNo);
	~Event ();
	void wait  ();
protected:
	friend class KernelEv;
	friend class IVTEntry;
	void signal(); 	// can call KernelEv
private:
	int myKernelEv;
};

#define PREPAREENTRY(ivtNo, flag)\
	void interrupt newRoutine##ivtNo(...);\
	IVTEntry ivtEntry##ivtNo(ivtNo, newRoutine##ivtNo);\
	void interrupt newRoutine##ivtNo(...) {\
		ivtEntry##ivtNo.signalEvent();\
		if (flag) ivtEntry##ivtNo.oldRoutine();\
	}


#endif /* EVENT_H_ */

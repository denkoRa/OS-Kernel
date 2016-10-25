/*
 * main.cpp
 *
 *  Created on: May 19, 2015
 *      Author: OS1
 */



#include <dos.h>
#include <iostream.h>
#include <stdlib.h>
#include "thread.h"
#include "pcb.h"
#include "schedule.h"
#include "kernel.h"
#include "semaphor.h"

int lockFlag = 0;
int userMain(int, char*[]);

int main(int argc, char* argv[]) {
	Kernel::initSystem();
	int ret = userMain(argc, argv);
	Kernel::restoreSystemAndClear();
	return ret;
}


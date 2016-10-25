/*
 * lock.h
 *
 *  Created on: May 21, 2015
 *      Author: OS1
 */

#ifndef LOCK_H_
#define LOCK_H_

#include <dos.h>

#define lock 	#ifndef BCC_BLOCK_IGNORE asm cli; #endif
#define unlock 	#ifndef BCC_BLOCK_IGNORE asm sti; #endif



#endif /* LOCK_H_ */

/* getprio.c - getprio */

#include <conf.h>
#include <kernel.h>
#include <proc.h>
#include <stdio.h>

/*------------------------------------------------------------------------
 * getprio -- return the scheduling priority of a given process
 *------------------------------------------------------------------------
 */
SYSCALL getprio(int pid)
{
	STATWORD ps;    
	struct	pentry	*pptr;
	int prio;
	disable(ps);
	if (isbadpid(pid) || (pptr = &proctab[pid])->pstate == PRFREE) {
		restore(ps);
		return(SYSERR);
	}
	prio=pptr->pprio;
	if(pptr->pinh>0)
		prio=pptr->pinh;
	//kprintf(" here priority = %d, inh = %d, pprio = %d pid = %d\n",prio,pptr->pinh,pptr->pprio,pid);	
	restore(ps);
	return prio;
}

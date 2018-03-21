/* chprio.c - chprio */

#include <conf.h>
#include <kernel.h>
#include <proc.h>
#include <q.h>
#include <stdio.h>
#include <lock.h>
/*------------------------------------------------------------------------
 * chprio  --  change the scheduling priority of a process
 *------------------------------------------------------------------------
 */
SYSCALL chprio(int pid, int newprio)
{
	STATWORD ps;    
	struct	pentry	*pptr;

	disable(ps);
	if (isbadpid(pid) || newprio<=0 ||
	    (pptr = &proctab[pid])->pstate == PRFREE) {
		restore(ps);
		return(SYSERR);
	}
	pptr->pprio = newprio;
	if(pptr->pstate == PRREADY)
	{
		dequeue(pid);
		insert(pid,rdyhead,pptr->pprio);
	}
	priorityinheritance(pid);
	if(pptr->pstate==PRWAITFORLOCK)
	{
		//kprintf("hereeee chprio %d\n",pptr->lockid_waiting);
		priorityrampup(pptr->lockid_waiting);
		priorityinheritance(pptr->lockid_waiting);
	}
	restore(ps);
	return(newprio);
}

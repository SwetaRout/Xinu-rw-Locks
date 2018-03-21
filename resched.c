/* resched.c  -  resched */

#include <conf.h>
#include <kernel.h>
#include <proc.h>
#include <q.h>

unsigned long currSP;	/* REAL sp of current process */
extern int ctxsw(int, int, int, int);
/*-----------------------------------------------------------------------
 * resched  --  reschedule processor to highest priority ready process
 *
 * Notes:	Upon entry, currpid gives current process id.
 *		Proctab[currpid].pstate gives correct NEXT state for
 *			current process if other than PRREADY.
 *------------------------------------------------------------------------
 */
int resched()
{
	register struct	pentry	*optr;	/* pointer to old process entry */
	register struct	pentry	*nptr;	/* pointer to new process entry */
	int max=-1,prio,pid,k=0;
	/* no switch needed if current process priority higher than next*/

	if ( ( (optr= &proctab[currpid])->pstate == PRCURR) &&
	   (lastkey(rdytail)<optr->pprio)) {
		return(OK);
	}
	for(k=q[rdytail].qprev;k!=rdyhead;k=q[k].qprev)
	{
		prio=proctab[currpid].pprio;
		if(proctab[currpid].pinh>0)
		{
			prio=proctab[currpid].pinh;
		}
		if(prio>max)
		{
			max=prio;
			pid=k;
		}
	}
	/* force context switch */
	prio=optr->pprio;
	if(optr->pinh>0)
		prio=optr->pinh;
	if (optr->pstate == PRCURR) {
		if(max<prio)
			return OK;
		optr->pstate = PRREADY;
		insert(currpid,rdyhead,optr->pprio);
	}

	/* remove highest priority process at end of ready list */
	currpid=pid;
	dequeue(pid);
	nptr = &proctab[ (currpid = pid) ];
	nptr->pstate = PRCURR;		/* mark it currently running	*/
#ifdef	RTCLOCK
	preempt = QUANTUM;		/* reset preemption counter	*/
#endif
	
	ctxsw((int)&optr->pesp, (int)optr->pirmask, (int)&nptr->pesp, (int)nptr->pirmask);
	
	/* The OLD process returns here when resumed. */
	return OK;
}

/* kill.c - kill */

#include <conf.h>
#include <kernel.h>
#include <proc.h>
#include <sem.h>
#include <mem.h>
#include <io.h>
#include <q.h>
#include <stdio.h>
#include <lock.h>
/*------------------------------------------------------------------------
 * kill  --  kill a process and remove it from the system
 *------------------------------------------------------------------------
 */
SYSCALL kill(int pid)
{
	STATWORD ps;    
	struct	pentry	*pptr;		/* points to proc. table for pid*/
	int	dev;
	int i=0;
	disable(ps);
	if (isbadpid(pid) || (pptr= &proctab[pid])->pstate==PRFREE) {
		restore(ps);
		return(SYSERR);
	}
	if (--numproc == 0)
		xdone();

	dev = pptr->pdevs[0];
	if (! isbaddev(dev) )
		close(dev);
	dev = pptr->pdevs[1];
	if (! isbaddev(dev) )
		close(dev);
	dev = pptr->ppagedev;
	if (! isbaddev(dev) )
		close(dev);
	
	send(pptr->pnxtkin, pid);

	freestk(pptr->pbase, pptr->pstklen);
	switch (pptr->pstate) {

	case PRCURR:	
                                    
			pptr->pstate = PRFREE;	/* suicide */
			resched();

	case PRWAIT:	semaph[pptr->psem].semcnt++;
//	case PRWAITFORLOCK:
	case PRREADY:	
//kprintf("KILLLLLL  222222222\n");
/*						for(i=0;i<NLOCKS;i++)
				if(proctab[pid].lock_assign_tab[i]>0)
					releaseall(1,locktab[i].lockid);*/
			dequeue(pid);
			pptr->pstate = PRFREE;
			break;
	case PRWAITFORLOCK:
				//kprintf("KILLLLLL\n");
		
				dequeue(pid);
				locktab[proctab[pid].lockid_waiting].lproc[pid]=0;
				priorityrampup(proctab[pid].lockid_waiting);
				for(i=0;i<NPROC;i++)
				{
					if(locktab[proctab[pid].lockid_waiting].lproc[i]==1)
					{
						//kprintf("killing\n");
						priorityinheritance(i);
					//	releaseall(1,proctab[pid].lockid_waiting);
					//	priorityinheritance(i);
					}
				}
				pptr->pstate=PRFREE;
				break;

	case PRSLEEP:
	case PRTRECV:	unsleep(pid);
						/* fall through	*/
	default:
			for(i=0;i<NLOCKS;i++)
                                if(proctab[pid].lock_assign_tab[i]>0)
                                        releaseall(1,locktab[i].lockid);

			pptr->pstate = PRFREE;
	}
	restore(ps);
	return(OK);
}

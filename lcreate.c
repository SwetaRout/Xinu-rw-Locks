#include <stdio.h>
#include <kernel.h>
#include <proc.h>
#include <q.h>
#include <lock.h>

int lcreate()
{
	STATWORD ps;
	int lock,i,j;
	disable(ps);
//	 kprintf("\n ******* LOCK CREATED ******\n");
	for(i=0;i<NLOCKS;i++)
	{
//		kprintf("\n ******* LOCK CREATED ******\n");
		lock=lock_available+1;
		lock_available++;
		if(lock_available>NLOCKS)
			lock_available=1;
		if(locktab[lock].is_free==1)
		{
			//kprintf("FREE LOCK = %d\n",lock);
			locktab[lock].is_free=0;
			locktab[lock].lockid=lock;
			locktab[lock].nr=locktab[lock].nw=0;
			for(j=0;j<NPROC;j++)
				proctab[j].lock_assign_tab[lock]=0;
			restore(ps);
			return lock;
		}
	}
	restore(ps);
	return SYSERR;
}	


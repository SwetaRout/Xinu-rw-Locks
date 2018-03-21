#include <kernel.h>
//#include <conf.h>
#include <proc.h>
#include <q.h>
#include <stdio.h>
#include <lock.h>
#include <sleep.h>
struct reader_writer_lock locktab[50];
int lock_available;
void linit()
{
//	STATWORD ps;
	struct reader_writer_lock *lock=NULL;
	int i=0,j=0;
	lock_available=1;
	extern unsigned long ctr1000;
//	disable(ps);
//kprintf("\n********** LOCKS INITI **************\n");
	for(i=0;i<NLOCKS;i++)
	{
		lock=&locktab[i];
		lock->lprio=-1;
		lock->nr=0;
		lock->nw=0;
		lock->is_free=1;
		lock->lockQtail = 1 + (lock->lockQhead = newqueue());
		//lock->writeQtail = 1 + (lock->writeQhead = newqueue());
		for(j=0;j<NPROC;j++)
		{
			proctab[j].pretval_isDELETED=0;
			proctab[j].lockwaitingtime=ctr1000;
			proctab[j].lock_assign_tab[i]=0;
			lock->lproc[j]=0;
			proctab[j].pinh=0;
			proctab[j].lockid_waiting=-1;
		}
	}
//	restore(ps);
}
	

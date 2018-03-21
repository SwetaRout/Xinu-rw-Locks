#include <stdio.h>
#include <kernel.h>
#include <proc.h>
#include <q.h>
#include <lock.h>

int ldelete (int lockdescriptor)
{
	STATWORD ps;
	struct reader_writer_lock *rwlock=NULL;	
	int i=0,pid;
	disable(ps);
	rwlock=&locktab[lockdescriptor];
	if(rwlock->is_free==1)
	{
		restore(ps);
		return SYSERR;
	}
	if(isempty(rwlock->lockQhead))
	{
		restore(ps);
		return SYSERR;
	}
	rwlock->is_free=1;
	rwlock->nr=rwlock->nw=0;
	rwlock->lockstate=DELETED;
	pid=getfirst(rwlock->lockQhead);
	while(pid!=EMPTY)
	{	
		proctab[i].pretval_isDELETED=1;
		ready(pid,RESCHNO);
		pid=getfirst(rwlock->lockQhead);
	}
	resched();
	for(i=0;i<NPROC;i++)
		proctab[i].lock_assign_tab[lockdescriptor]=0;
	
	restore(ps);
	return OK;
}
	

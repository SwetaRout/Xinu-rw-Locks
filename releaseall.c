#include <stdio.h>
#include <kernel.h>
#include <proc.h>
#include <q.h>
#include <lock.h>

int releaseall (int numlocks, int ldes1, ...)
{
	STATWORD ps;
	int i=0,k,lid,next;
	unsigned long *pargs=(unsigned long *)(&ldes1)+(numlocks-1);
	disable(ps);
//kprintf("rellllllllll   %d\n",numlocks);
	for(;numlocks>0;numlocks--)
	{
		lid=*pargs++;
//kprintf("release lockid  %d\n",lid);


		if(locktab[lid].is_free)
		{
//kprintf("RETURNS FROM HERE\n");
			resched();
			restore(ps);
			return SYSERR;
		}
//kprintf("RELEASEEEE\n");	

		proctab[currpid].lock_assign_tab[lid]=proctab[currpid].lock_assign_tab[lid]-1;
		locktab[lid].lproc[currpid]=0;
		if(locktab[lid].nr>0 && locktab[lid].nw==0)
			locktab[lid].nr--;
		else if(locktab[lid].nr==0 && locktab[lid].nw==1)
			locktab[lid].nw--;
		else
			;
		priorityinheritance(currpid);
		if(locktab[lid].nr == 0 && locktab[lid].nw==0)
		{
//kprintf("out of loop\n");
			next = get_next_item(lid);
/*kprintf("next = %d",next);
for(k=q[locktab[lid].lockQtail].qprev;k!=locktab[lid].lockQhead;k=q[k].qprev)
{
        kprintf("\n*********** queue values : %d,%d   *********\n",lid,q[k].qkey);
}*/

			while(next!=-1)
			{
//		kprintf("inside loop %d \n",next);	
/*for(k=q[locktab[lid].lockQtail].qprev;k!=locktab[lid].lockQhead;k=q[k].qprev)
{
	kprintf("\n*********** queue values : %d   *********\n",q[k].qkey);
}*/
				if(q[next].qtype==READ)
				{
//kprintf("READ %d\n",next);
//next=46;
					proctab[next].lock_assign_tab[lid]++;
					locktab[lid].lproc[next] = 1;
					
					priorityrampup(lid);
					for(i=0; i<NPROC; i++)
						if(locktab[lid].lproc[i] ==1)
							priorityinheritance(i);
   
					dequeue(next);
					locktab[lid].nr++;
					ready(next,RESCHNO);
					if((next=get_next_item(lid))!=-1 && (q[next].qtype==WRITE))
					{
						next=-1;
						break;
					}
				}
				else
				{
//kprintf("WRITE %d\n",next);
					proctab[next].lock_assign_tab[lid]++;
					locktab[lid].nw++;
					locktab[lid].lproc[next]  = 1;

					priorityrampup(lid);
					for(i=0; i<NPROC; i++)
						if(locktab[lid].lproc[i] ==1 )
							priorityinheritance(i);
   
					dequeue(next);
					ready(next,RESCHNO);
					break;
				}
					
			}
		}
	}
	resched();
	restore(ps);
	return OK;
}

int get_next_item(int lid)
{
//	STATWORD ps;
	int next=q[locktab[lid].lockQtail].qprev;
	int k=0;
//	disable(ps);
	if(next ==locktab[lid].lockQhead)
	{
//		restore(ps);
//kprintf("returns here\n");		
		return -1;
	}
	for(k=q[next].qprev;k!=locktab[lid].lockQhead;k=q[k].qprev)
	{

		if(q[k].qkey<q[next].qkey)
		{
//			restore(ps);
//kprintf("HERE PRIO\n");			
			return next;
		}
		else if(q[k].qkey==q[next].qkey)
		{
//		kprintf("same priority\n");	
			if((proctab[next].lockwaitingtime-proctab[k].lockwaitingtime)<1)
				if(q[next].qtype==READ && q[k].qtype==WRITE)
					next=k;
			if(proctab[next].lockwaitingtime>proctab[k].lockwaitingtime)
				next=k;
		}
	}
//	restore(ps);
//kprintf("HERE RET\n");	
	return next;
}			
		
	
		

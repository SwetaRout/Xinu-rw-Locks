#include <stdio.h>
#include <kernel.h>
#include <proc.h>
#include <q.h>
#include <lock.h>
#include <sleep.h>

int lock (int ldes1, int type, int priority)
{
//kprintf("\n**************hereeeeee   %d       %d\n",locktab[ldes1],priority);
        STATWORD ps;
         int k=0,i;
	 extern unsigned long ctr1000;
         disable(ps);
	if(locktab[ldes1].is_free==1)
	{
//		kprintf("\n Returns at free\n");
		restore(ps);
		return SYSERR;
	}
	
	if(locktab[ldes1].nr==0 && locktab[ldes1].nw==0)
	{
//		kprintf(" both are zero%d and  %d\n",currpid,ldes1);
		proctab[currpid].lock_assign_tab[ldes1]++;
		locktab[ldes1].lproc[currpid]=1;
		priorityinheritance(currpid);
		if(type==READ)
			locktab[ldes1].nr++;
		else
			locktab[ldes1].nw++;
		restore(ps);
		return OK;
	}		
	
	if(locktab[ldes1].nr>0 && locktab[ldes1].nw==0)
	{
//kprintf(" line 32\n");
		if(type==READ)
		{
			for(k=q[locktab[ldes1].lockQtail].qprev;k!=locktab[ldes1].lockQhead&&priority<q[k].qkey;k=q[k].qprev)
			{
				if(q[k].qtype==WRITE)
				{
					proctab[currpid].pstate = PRWAITFORLOCK;
					proctab[currpid].pretval_isDELETED=0;
					proctab[currpid].lockwaitingtime=ctr1000;
					proctab[currpid].lockid_waiting=ldes1;
					insert(currpid,locktab[ldes1].lockQhead,priority);
					priorityrampup(ldes1);
					q[currpid].qtype=type;
					for(i=0;i<NPROC;i++)
						if(locktab[ldes1].lproc[i]==1)
							priorityinheritance(i);
					resched();	
					restore(ps);
					return (proctab[currpid].pretval_isDELETED==1?DELETED:OK);
				}		
			}
			proctab[currpid].lock_assign_tab[ldes1]++;
			locktab[ldes1].lproc[currpid]=1;
			priorityinheritance(currpid);
                       	locktab[ldes1].nr++;
			restore(ps);
	                return OK;

		}
//		restore(ps);
//		return SYSERR;
	}
//	if(locktab[ldes1].nr>=0 && locktab[ldes1].nw==1)
	if((locktab[ldes1].nr==0 && locktab[ldes1].nw==1)||(locktab[ldes1].nr>0 && locktab[ldes1].nw==0&&type==WRITE))	
	{
//kprintf("line 71\n");
		proctab[currpid].pstate = PRWAITFORLOCK;
                proctab[currpid].pretval_isDELETED=0;
		proctab[currpid].lockid_waiting=ldes1;
                proctab[currpid].lockwaitingtime=ctr1000;
                insert(currpid,locktab[ldes1].lockQhead,priority);
	        priorityrampup(ldes1);
                q[currpid].qtype=type;
                for(i=0;i<NPROC;i++)
	                if(locktab[ldes1].lproc[i]==1)
        	                priorityinheritance(i);
/* for(k=q[locktab[ldes1].lockQtail].qprev;k!=locktab[ldes1].lockQhead;k=q[k].qprev)
        {
                kprintf(" Queue values :\n %d,%d********\n",ldes1,q[k].qkey);
        }*/

                resched();
		restore(ps);
                return (proctab[currpid].pretval_isDELETED==1?DELETED:OK);

	}
	
	restore(ps);
	return OK;

}

void priorityinheritance(int pid)
{
	//kprintf("enter inh...%d    %d.\n",proctab[pid].pprio,proctab[pid].pinh);
	int max=0,i;
	
	for(i=0;i<NLOCKS;i++)
	{
		if(proctab[pid].lock_assign_tab[i]>0 && locktab[i].lprio>max)// && !isempty(locktab[i].lockQhead))
		{
			max=locktab[i].lprio;
		}
	}	
	if(max>proctab[pid].pprio)
		proctab[pid].pinh=max;
	else
		proctab[pid].pinh=0;
//kprintf("enter inh...pprio = %d   inh =  %d  pid = %d.\n",proctab[pid].pprio,proctab[pid].pinh,pid);	
}

void priorityrampup(int lid)
{
//kprintf("enter rampup ...........\n");
	int max=0,i;
	if(isempty(locktab[lid].lockQhead))
		return;

	for(i=q[locktab[lid].lockQtail].qprev;i!=locktab[lid].lockQhead;i=q[i].qprev)
	{
		if(proctab[i].pprio>max)
			max = proctab[i].pprio;
	}
	locktab[lid].lprio=max;
}

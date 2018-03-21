#include <conf.h>
#include <kernel.h>
#include <sem.h>
#include <lock.h>
#include <proc.h>
#include <stdio.h>
#include <sleep.h>

extern unsigned long ctr1000;

int s1;
int lck;
int pa, pb, pc;

void proc_A_1(void)
{
    unsigned long t1, t2, i = 0, j = 0;

    t1 = ctr1000;
    wait(s1);
    t2 = ctr1000;
  
    for (j = 0; j < 10; ++j) {
        sleep1000(2);
        while (++i)
            if (100 == i) break;
    }

	signal(s1);

    return;
}

void proc_B(void)
{
    unsigned long i = 0, j = 0;

     for (j = 0; j < 10; ++j) {
        sleep1000(2);
        while (++i)
            if (100 == i) break;
    }

    return;
}

void proc_A_2(void)
{
    unsigned long t1, t2, i = 0, j = 0;
    t1 = ctr1000;
    lock(lck, WRITE, 50);
    t2 = ctr1000;
    for (j = 0; j < 10; ++j) {
        sleep1000(2);
        while (++i)
            if (100 == i) break;
    }

    releaseall(1, lck);

    return;
}
    


void task1(void)
{
    /* Test Xinu's semaphore */
    s1 = screate(1);
    pa = create((int *) proc_A_1, 512, 20, "procA", 0, 0);
	pb = create((int *) proc_B, 512, 25, "procB", 0, 0);
	pc = create((int *) proc_A_1, 512, 30, "procC", 0, 0);
	resume(pa);
    resume(pb);
    resume(pc);
    sleep(1);
    wait(s1);
    signal(s1);
    sdelete(s1);
 

    sleep(2);

    /* Test PA2 locks */
    kprintf("\n\nTesting PA2 locks..\n");
    lck = lcreate();
	pa = create((int *) proc_A_2, 512, 20, "procA", 0, 0);
	pb = create((int *) proc_B, 512, 25, "procB", 0, 0);
	pc = create((int *) proc_A_2, 512, 30, "procC", 0, 0);
	resume(pa);
    resume(pb);
    resume(pc);
    sleep(1);
    lock(lck, WRITE, 50);
    releaseall(1, lck);
    ldelete(lck);
    

    return;
}

#define NLOCKS 50
#define DELETED_LOCK 0
#define READ 1
#define WRITE 2
struct reader_writer_lock
{
	int lockid;	//lock id
	int lockstate;	//lock state
	int lprio;
	int nr;	//num of readers
	int nw;	//num of writers
	int lockQhead;	//Q headi
	int lockQtail;	//Q tail
	int is_free;	//if lock is available	
	int is_new;
	int lproc[NPROC];
};
extern struct reader_writer_lock locktab[];
extern int lock_available;
void linit();
int lcreate();
int ldelete (int lockdescriptor);
int lock (int ldes1, int type, int priority);
int releaseall (int numlocks, int ldes1, ...);
void priorityrampup(int lid);
void priorityinheritance(int pid);
int get_next_item(int lid);

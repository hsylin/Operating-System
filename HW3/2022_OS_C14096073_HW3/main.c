#include <stdlib.h>
#include <time.h>
#include <ucontext.h>
#include <signal.h>
#include "include/shell.h"
#include "include/command.h"
#include "include/scheduler.h"
#include <sys/time.h>
struct task job[10000]={
0,
NULL,
{0,0,0,0,0,0,0,0,0,0},
0,
0,
0,
{0,0,0,0,0,0,0,0},
10000,
0,
0,
{0,0,0,0,0,0,0,0},
0

};
struct task waiting[10000]={
0,
NULL,
{0,0,0,0,0,0,0,0,0,0},
0,
0,
0,
{0,0,0,0,0,0,0,0},
10000,
0,
0,
{0,0,0,0,0,0,0,0},
0
};
struct task ready[10000]={
0,
NULL,
{0,0,0,0,0,0,0,0,0,0},
0,
0,
0,
{0,0,0,0,0,0,0,0},
10000,
0,
0,
{0,0,0,0,0,0,0,0},
0
};
struct task pre_waiting[10000]={
0,
NULL,
{0,0,0,0,0,0,0,0,0,0},
0,
0,
0,
{0,0,0,0,0,0,0,0},
10000,
0,
0,
{0,0,0,0,0,0,0,0},
0
};
struct task pre_ready[10000]={
0,
NULL,
{0,0,0,0,0,0,0,0,0,0},
0,
0,
0,
{0,0,0,0,0,0,0,0},
10000,
0,
0,
{0,0,0,0,0,0,0,0},
0
};

int k=1;
ucontext_t ctx_main;
ucontext_t ctx_sche;
int res[8]={1,1,1,1,1,1,1,1};
int once=0;
char* alog;
int w=1;
int r=1;
int wh=1;
int rh=1;
int pw=1;
int pr=1;
int pwh=1;
int prh=1;
int runningnumber=0;
int old_runningnumber=0;
int c=0;
int count_RR=0;
int count_ms=0;
struct task *top_w=&waiting[1];
struct task *top_r=&ready[1];
struct task *top_pw=&pre_waiting[1];
struct task *top_pr=&pre_ready[1];
struct itimerval value_new;


void sig_handler(int n)
{
    /*value_new.it_value.tv_sec = 0;  
    value_new.it_value.tv_usec = 0;
    value_new.it_interval.tv_sec = 0;
    value_new.it_interval.tv_usec = 0;
    setitimer(ITIMER_VIRTUAL, &value_new, NULL);*/
    if(runningnumber!=0)
    swapcontext(&job[runningnumber].ctx,&ctx_main);
    else if(old_runningnumber!=0)
    swapcontext(&job[old_runningnumber].ctx,&ctx_main);
    else if(old_runningnumber==0&&runningnumber==0)
    swapcontext(&ctx_sche,&ctx_main);
}


int main(int argc, char *argv[])
{
	for(int i=0;i<10000;i++)
{
	job[i].priority=10000;
	waiting[i].priority=10000;
	ready[i].priority=10000;
	pre_ready[i].priority=10000;
	pre_waiting[i].priority=10000;
	
}
	history_count = 0;
	for (int i = 0; i < MAX_RECORD_NUM; ++i)
    	history[i] = (char *)malloc(BUF_SIZE * sizeof(char));
	
	
	alog=argv[1];
	
	char stack1[16384];
	char stack2[16384];
	getcontext(&ctx_main);
	ctx_main.uc_stack.ss_sp =stack1;
	ctx_main.uc_stack.ss_size = 16384;
	ctx_main.uc_stack.ss_flags = 0;
    	makecontext(&ctx_main,shell,0);
    	
    getcontext(&ctx_sche);	
    ctx_sche.uc_stack.ss_sp =stack2;
	ctx_sche.uc_stack.ss_size = 16384;
	ctx_sche.uc_stack.ss_flags = 0;
    makecontext(&ctx_sche,sche_exec,0);
    	
    	struct sigaction sa;
    	sa.sa_handler = sig_handler;
    	sa.sa_flags = 0;
    	sigemptyset(&sa.sa_mask);

    	if ( sigaction(SIGTSTP, &sa, NULL) == -1 ) {
        perror("Couldn't set SIGTSTP handler");
        exit(EXIT_FAILURE);
   	 }

	shell();

	for (int i = 0; i < MAX_RECORD_NUM; ++i)
    	free(history[i]);

	return 0;
}

#ifndef COMMAND_H
#define COMMAND_H

#define MAX_RECORD_NUM 16
#define BUF_SIZE 1024
#include <time.h>
#include <stdbool.h>
#include <ucontext.h>
#include <signal.h>
#include <sys/time.h>
struct pipes {
	char **args;
	int length;
	struct pipes *next;
};

struct cmd {
	struct pipes *head;
	bool background;
    char *in_file, *out_file;
};

char *history[MAX_RECORD_NUM];
int history_count;

char *read_line();
struct cmd *split_line(char *);
void test_cmd_struct(struct cmd *);

struct task{

int TID;
char* task_name;
char task_state[10];
int running_time;
int waiting_time;
int turnaround_time;
int resources[8];
int priority;
int count_sleep;
int count_resources;
int want_resources[8];
int has_res;
ucontext_t ctx;


};
extern struct task job[10000];
extern int k;
extern ucontext_t ctx_main;
extern ucontext_t ctx_sche;
extern int res[8];
extern int runningnumber;
extern int old_runningnumber;
extern struct task waiting[10000];
extern struct task ready[10000];
extern struct task pre_waiting[10000];
extern struct task pre_ready[10000];
extern struct task *top_w;
extern struct task *top_r;
extern struct task *top_pw;
extern struct task *top_pr;

extern int* is_res[100];
extern int once;
extern char* alog;
extern int w;
extern int r;
extern int pw;
extern int pr;

extern int wh;
extern int rh;

extern int pwh;
extern int prh;

extern int c;
extern int count_RR;

extern int count_ms;
extern struct itimerval value_new;
    

#endif
//printf("top_r->TID=%d\n",top_r->TID);
	//printf("top_r->task_state=%s\n",top_r->task_state);
	//printf(" r=%d ",r);
	//printf("rh=%d\n",rh);
	/*printf("top_w->TID=%d",top_w->TID);
	printf("top_w->count_sleep=%d",top_w->count_sleep);*/

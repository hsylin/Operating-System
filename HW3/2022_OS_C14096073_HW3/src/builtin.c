#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <dirent.h>
#include <fcntl.h>
#include <time.h>
#include <ucontext.h>
#include <signal.h>
#include "../include/builtin.h"
#include "../include/command.h"
#include "../include/function.h"
#include "../include/resource.h"
#include "../include/scheduler.h"
#include <sys/time.h>
const char *func_str[] = {
 	"task1",
 	"task2",
	"task3",
 	"task4",
 	"task5",
	"task6",
	"task7",
	"task8",
	"task9",
	"test_sleep",
	"test_exit",
	"idle",
	"test_resource1",
	"test_resource2",
};

const void (*func_func[]) (void ) = {
	&task1,
	&task2,
	&task3,
	&task4,
  	&task5,
	&task6,
	&task7,
	&task8,
	&task9,
	&test_sleep,
	&test_exit,
	&idle,
	&test_resource1,
	&test_resource2,
};
int help(char **args)
{
	int i;
    printf("--------------------------------------------------\n");
  	printf("My Little Shell!!\n");
	printf("The following are built in:\n");
	for (i = 0; i < num_builtins(); i++) {
    	printf("%d: %s\n", i, builtin_str[i]);
  	}
	printf("%d: replay\n", i);
    printf("--------------------------------------------------\n");
	return 1;
}

int cd(char **args)
{
	if (args[1] == NULL) {
    	fprintf(stderr, "lsh: expected argument to \"cd\"\n");
  	} else {
    	if (chdir(args[1]) != 0)
      		perror("lsh");
	}
	return 1;
}

int echo(char **args)
{
	bool newline = true;
	for (int i = 1; args[i]; ++i) {
		if (i == 1 && strcmp(args[i], "-n") == 0) {
			newline = false;
			continue;
		}
		printf("%s", args[i]);
		if (args[i + 1])
			printf(" ");
	}
	if (newline)
		printf("\n");

	return 1;
}

int exit_shell(char **args)
{
	return 0;
}

int record(char **args)
{
	if (history_count < MAX_RECORD_NUM) {
		for (int i = 0; i < history_count; ++i)
			printf("%2d: %s\n", i + 1, history[i]);
	} else {
		for (int i = history_count % MAX_RECORD_NUM; i < history_count % MAX_RECORD_NUM + MAX_RECORD_NUM; ++i)
			printf("%2d: %s\n", i - history_count % MAX_RECORD_NUM + 1, history[i % MAX_RECORD_NUM]);
	}
	return 1;
}

bool isnum(char *str)
{
	for (int i = 0; i < strlen(str); ++i) {
    	if(str[i] >= 48 && str[i] <= 57)
			continue;
        else
		    return false;
  	}
  	return true;
}

int mypid(char **args)
{
	char fname[BUF_SIZE];
	char buffer[BUF_SIZE];
	if(strcmp(args[1], "-i") == 0) {

	    pid_t pid = getpid();
	    printf("%d\n", pid);
	
	} else if (strcmp(args[1], "-p") == 0) {
	
		if (args[2] == NULL) {
      		printf("mypid -p: too few argument\n");
      		return 1;
    	}

    	sprintf(fname, "/proc/%s/stat", args[2]);
    	int fd = open(fname, O_RDONLY);
    	if(fd == -1) {
      		printf("mypid -p: process id not exist\n");
     		return 1;
    	}

    	read(fd, buffer, BUF_SIZE);
	    strtok(buffer, " ");
    	strtok(NULL, " ");
	    strtok(NULL, " ");
    	char *s_ppid = strtok(NULL, " ");
	    int ppid = strtol(s_ppid, NULL, 10);
    	printf("%d\n", ppid);
	    
		close(fd);

  	} else if (strcmp(args[1], "-c") == 0) {

		if (args[2] == NULL) {
      		printf("mypid -c: too few argument\n");
      		return 1;
    	}

    	DIR *dirp;
    	if ((dirp = opendir("/proc/")) == NULL){
      		printf("open directory error!\n");
      		return 1;
    	}

    	struct dirent *direntp;
    	while ((direntp = readdir(dirp)) != NULL) {
      		if (!isnum(direntp->d_name)) {
        		continue;
      		} else {
        		sprintf(fname, "/proc/%s/stat", direntp->d_name);
		        int fd = open(fname, O_RDONLY);
        		if (fd == -1) {
          			printf("mypid -p: process id not exist\n");
          			return 1;
        		}

        		read(fd, buffer, BUF_SIZE);
        		strtok(buffer, " ");
        		strtok(NULL, " ");
        		strtok(NULL, " ");
		        char *s_ppid = strtok(NULL, " ");
		        if(strcmp(s_ppid, args[2]) == 0)
		            printf("%s\n", direntp->d_name);

        		close(fd);
     		}
	   	}
    	
		closedir(dirp);
	
	} else {
    	printf("wrong type! Please type again!\n");
  	}
	
	return 1;
}

int add(char **args)
{
	job[k].TID=k;
	char* name=malloc(sizeof(char) * (1000));
	memcpy(name,args[1],strlen(args[1])+1);
	job[k].task_name=name;
	strcpy(job[k].task_state,"ready");
	if(strcmp(alog,"PP") == 0)
	{
		job[k].priority=atoi(args[3]);
	}
	
	
	char* stack=malloc(sizeof(char) * (16384));
	for (int i = 0; i < 14; ++i)
	{
		if (strcmp(args[2], func_str[i]) == 0)
		{
		getcontext(&job[k].ctx);
		job[k].ctx.uc_stack.ss_sp =stack;
		job[k].ctx.uc_stack.ss_size = 16384;
		job[k].ctx.uc_stack.ss_flags = 0;
    		makecontext(&job[k].ctx,func_func[i],0);
		break;
		}
	}
	printf("Task %s is ready.\n",job[k].task_name);
	
	ready[r].TID=job[k].TID;
	
	char* name_copy=malloc(sizeof(char) * (1000));
	memcpy(name_copy,args[1],strlen(args[1])+1);
	ready[r].task_name=name_copy;
	strcpy(ready[r].task_state,job[k].task_state);
	ready[r].running_time=job[k].running_time;
	ready[r].waiting_time=job[k].waiting_time;
	ready[r].turnaround_time=job[k].turnaround_time;
	ready[r].priority=job[k].priority;
	ready[r].ctx=job[k].ctx;
	ready[r].count_sleep=job[k].count_sleep;
	ready[r].count_resources=job[k].count_resources;
	k++;
	r++;
	
	
	return 1;
}

int del(char **args)
{
	for(int i=1;i<k;i++)
	{
		if(strcmp(job[i].task_name,args[1]) == 0)
		{
			
				for(int i=1;i<w;i++)
				{
					if(strcmp(waiting[i].task_name,args[1]) == 0)
					{
							strcpy(waiting[i].task_state,"TERMINATED");
							
							release_resources(waiting[i].count_resources,waiting[i].want_resources);
						
					}
				
				}
			
				for(int i=1;i<r;i++)
				{
					if(strcmp(ready[i].task_name,args[1]) == 0)
					{
						strcpy(ready[i].task_state,"TERMINATED");				
						
						release_resources(ready[i].count_resources,ready[i].want_resources);
						
					}
				}
			
			
				for(int i=1;i<pw;i++)
				{
					if(strcmp(pre_waiting[i].task_name,args[1]) == 0)
					{
						strcpy(pre_waiting[i].task_state,"TERMINATED");					
						
						release_resources(pre_waiting[i].count_resources,pre_waiting[i].want_resources);
						
					}
				
				}
			
				for(int i=1;i<pr;i++)
				{
					if(strcmp(pre_ready[i].task_name,args[1]) == 0)
					{
						strcpy(pre_ready[i].task_state,"TERMINATED");					
						
						release_resources(pre_ready[i].count_resources,pre_ready[i].want_resources);
						
						
					}
				
				}
				
				
			strcpy(job[i].task_state,"TERMINATED");			
			
			release_resources(job[i].count_resources,job[i].want_resources);
			printf("Task %s is killed.\n",job[i].task_name);
			break;
		}
	}
	
	return 1;
}

int ps(char **args)
{
	printf("             TID|            name|           state|         running|         waiting|      turnaround|       resources|        priority\n");
	printf("---------------------------------------------------------------------------------------------------------------------------------------\n");
	for(int i=1;i<k;i++)
	{
	printf("%16d|%16s|%16s|%16d|%16d|",job[i].TID,job[i].task_name,job[i].task_state,job[i].running_time,job[i].waiting_time);
	if(strcmp(job[i].task_state, "TERMINATED") == 0)
	{
		printf("%16d|",job[i].turnaround_time);
	}
	else
	{
		printf("            none|");
	}
	
	int none=1;
	for(int q=0;q<8;q++)
	{
		if(job[i].resources[k]!=0)
		{
			none=0;
		}
	
	}
	if(none==0)
	{
	for(int j=0;j<8;j++)
	{
		if(job[i].resources[j]!=0)
		{
			printf(" %d",j);
			
		}
		else 
		{
			printf("  ");
		}
		
		
	}
	}
	else
	{
		printf("            none");
	}
	printf("|");
	
	if(job[i].priority!=0&&strcmp(alog,"PP")==0)
	printf("%16d|\n",job[i].priority);
	else
	printf("            none\n");
	
	}
	return 1;
}

int start(char **args)
{
	/*value_new.it_value.tv_sec = 0;  
    	value_new.it_value.tv_usec = 10000;
    	value_new.it_interval.tv_sec = 0;
    	value_new.it_interval.tv_usec = 10000;
    	setitimer(ITIMER_VIRTUAL, &value_new, NULL);*/
	if(c==0)
	{
	
	printf("Start simulation.\n");
	swapcontext(&ctx_main,&ctx_sche);
	c++;
	}else
	{
		printf("Start simulation.\n");
		if(runningnumber!=0)
        	swapcontext(&ctx_main,&job[runningnumber].ctx);
        	else if(old_runningnumber!=0)
        	swapcontext(&ctx_main,&job[old_runningnumber].ctx);
        	else if(old_runningnumber==0&&runningnumber==0)
        	swapcontext(&ctx_main,&ctx_sche);
        	
	}
	return 1;
}

const char *builtin_str[] = {
 	"help",
 	"cd",
	"echo",
 	"exit",
 	"record",
	"mypid",
	"add",
	"del",
	"ps",
	"start"
};

const int (*builtin_func[]) (char **) = {
	&help,
	&cd,
	&echo,
	&exit_shell,
  	&record,
	&mypid,
	&add,
	&del,
	&ps,
	&start
};


int num_builtins() {
	return sizeof(builtin_str) / sizeof(char *);
}

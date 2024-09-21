#include "../include/resource.h"
#include "../include/command.h"
#include "../include/scheduler.h"

#include <time.h>
#include <ucontext.h>
#include <signal.h>
#include <sys/time.h>
void get_resources(int count, int *resources)
{
	
	
	while(1)
	{
	
	int yes=1;
	for(int i=0;i<count;i++)
	{
		if(res[resources[i]]==0)
		{
			yes=0;
			break;
		}
	}
	if(yes)
	{
		for(int i=0;i<count;i++)
		{
			res[resources[i]]=0;
			job[runningnumber].resources[resources[i]]=1;

			printf("Task %s gets resouces %d.\n",job[runningnumber].task_name,resources[i]);
		}
		job[runningnumber].has_res=1;
		for(int i=0;i<8;i++)
		{
			job[runningnumber].want_resources[i]=0;
		}
		job[runningnumber].count_resources=0;
		break;
		
	}
	else
	{
		
		printf("Task %s is waiting resource.\n",job[runningnumber].task_name);
		job[runningnumber].has_res=0;
		job[runningnumber].count_resources=count;
		int store=runningnumber;
		for(int i=0;i<count;i++)
		{
			job[runningnumber].want_resources[resources[i]]=1;
		}
		
		running_to_waiting();
		while(strcmp(job[store].task_state,"waiting")==0);
		
		
		
		
		
	}
	
	}
	
}

void release_resources(int count, int *resources)
{
	for(int i=0;i<count;i++)
	{
			
			res[resources[i]]=1;
			job[runningnumber].resources[resources[i]]=0;
			job[runningnumber].want_resources[resources[i]]=0;
			job[runningnumber].count_resources=0;
			printf("Task %s releases resource %d.\n",job[runningnumber].task_name,resources[i]);
	}
	job[runningnumber].has_res=0;
		
    
}

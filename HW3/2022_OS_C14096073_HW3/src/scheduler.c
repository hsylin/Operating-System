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
#include <sys/time.h>
#include "../include/builtin.h"
#include "../include/command.h"
#include "../include/function.h"

int cmpfunc (const void * a, const void * b)
{
   struct task *pa=(struct task *)a;
   struct task *pb=(struct task *)b;
   return ( pa->priority - pb->priority );
}

void waiting_to_ready()
{

	for(int i=1;i<w;i++)
	{
		top_w=waiting;
		top_w+=i;
		int cmp=0;
		if(top_w->count_sleep==0&&top_w->TID!=0)
		{
		
				if(top_w->has_res==0)
				{
				
				
				for(int i=0;i<8;i++)
				{
					if(res[i]==1&&top_w->want_resources[i]==1)
					{
						cmp++;
					}
				}
				}
				
				if(cmp==top_w->count_resources||top_w->has_res!=0)
				{
				
			
				strcpy(top_w->task_state,"ready");
				top_w->turnaround_time+=1;
				pre_ready[pr].TID=top_w->TID;
				
				char* name_copy=malloc(sizeof(char) * (1000));
				memcpy(name_copy,top_w->task_name,strlen(top_w->task_name)+1);
				pre_ready[pr].task_name=name_copy;
				
				
				strcpy(pre_ready[pr].task_state,"ready");
				pre_ready[pr].running_time=top_w->running_time;
				pre_ready[pr].waiting_time=top_w->waiting_time;
				pre_ready[pr].turnaround_time=top_w->turnaround_time;
				pre_ready[pr].priority=top_w->priority;
				//pre_ready[pr].ctx=top_w->ctx;
				pre_ready[pr].count_sleep=top_w->count_sleep;
				pre_ready[pr].count_resources=top_w->count_resources;
				pre_ready[pr].has_res=top_w->has_res;
				
				strcpy(job[top_w->TID].task_state,"ready");
				job[top_w->TID].running_time=top_w->running_time;
				job[top_w->TID].waiting_time=top_w->waiting_time;
				job[top_w->TID].turnaround_time=top_w->turnaround_time;
				job[top_w->TID].priority=top_w->priority;
				//job[top_w->TID].ctx=top_w->ctx;
				job[top_w->TID].count_sleep=top_w->count_sleep;
				job[top_w->TID].count_resources=top_w->count_resources;
				job[top_w->TID].has_res=top_w->has_res;
				
				for(int i=0;i<8;i++)
				{
					pre_ready[pr].resources[i]=top_w->resources[i];
					job[top_w->TID].resources[i]=top_w->resources[i];
					
					pre_ready[pr].want_resources[i]=top_w->want_resources[i];
					job[top_w->TID].want_resources[i]=top_w->want_resources[i];
				}
				pr++;
				wh++;
				top_w->TID=0;
				}
				
				
		}
		top_w++;
		
	}
	
	for(int i=1;i<=w;i++)
	{
		int num=waiting[i].TID;
		if(num!=0&&strcmp(waiting[i].task_state,"waiting")==0)
		{
		 
		 top_w=waiting;
		 top_w+=i;
		 break;
		}
	}
	

}


void running_to_ready()
{
	strcpy(job[runningnumber].task_state,"ready");
	job[runningnumber].running_time+=1;
	job[runningnumber].turnaround_time+=1;
	
	
	pre_ready[pr].TID=job[runningnumber].TID;
	char* name_copy=malloc(sizeof(char) * (1000));
	memcpy(name_copy,job[runningnumber].task_name,strlen(job[runningnumber].task_name)+1);
	pre_ready[pr].task_name=name_copy;
	strcpy(pre_ready[pr].task_state,"ready");
	pre_ready[pr].running_time=job[runningnumber].running_time;
	pre_ready[pr].waiting_time=job[runningnumber].waiting_time;
	pre_ready[pr].turnaround_time=job[runningnumber].turnaround_time;
	pre_ready[pr].priority=job[runningnumber].priority;
	//pre_ready[pr].ctx=job[runningnumber].ctx;	
	pre_ready[pr].count_sleep=job[runningnumber].count_sleep;
	pre_ready[pr].count_resources=job[runningnumber].count_resources;
	pre_ready[pr].has_res=job[runningnumber].has_res;
	
	
	for(int i=0;i<8;i++)
	{
		pre_ready[pr].resources[i]=job[runningnumber].resources[i];
		pre_ready[pr].want_resources[i]=job[runningnumber].want_resources[i];
	}

	
	pr++;
	old_runningnumber=runningnumber;
	runningnumber=0;



}


void ready_to_running()
{

	
	runningnumber=top_r->TID;
	
	strcpy(top_r->task_state,"running");
	top_r->waiting_time+=1;
	top_r->turnaround_time+=1;
	
	
	job[runningnumber].TID=top_r->TID;
	char* name_copy=malloc(sizeof(char) * (1000));
	memcpy(name_copy,top_r->task_name,strlen(top_r->task_name)+1);
	job[runningnumber].task_name=name_copy;
	strcpy(job[runningnumber].task_state,"running");
	
	job[runningnumber].running_time=top_r->running_time;
	job[runningnumber].waiting_time=top_r->waiting_time;
	job[runningnumber].turnaround_time=top_r->turnaround_time;
	job[runningnumber].priority=top_r->priority;
	//job[runningnumber].ctx=top_r->ctx;
	job[runningnumber].count_sleep=top_r->count_sleep;
	//printf("%d\n",job[runningnumber].count_sleep);
	job[runningnumber].count_resources=top_r->count_resources;
	job[runningnumber].has_res=top_r->has_res;
	
	for(int i=0;i<8;i++)
	{
		job[runningnumber].resources[i]=top_r->resources[i];
		job[runningnumber].want_resources[i]=top_r->want_resources[i];
	}
	
	
	rh++;
	top_r->TID=0;
	top_r++;
	printf("Task %s is running.\n",job[runningnumber].task_name);
}

void running_to_waiting()
{
	strcpy(job[runningnumber].task_state,"waiting");
	job[runningnumber].running_time+=1;
	job[runningnumber].turnaround_time+=1;
	
	
	pre_waiting[pw].TID=job[runningnumber].TID;
	char* name_copy=malloc(sizeof(char) * (1000));
	memcpy(name_copy,job[runningnumber].task_name,strlen(job[runningnumber].task_name)+1);
	pre_waiting[pw].task_name=name_copy;
	strcpy(pre_waiting[pw].task_state,"waiting");
	pre_waiting[pw].running_time=job[runningnumber].running_time;
	pre_waiting[pw].waiting_time=job[runningnumber].waiting_time;
	pre_waiting[pw].turnaround_time=job[runningnumber].turnaround_time;
	pre_waiting[pw].priority=job[runningnumber].priority;
	//pre_waiting[pw].ctx=job[runningnumber].ctx;	
	pre_waiting[pw].count_sleep=job[runningnumber].count_sleep;
	pre_waiting[pw].count_resources=job[runningnumber].count_resources;
	pre_waiting[pw].has_res=job[runningnumber].has_res;
	
	
	for(int i=0;i<8;i++)
	{
		pre_waiting[pw].resources[i]=job[runningnumber].resources[i];
		pre_waiting[pw].want_resources[i]=job[runningnumber].want_resources[i];
	}

	
	pw++;
	old_runningnumber=runningnumber;
	runningnumber=0;
	
}



void handler()
{
	
	once=0;
	for(int i=1;i<w;i++)
	{
		waiting[i].turnaround_time+=1;
		if(waiting[i].count_sleep>0)
		{
			waiting[i].count_sleep--;
			
		}
		
	}
	
	for(int i=1;i<r;i++)
	{
		ready[i].waiting_time+=1;
		ready[i].turnaround_time+=1;
	}
	if(runningnumber)
	{
	job[runningnumber].running_time+=1;
	job[runningnumber].turnaround_time+=1;		
	}
	

	for(int i=pwh;i<pw;i++)
	{
		waiting[w].TID=pre_waiting[i].TID;
		
		char* name_copy=malloc(sizeof(char) * (1000));
		memcpy(name_copy,pre_waiting[i].task_name,strlen(pre_waiting[i].task_name)+1);
		waiting[w].task_name=name_copy;
		strcpy(waiting[w].task_state,pre_waiting[i].task_state);
		waiting[w].running_time=pre_waiting[i].running_time;
		waiting[w].waiting_time=pre_waiting[i].waiting_time;
		waiting[w].turnaround_time=pre_waiting[i].turnaround_time;
		waiting[w].priority=pre_waiting[i].priority;
		//waiting[w].ctx=pre_waiting[i].ctx;
		
		waiting[w].count_sleep=pre_waiting[i].count_sleep;
		waiting[w].count_resources=pre_waiting[i].count_resources;
		waiting[w].has_res=pre_waiting[i].has_res;
		
	
		for(int j=0;j<8;j++)
		{
			waiting[w].resources[j]=pre_waiting[i].resources[j];
			waiting[w].want_resources[j]=pre_waiting[i].want_resources[j];
		}
		w++;
	}
	pwh=pw;
	
	waiting_to_ready();
	for(int i=prh;i<pr;i++)
	{
		
		ready[r].TID=pre_ready[i].TID;
		char* name_copy=malloc(sizeof(char) * (1000));
		memcpy(name_copy,pre_ready[i].task_name,strlen(pre_ready[i].task_name)+1);
		ready[r].task_name=name_copy;
		strcpy(ready[r].task_state,pre_ready[i].task_state);
		ready[r].running_time=pre_ready[i].running_time;
		ready[r].waiting_time=pre_ready[i].waiting_time;
		ready[r].turnaround_time=pre_ready[i].turnaround_time;
		ready[r].priority=pre_ready[i].priority;
		//ready[r].ctx=pre_ready[i].ctx;	
		ready[r].count_sleep=pre_ready[i].count_sleep;
		ready[r].count_resources=pre_ready[i].count_resources;
		ready[r].has_res=pre_ready[i].has_res;
	
	
		for(int j=0;j<8;j++)
		{
			ready[r].resources[j]=pre_ready[i].resources[j];
			ready[r].want_resources[j]=pre_ready[i].want_resources[j];
		}
		
		r++;
		
	}
	prh=pr;
	
	
	qsort(top_r,r-1, sizeof(struct task), cmpfunc);

	for(int i=1;i<=r;i++)
	{
		int num=ready[i].TID;
		if(num!=0&&strcmp(job[num].task_state,"ready")==0)
		{
		 rh=i;
		 top_r=ready;
		 top_r+=i;
		 break;
		}
		else if(i==r)
		{
		 top_r=ready;
		 top_r+=i;
		}
	}
	
	
	
	
	
	count_ms++;
	//printf("count_ms=%d\n",count_ms);
	if(top_r->TID!=0||runningnumber!=0)
	{
		
		if(strcmp(alog, "FCFS") == 0)
		{
			
			if(runningnumber==0)
			{
				ready_to_running();
				//printf("runningnumber=%d\n",runningnumber);
				if(old_runningnumber!=0)
				{
					if(old_runningnumber!=runningnumber)
					{
					swapcontext(&job[old_runningnumber].ctx,&job[runningnumber].ctx);
					}
				}
				else
				{
								
				swapcontext(&ctx_sche,&job[runningnumber].ctx);
				}
			}else if((runningnumber!=0&&strcmp(job[runningnumber].task_state,"TERMINATED")==0))
			{
				if(top_r->TID!=0)
				{
				   ready_to_running();
				   //printf("runningnumber=%d\n",runningnumber);
					if(old_runningnumber!=0)
				{
					if(old_runningnumber!=runningnumber)
					{
					swapcontext(&job[old_runningnumber].ctx,&job[runningnumber].ctx);
					}
				}
				else
				{
								
				swapcontext(&ctx_sche,&job[runningnumber].ctx);
				}
				   
				}
				else
				{
					printf("CPU idle.\n");
				}
				
			}			
		
		}
		else if(strcmp(alog, "RR") == 0)
		{
			if(runningnumber==0)
			{
				if(top_r->TID!=0)
				{
				   count_RR=0;
				   ready_to_running();
				   //printf("runningnumber=%d\n",runningnumber);
					if(old_runningnumber!=0)
				{
					if(old_runningnumber!=runningnumber)
					{
					swapcontext(&job[old_runningnumber].ctx,&job[runningnumber].ctx);
					}
				}
				else
				{
								
				swapcontext(&ctx_sche,&job[runningnumber].ctx);
				}
				   
				}
				else
				{
					printf("CPU idle.\n");
				}
					
			}
			else if((runningnumber!=0&&strcmp(job[runningnumber].task_state,"TERMINATED")==0)||(runningnumber!=0&&count_RR==3))
			{
			running_to_ready();
				if(top_r->TID!=0)
				{
				   count_RR=0;
				   ready_to_running();
				   //printf("runningnumber=%d\n",runningnumber);
				  if(old_runningnumber!=0)
				{
					if(old_runningnumber!=runningnumber)
					{
					swapcontext(&job[old_runningnumber].ctx,&job[runningnumber].ctx);
					}
				}
				else
				{
								
				swapcontext(&ctx_sche,&job[runningnumber].ctx);
				}
				}
				else
				{
					printf("CPU idle.\n");
				}
			}
			else
			{
				count_RR++;
			}
		
		
		}
		else if(strcmp(alog, "PP") == 0)
		{
			
			if(runningnumber==0)
			{
				if(top_r->TID!=0)
				{
					ready_to_running();
					//printf("runningnumber=%d\n",runningnumber);
				  if(old_runningnumber!=0)
				{
					if(old_runningnumber!=runningnumber)
					{
					swapcontext(&job[old_runningnumber].ctx,&job[runningnumber].ctx);
					}
				}
				else
				{
								
				swapcontext(&ctx_sche,&job[runningnumber].ctx);
				}
					
				}
				else
				{
					printf("CPU idle.\n");
				}
				
			}
			else if((runningnumber!=0&&strcmp(job[runningnumber].task_state,"TERMINATED")==0)&&top_r->priority<job[runningnumber].priority)
			{
				running_to_ready();
				ready_to_running();
				//printf("runningnumber=%d\n",runningnumber);
				   if(old_runningnumber!=0)
				{
					if(old_runningnumber!=runningnumber)
					{
					swapcontext(&job[old_runningnumber].ctx,&job[runningnumber].ctx);
					}
				}
				else
				{
								
				swapcontext(&ctx_sche,&job[runningnumber].ctx);
				}
			
			}
		
		}
		
	}
	else
	{
		
		printf("CPU idle.\n");
	}
	
		
		
		if(runningnumber==0&&top_r->TID==0&&top_w->TID==0&&pre_waiting[pwh].TID==0&&pre_ready[prh].TID==0)
		{
			printf("Simualtion over.\n");
			swapcontext(&job[old_runningnumber].ctx,&ctx_main);
		}
			
	
	
	
	
	
}

void sche_exec()
{
    value_new.it_value.tv_sec = 0;  
    	value_new.it_value.tv_usec = 10000;
    	value_new.it_interval.tv_sec = 0;
    	value_new.it_interval.tv_usec = 10000;
    	setitimer(ITIMER_VIRTUAL, &value_new, NULL);
    signal(SIGVTALRM,handler);
    while(1);

}







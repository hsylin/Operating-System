#include "../include/task.h"
#include "../include/command.h"
#include "../include/scheduler.h"
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
void task_sleep(int ms)
{
 
    printf("Task %s goes to sleep.\n",job[runningnumber].task_name);
    job[runningnumber].count_sleep=ms;
    int store=runningnumber;
   
    running_to_waiting();
    while(1)
    {
    	if(job[store].count_sleep==0)
    	{
    		//if(job[store].count_resources==0)
    		break;
    		/*else
    		{
    		int want_resources[8]={0};
    		int j=0;
    		for(int i=0;i<8;i++)
    		{
    			if(job[store].want_resources[i]==1)
    			{
    				want_resources[j]=i;
    				j++;
    			}
    		}
    		get_resources(j,want_resources);
    		if(job[store].count_resources==0)
    		break;
    		else
    		running_to_waiting();
    		}*/
    		
    	}
    }
}

void task_exit()
{
    printf("Task %s has terminated.\n",job[runningnumber].task_name);
    strcpy(job[runningnumber].task_state,"TERMINATED");
    old_runningnumber=runningnumber;
    runningnumber=0;
}

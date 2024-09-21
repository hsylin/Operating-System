#include <sys/wait.h>
#include <sys/types.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <dirent.h>
#include <ctype.h>

#ifndef READ
#define READ 0
#endif

#ifndef WRITE
#define WRITE 1
#endif
#define LSH_TOK_BUFSIZE 64
#define LSH_TOK_DELIM " \t\r\n\a"


void recordhistory(char *line);
int record(char **args,char* line);
char* replay(char **args,char* line);
int lsh_cd(char **args,char* line);
int lsh_help(char **args,char* line);
int lsh_exit(char **args,char* line);
int echo(char **args,char* line);
int mypid(char**args,char* line);


char *lsh_read_line(void);
char **lsh_split_line(char *line);
int lsh_execute(char **args,char* line);
int pipe_or_redirect(char** args);
int lsh_pipexecute(int inputRedirectFlag,int outputRedirectFlag,int backgroundFlag,int status,int pipeFlag,char** args,char* line,int buildinFlag);
int background(int inputRedirectFlag,int outputRedirectFlag,int backgroundFlag,int status,int pipeFlag,char** args,char* line,int buildinFlag);





char* history[17]={0};
int historycount=1;

int inputRedirectFlag, outputRedirectFlag,backgroundFlag,pipeFlag,buildinFlag;
int status =1;
int newpipe[2];
int argLocation[100] = { 0 };
int count, pipes;
char *in_path, *out_path;
int Flag;


char *builtin_str[] = 
{
  "cd",
  "help",
  "exit",
  "record",
  "echo",
  "mypid"
};

int (*builtin_func[]) (char **,char *) = 
{
  &lsh_cd,
  &lsh_help,
  &lsh_exit,
  &record,
  &echo,
  &mypid
};



int lsh_cd(char **args,char *line)
{
  if (args[1] == NULL) 
  {
    fprintf(stderr, "lsh: expected argument to \"cd\"\n");
  } else 
  {
    if ((chdir(args[1])) != 0) 
    {
      perror("lsh");
    }
  }
  return 1;
}


int lsh_help(char **args,char *line)
{
  printf("The following are build in:\n");
  printf("1: help :   show all build in function info\n");
  printf("2: cd :     change diretory\n");
  printf("3: echo :   echo the stirngs to standard output\n");
  printf("4: record : show last-16 cmds you typed in \n");
  printf("5: replay : re-execute the cmd showed in record\n");
  printf("6: mypid :  find and print process-ids\n");
  printf("7: exit :   exit shell\n");
  printf("Use the man command for information on other programs.\n");
  return 1;
}


int lsh_exit(char **args,char *line)
{
  printf("bye bye\n");
  return 0;
}

int record(char **args,char *line)
{
    printf("history cmd:\n");
    for(int i=1;i<=16;i++)
    {
        printf("%d :%s\n",i,history[i]);
    }
    return 1;
}
int echo(char **args,char *line)
{

    if(!strcmp(args[1],"-n"))
    {
    	    for(int i=8;line[i]!='\t'&&line[i]!='\r'&&line[i]!='\n'&&line[i]!='\a'&&line[i]!='|'&&line[i]!='>'&&line[i]!='<'&&line[i]!='&'&&line[i]!=0;i++)
	    {
	    	printf("%c",line[i]);
	    }
	}	       
    else
    {
    	for(int i=5;line[i]!='\t'&&line[i]!='\r'&&line[i]!='\n'&&line[i]!='\a'&&line[i]!='|'&&line[i]!='>'&&line[i]!='<'&&line[i]!='&'&&line[i]!=0;i++)
	    {
	    	printf("%c",line[i]);
	    }
	    printf("\n");
	    
    }    
    return 1;
}
char* replay(char **args,char* line)
{
    
    int num;
    num=atoi(args[1]);
    if(num==0||historycount<=num)
    {
    	printf("replay: wrong args\n");
    	return NULL ;
    }

    char* copy=malloc(sizeof(char) * (40*strlen(history[num])));
    char* paste=malloc(sizeof(char) * (20*strlen(history[num])));
    
    memcpy(copy,history[num],strlen(history[num])+1);
    if(num>9)
    {
    	memcpy(paste,line+9,strlen(line)+1);
    }
    else
    {
    	memcpy(paste,line+8,strlen(line)+1);
    }
    strcat(copy,paste);
    
   
    return copy;
}

int mypid(char**args,char* line)
{
    
    if(strcmp(args[1],"-i")==0)
    {
     printf("%d\n", getpid());
     return 1;
    }

    if(strcmp(args[1],"-p")==0)
    {
        int inputpid;
        sscanf(args[2], "%d", &inputpid);
        int unused1;
        char unused2[1000];
        char unused3;
        int ppid;
        char filename[50];
        sprintf(filename, "/proc/%d/stat", inputpid);
        FILE *f = fopen(filename, "r");
        if(f==NULL)
        {
            perror("Error: ");
            return 1;
        }
        fscanf(f, "%d %s %c %d", &unused1, unused2, &unused3, &ppid);
        printf("%d\n", ppid);
        fclose(f);
    }
    
    if(strcmp(args[1],"-c")==0)
    {
        int haveinputpid=0;
        int inputpid;
        sscanf(args[2], "%d", &inputpid);
        struct dirent   *dirent;
        DIR             *dir;
        int             r;
        int             pid;//process pid
        int processpid;
        char unused2[1000];
        char unused3;
        int ppid;

        // 1) find all processes
        if (!(dir = opendir ("/proc"))) 
        {
            perror("Error: ");
            return 1;
        }
        //readdir()返回引數dir 目錄流的下個目錄進入點。
        while (dirent = readdir (dir)) 
        {
            // 2) we are only interested in process IDs
            if (isdigit (*dirent -> d_name)) 
            {
                pid = atoi (dirent -> d_name);
                char filename[50];
                sprintf(filename, "/proc/%d/stat",pid);
                FILE *f = fopen(filename, "r"); 
               fscanf(f, "%d %s %c %d", &processpid, unused2, &unused3, &ppid);
                if(ppid==inputpid)
                {
                    printf("%d\n", pid);
                }
                if(processpid==inputpid)
                {
                    haveinputpid=1;
                }
                
                fclose(f);
            }
            
        }
        closedir (dir);
        if(haveinputpid==0)
        {
           perror("Error: ");
            return 1;
        }
    }
    return 1;
}


int lsh_launch(char **args,char* line)
{
  pid_t pid;


  pid = fork();
  
  if (pid == 0) {
    // Child process
    if(!strcmp(args[0],"help"))
    {
    	lsh_help(args,line);
    	exit(EXIT_SUCCESS);
    }
    else if(!strcmp(args[0],"cd"))
    {
    	lsh_cd(args,line);
    	exit(EXIT_SUCCESS);
    }
    else if(!strcmp(args[0],"echo"))
    {
    	echo(args,line);
    	exit(EXIT_SUCCESS);
    }
    else if(!strcmp(args[0],"exit"))
    {
    	lsh_exit(args,line);
    	exit(EXIT_SUCCESS);
    }
    else if(!strcmp(args[0],"record"))
    {
    	record(args,line);
    	exit(EXIT_SUCCESS);
    }
    else if(!strcmp(args[0],"mypid"))
    {
    	mypid(args,line);
    	exit(EXIT_SUCCESS);
    }
    else if(execvp(args[0], args) == -1) 
    {
      perror("lsh");
      exit(EXIT_FAILURE);
    }
    
  } else if (pid < 0) {
    // Error forking
    perror("lsh");
  } else {
    // Parent process
    if (backgroundFlag==0)
    {
	     do {
	      waitpid(pid, &status, WUNTRACED);
	    } while (!WIFEXITED(status) && !WIFSIGNALED(status));
    }
    else
    {
    	if(pipeFlag==0)
    	printf("[Pid] : %d\n", pid);
    }

  }

  return 1;
}


int lsh_execute(char **args,char* line)
{
  int i;

  if (args[0] == NULL) {
    // An empty command was entered.
    return 1;
  }



  for (i = 0; i < 6; i++) 
  {
    if (strcmp(args[0], builtin_str[i]) == 0) 
    {
      return (*builtin_func[i])(args,line);
      
    }
  }
  return lsh_launch(args,line);
}

char *lsh_read_line(void)
{
  char *line = NULL;
  ssize_t bufsize = 0; // have getline allocate a buffer for us
  if (getline(&line, &bufsize, stdin) == -1) 
  {
    if (feof(stdin)) 
    {
      exit(EXIT_SUCCESS);  // We received an EOF
    } else  
    {
      perror("lsh: getline\n");
      exit(EXIT_FAILURE);
    }
  }
 if(strlen(line)>1)
 {
  line=strtok(line,"\n");
 }
  return line;
}



char **lsh_split_line(char *line)
{
  int bufsize = LSH_TOK_BUFSIZE, position = 0;
  char **tokens = malloc(bufsize * sizeof(char*));
  
  char *token=malloc(strlen(line) * sizeof(char*));
  memcpy(token,line,strlen(line)+1);
  char *cline=malloc(strlen(line) * sizeof(char*));
  memcpy(cline,line,strlen(line)+1);

  if (!tokens) 
  {
    fprintf(stderr, "lsh: allocation error\n");
    exit(EXIT_FAILURE);
  }

  token = strtok(cline, LSH_TOK_DELIM);
  while (token != NULL) 
  {
    tokens[position] = token;
    position++;

    if (position >= bufsize) 
    {
      bufsize += LSH_TOK_BUFSIZE;
      tokens = realloc(tokens, bufsize * sizeof(char*));
      if (!tokens) 
	 {
        fprintf(stderr, "lsh: allocation error\n");
        exit(EXIT_FAILURE);
      }
    }
    token = strtok(NULL, LSH_TOK_DELIM);
  }
  tokens[position] = NULL;
  return tokens;
}

int pipe_or_redirect(char** args)
{
        count = pipes = 0;
        pipeFlag = backgroundFlag = inputRedirectFlag = outputRedirectFlag =buildinFlag= 0;

        while (args[count] != 0) 
	   {
            if (strcmp(args[count], "|") == 0) 
		  {
                args[count] = 0;
                argLocation[pipes + 1] = count + 1;
                ++pipes;
                pipeFlag = 1;
            }
            else if (strcmp(args[count], "<") == 0) 
		  {
                in_path = strdup(args[count + 1]);
                args[count] = 0;
                inputRedirectFlag ++;
            }
            else if (strcmp(args[count], ">") == 0)
		  {
                out_path = strdup(args[count + 1]);
                args[count] = 0;
                outputRedirectFlag ++;
            }
            else if (strcmp(args[count], "&") == 0) 
		  {
                args[count] = 0;
                backgroundFlag++;
            }
            else if(!(strcmp(args[0],"cd")&&strcmp(args[0],"echo")&&strcmp(args[0],"help")&&strcmp(args[0],"exit")&&strcmp(args[0],"record")&&strcmp(args[0],"mypid"))) 
            {
        	buildinFlag=1;
	    }
            else 
		  {
                argLocation[count] = count;
            }
            
            ++count;
        }
       
        if(pipeFlag||inputRedirectFlag||outputRedirectFlag)
        {
        	return 1;
	   }
	return 0;
}
int background(int inputRedirectFlag,int outputRedirectFlag,int backgroundFlag,int status,int pipeFlag,char** args,char* line,int buildinFlag)
{
    pid_t pid=0;
    pid_t grandpid;
    pipe(newpipe);
    int savei=dup(READ);
    pid=fork();

    
    if(pid==0)
    {
        pid=-10;//fake pid for process be fork
 
        lsh_pipexecute(inputRedirectFlag,outputRedirectFlag,backgroundFlag,status,pipeFlag,args,line,buildinFlag);
	
    }
    else
    {
        printf("[Pid] : %d\n", pid);
    }
   
    return 1;
}
int lsh_pipexecute(int inputRedirectFlag,int outputRedirectFlag,int backgroundFlag,int status,int pipeFlag,char** args,char* line,int buildinFlag)
{
int l_pipe[2], r_pipe[2];
pid_t pid;
int saveo=dup(WRITE);

if((pipes!=0)||(buildinFlag==0))	
{
        for (int index=0; index <=pipes; ++index) 
        {
            if (pipes > 0 && index != pipes) 
            {
                pipe(r_pipe); 
            }
            switch (pid = fork()) 
            {
                case -1: perror("fork failed"); /* fork() error */
                         break;

                case 0:     
                 
                  if ((index == 0) && (inputRedirectFlag == 1)) 
			   {
	                           int inputFileDescriptor = open(in_path, O_RDONLY , 0400);
	                           if (inputFileDescriptor == -1) 
						  {
	                               perror("input file failed to open\n");
	                               return(EXIT_FAILURE);
	                           }
	                           dup2(inputFileDescriptor,READ);
	                           close(inputFileDescriptor);
	           } /* end of input redirection management */
	
	          if ((index == pipes) && (outputRedirectFlag == 1)) 
			{
	                           //printf("DEBUG: here we should be about to create our output file\n");
	                           int outputFileDescriptor = creat(out_path, 0700);
	                           if (outputFileDescriptor < 0) 
						  {
	                               perror("output file failed to open\n");
	                               return(EXIT_FAILURE);
	                           }
	                           dup2(outputFileDescriptor,WRITE);
	                           close(outputFileDescriptor);
	           } /* end of output redirection management */
                       
                        if (pipes > 0) 
				    {
                       		    if (index == 0){ /* first child process */
	                                
	                                dup2(r_pipe[WRITE],WRITE);
	                                close(r_pipe[WRITE]);
	                                close(r_pipe[READ]);
	                            }
	                            else if (index < pipes) 
						   { /* in-between child process */
	                                
	                                dup2(l_pipe[READ],READ);
	                                close(l_pipe[READ]);
	                                close(l_pipe[WRITE]);
	                                dup2(r_pipe[WRITE],WRITE);
	                                close(r_pipe[READ]);
	                                close(r_pipe[WRITE]);
	                            }
	                            else 
						   { /* final child process */
	                                dup2(l_pipe[READ],READ);
	                                close(l_pipe[READ]);
	                                close(l_pipe[WRITE]);
	                            }
	                        }
	                        
	                         
	                       /* execute command */
	                       if(index!=0||(buildinFlag==0))
	                       {
	                               execvp(args[argLocation[index]], &args[argLocation[index]]);
			               /* if execvp() fails */
			               perror("execution of command failed\n");
			               
	                       }
			       else
			       {
			       	exit(EXIT_SUCCESS);
			       } 

	                       break;
	                        default: /* parent process manages the pipes for child process(es) */
	                        
	                        if(index==0&&(buildinFlag==1))
	                        {
	                        	if(pipes)
	                        	{
	                        		dup2(r_pipe[WRITE],WRITE);
					        lsh_execute(args,line);
					        dup2(saveo,WRITE); 
	                        	}
	                        }
	                        if (index > 0) {
	                            close(l_pipe[READ]);
	                            close(l_pipe[WRITE]);
	                        }
	                        l_pipe[READ] = r_pipe[READ];
	                        l_pipe[WRITE] = r_pipe[WRITE];
	                        /* parent waits for child process to complete */
	                        do {
	      waitpid(pid, &status, WUNTRACED);
	    } while (!WIFEXITED(status) && !WIFSIGNALED(status));                     
	                        break;
            } /* end of switch-statement for command execution */
    }   

}
else//output redirection
{
        int outputFileDescriptor = creat(out_path, 0700);
	    if (outputFileDescriptor < 0) 
	    {
	       perror("output file failed to open\n");
	       return(1);
	    }
	   close(WRITE);
	   dup(outputFileDescriptor);
	   close(outputFileDescriptor);
	   lsh_execute(args,line);
       dup2(saveo,WRITE);  

}
if(backgroundFlag == 0)
return 1;
else
exit(EXIT_SUCCESS);//\A8\BE\A4\EEchild-->zombie 

}


void lsh_loop(void)
{

  char *line;
  char **args;
  const char *replayy="replay ";
  do {
  
    printf(">>> $ ");
    
    line = lsh_read_line();
    args = lsh_split_line(line);
    Flag=pipe_or_redirect(args);

    if(strncmp(line,replayy,7)&&strcmp(line,"\n"))
    {
       recordhistory(line);
    }
    else if(!strncmp(line,replayy,7))
    {
    	line = replay(args,line);
    	if(line!=NULL)
	{
		
    		recordhistory(line);
    		args = lsh_split_line(line);
    		Flag=pipe_or_redirect(args);
    	}
    	else
    	{
    		continue;
    	}
	}	
    
    if(backgroundFlag>1||inputRedirectFlag>1||outputRedirectFlag>1)
    {
    	printf("error\n");
    }
    else
    {
	    if(Flag)
	    { 	    
	    	    if(backgroundFlag)
	    	    {
	    	    	status = background(inputRedirectFlag,outputRedirectFlag,backgroundFlag,status,pipeFlag,args,line,buildinFlag);
	   	    }
	   	    else
	   	    {
	   	    	status = lsh_pipexecute(inputRedirectFlag,outputRedirectFlag,backgroundFlag,status,pipeFlag,args,line,buildinFlag);
	   	    }    
	    	    
	    }
	    else    
	    {
		    
		  if(backgroundFlag)
	          {
	          	status = lsh_launch(args,line);
	          }
	          else
	          {
	          	status = lsh_execute(args,line);
		  }
	    }
	    
    }
	    
    
    for (int i = 0; i < 100; ++i) 
    {
        argLocation[i] = 0;
    }
    
    free(line);
    free(args);
	    
    
  } while (status);
}

int main(int argc, char **argv)
{

  lsh_loop();
  
  return EXIT_SUCCESS;
}


void recordhistory(char *line)
{
 	char* copy=malloc(sizeof(char) * (strlen(line)+2));
 	memcpy(copy,line,strlen(line)+1);
 	    if(historycount>16)
 	    {
 	     for(int i=1;i<=15;i++)
 	     {
 	         history[i]=history[i+1];
 	     }
 	       history[16]=copy;
 	    }
 	    else
 	    {
 	    	
 	       history[historycount]=copy;
 	       historycount++;
 	    }   
 }




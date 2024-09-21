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
#include <pthread.h>
#include <sys/syscall.h>
#include <time.h>
#define LSH_TOK_DELIM " \t\r\n\a"

long long int  m1row,m1col, m2row,m2col;
long long int  m1[5000][5000]={0};
long long int  m2[5000][5000]={0};
long long int  m3[5000][5000]={0};
long long int  totalthrnum;
pthread_mutex_t mutex1 = PTHREAD_MUTEX_INITIALIZER;
char* infile2;
char* infile1;
pthread_t* t;
long long int max(long long int a, long long int b) {
  if (a>b)
    return a;
  else
    return b;
}

long long int *tnum;
void r(char *s)
{
    long long int  i = 0,j = strlen(s)-1;
    while(i<j)
    {
        long long int  c = s[i];
        s[i] = s[j];
        s[j] = c;
        i++;
        j--;
    }
}
void itoa(long long int  n,char *s)
{

    long long int  i = 0;
    while(n != 0)
    {
        s[i++] = n%10+'0';
        n = n/10;
    }
    
    s[i] = '\0';
    r(s);
}

long long int m3size;
long long int throp;
long long int restop=0;
long long int max3;
void * matrixmul(void * thrnum)
{

    long long int threadnumber= *(long long int *)thrnum;
    
		
    for(long long int count=threadnumber;count<m3size;count+=totalthrnum)
    {
        for(long long int k=0; k<max3; k++)
        {
        
        //printf("m1[%lld][%lld]=%lld * m2[%lld][%lld]=%lld\n",count/max3,k,m1[count/max3][k],k,count%max3,m2[k][count%max3]);
	 m3[count/max3][count%max3]+= m1[count/max3][k] * m2[k][count%max3];
	//printf("m3[%lld][%lld]=%lld\n",count/max3,count%max3,m3[count/max3][count%max3]);
    	}
    }
    
   pthread_mutex_lock( &mutex1 );
   
     tnum[threadnumber]=syscall(SYS_gettid);
     	int fd = open("/proc/thread_info",  O_RDWR);
     	char string[40]={0};
     	itoa(tnum[threadnumber],string);
     	write(fd,string,strlen(string));
     	close(fd);
     pthread_mutex_unlock( &mutex1 );
     
     return NULL;
}



	
int main(int argc,char** argv) 
{
	
	//read m1.txt,m2.txt
	 totalthrnum = atoi(argv[1]);
	 tnum= malloc(totalthrnum*(sizeof(long long int ))); 
	 FILE *fp;
	 t= malloc(totalthrnum*(sizeof(pthread_t))); 
	 
	fp= fopen(argv[2],"r" );
	
		if(NULL == fp) 
		{
	        printf("m1 cannot open\n") ;
	        return 0;
	    }

	    long long int num;
	    fscanf(fp, "%lld", &num);
            m1row=num;
            fscanf(fp, "%lld", &num);
	    m1col=num;
	    
	long long int count;
	    
	for(long long int i=0;i<m1row;i++)
	{
		for(long long int j=0;j<m1col;j++)
		{
			fscanf(fp, "%lld", &num);
			m1[i][j]=num;	
			
		}
	}
		fclose(fp);
		
	fp= fopen(argv[3],"r" );
		if(NULL == fp) 
		{
	        printf("m2 cannot open\n") ;
	        return 0;
	    }
	    
	    count=0;
	    fscanf(fp, "%lld", &num);
            m2row=num;
            fscanf(fp, "%lld", &num);
	    m2col=num;
	    
	    
        for(long long int i=0;i<m2row;i++)
	{
		for(long long int j=0;j<m2col;j++)
		{
			fscanf(fp, "%lld", &num);
			m2[i][j]=num;	
			
		}
	}
		fclose(fp);	
		long long int max1=max(m1col,m1row);
		long long int max2=max(m2col,m2row);
		max3=max(max1,max2);
		
		
		m3size=max3*max3;
		throp=m3size/totalthrnum;
		if(totalthrnum<=m3size)
		{
			restop=m3size%totalthrnum;
		}
			
		time_t creatime; 
		time_t endtime; 

		long long int* args= malloc(totalthrnum*(sizeof(long long int))); 
		
		creatime = time(NULL);
		for(long long int i=0;i<totalthrnum;i++)
		{
			args[i]=i;
			tnum[i]=0;
		     if(pthread_create(&t[i], NULL,matrixmul,&args[i])!=0)
		     {
		     	printf("Error: pthread_create\n");
		     	
		     }
		}
		for(int i=0;i<totalthrnum;i++)
		{
			pthread_join(t[i],NULL);
		}
		endtime=time(NULL);

	FILE *fptr;
    	fptr = fopen("result.txt","w");
        if(NULL == fptr) 
		{
	        printf("output cannot open\n") ;
	        return 0;
	    }
        fprintf(fptr,"%lld %lld\n",m1row,m2col);
        for(long long int i=0;i<m1row;i++)
        {
        	for(long long int j=0;j<m2col;j++)
        	{
        		fprintf(fptr,"%lld ",m3[i][j]);
        	}
        	fprintf(fptr,"\n");
        }
    	fclose(fptr);
    	
	long long int n=syscall(SYS_gettid);
	printf("PID:%lld\n",n);
	
	long long int buf[1000]={0}; 
	long long int fd = open("/proc/thread_info",  O_RDWR);
	read(fd,buf,1000);
	long long int threadtime=0;
	for(long long int i=0;i<totalthrnum*3;i+=3)
	{
		printf("\tThreadID:");	
		printf("%lld",buf[i]);
		printf(" Time:");
		printf("%lld",buf[i+1]);
		printf("(ms) context switch times:");
		printf("%lld",buf[i+2]);
		printf("\n");
		
	}

	
	close(fd);	


	
	return 0;	
}

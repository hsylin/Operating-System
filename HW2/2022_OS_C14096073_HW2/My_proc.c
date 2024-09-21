#include <linux/module.h>
#include <linux/moduleparam.h>
#include <linux/init.h>
#include <linux/kernel.h>   
#include <linux/proc_fs.h>
#include <asm/uaccess.h>
#include <linux/uaccess.h>
#include<linux/vmalloc.h>
#define PROCFS_MAX_SIZE 1000
#define BUFSIZE  100
MODULE_LICENSE("Dual BSD/GPL");
MODULE_AUTHOR("Liran B.H");
static long long int procfs_buffer[PROCFS_MAX_SIZE];
static unsigned long procfs_buffer_size = 0;

static long long int atoi(char *s)
{
    long long int sum = 0;
    int i=0;
    while(s[i]!=0)
    {
        sum = sum*10+s[i]-'0';
        i++;
    }
    return sum;
}



static struct proc_dir_entry *ent;

static ssize_t mywrite(struct file *file, const char __user *ubuf,size_t count, loff_t *ppos) 
{
	char* buf1;
	if (count > 1024 ) {
	    buf1=(char *)vmalloc(1024); 
	}
	else
	{
	    buf1=(char *)vmalloc(count);  
	}
	if ( copy_from_user(buf1, ubuf, count) ) {
		return -EFAULT;
	}
	long long int threadid;
	
	procfs_buffer[procfs_buffer_size]=atoi(buf1);
	threadid=procfs_buffer[procfs_buffer_size];

	procfs_buffer_size++;
	ppos++;
	
struct task_struct *the_process;
struct task_struct *the_thread;

for_each_process(the_process) {

            for_each_thread(the_process, the_thread) {
               long long int threadnum=the_thread->pid;
    		if(threadnum==threadid)
    		{
    		long long int threadutime=the_thread->utime;
	       long long int threadcontextswitch =the_thread->nvcsw+the_thread->nivcsw;
	      
	       threadutime/=1000000;
	       procfs_buffer[procfs_buffer_size]=threadutime;
	       

	       procfs_buffer_size++;
		ppos++;
		
	       procfs_buffer[procfs_buffer_size]=threadcontextswitch;
	     
	       procfs_buffer_size++;
		ppos++;
		
		}

    		
            }
        
}

	return 1;
}

static ssize_t myread(struct file *file, char __user *ubuf,size_t count, loff_t *ppos) 
{
	ppos=0;
	 copy_to_user(ubuf,procfs_buffer,1000);
	procfs_buffer_size = 0;
	
	return 1;
}

static struct proc_ops myops = 
{
	//.owner = THIS_MODULE,
	.proc_read = myread,
	.proc_write = mywrite,
};

static int simple_init(void)
{
	ent=proc_create("thread_info",0660,NULL,&myops);
	return 0;
}

static void simple_cleanup(void)
{
	proc_remove(ent);
	printk(KERN_WARNING "bye ...\n");
}

module_init(simple_init);
module_exit(simple_cleanup);

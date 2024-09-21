#ifndef TASK_H
#define TASK_H
#include <time.h>
#include <ucontext.h>
#include <signal.h>
#include <sys/time.h>
void task_sleep(int);
void task_exit();

#endif

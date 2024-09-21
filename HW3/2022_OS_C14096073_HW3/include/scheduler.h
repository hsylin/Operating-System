#ifndef SCHEDULER_H
#define SCHEDULER_H
#include <time.h>
#include <stdbool.h>
#include <ucontext.h>
#include <signal.h>
#include <sys/time.h>
#include <stdio.h>

int cmpfunc (const void * a, const void * b);
void waiting_to_ready();
void running_to_ready();
void ready_to_running();
void running_to_waiting();
void handler();
void sche_exec();

#endif

#ifndef RESOURCE_H
#define RESOURCE_H
#include <time.h>
#include <ucontext.h>
#include <signal.h>
#include <sys/time.h>
void get_resources(int, int *);
void release_resources(int, int *);

#endif

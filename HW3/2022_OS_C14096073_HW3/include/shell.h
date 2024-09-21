#ifndef SHELL_H
#define SHELL_H
#include <time.h>
#include <ucontext.h>
#include <signal.h>
#include "command.h"
#include <sys/time.h>

int execute(struct pipes *);
int spawn_proc(int, int, struct cmd *, struct pipes *);
int fork_pipes(struct cmd *);
void shell();

#endif

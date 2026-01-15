#ifndef SCHEDULER_H
#define SCHEDULER_H

#include "core.h"
#include "thread.h"

// Her algoritma bir core ID döner
int assignRR(Core *cores, int k, Thread t, int *rrIndex);
int assignLeastLoaded(Core *cores, int k, Thread t);
int assignPriority(Core *cores, int k, Thread t);
int assignAffinity(Core *cores, int k, Thread t, int *lastCore);
void balanceWorkStealing(Core *cores, int k);

#endif

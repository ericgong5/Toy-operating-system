#ifndef kernel_H
#define kernel_H

#include "pcb.h"
#include <stdbool.h>

void ready_queue_initialize();

int get_scheduling_policy_number(char* policy);

int myinit(const char *filename);
int scheduler();
PCB ready_queue_pop(int index, bool inPlace);
void ready_queue_add_to_end(PCB *pPCB);


#endif

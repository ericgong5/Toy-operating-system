#include "pcb.h"

#include <stdint.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>

//In this implementation, Pid is the same as file ID 
PCB* makePCB(int start, int end, char* pid){

    PCB * newPCB = malloc(sizeof(PCB));

    for (int i = 0; i<10; i++){
    
    newPCB->pageTable[i] = -1; // initialize pageTable entries to -1 for empty
    
    }

    newPCB->pid = pid;
    newPCB->PC = start;
    newPCB->start  = start;
    newPCB->end = end;
    newPCB->job_length_score = 1+end-start;

    newPCB->pageTableIndex = 0;
    newPCB->programCounter = 0;

    return newPCB;
    
}
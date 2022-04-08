#include "pcb.h"

#include <stdint.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>

//In this implementation, Pid is the same as file ID 
PCB* makePCB(int start, int end, char* pid){
    PCB * newPCB = malloc(sizeof(PCB));
    for (int i = 0; i<100; i++){
        newPCB->pageTable[i] = -1; // initialize pageTable entries to -1 for empty
    }
    newPCB->pid = pid;
    newPCB->PC = start;
    newPCB->start  = start;
    newPCB->end = end;
    //newPCB->job_length_score = 1+end-start; <- not used in A3

    newPCB->pageTableIndex = 0;
    newPCB->frameIndex = 0;

    return newPCB;
}



int find_empty_page_table(PCB* pcb){
    for(int i = 0; i < 100; i++){
        if((pcb->pageTable[i]) == -1){
            return i;
        }
    }
    return -1;
}




/*
int main(int argc, char *argv[]) {
    PCB* newPCB = makePCB(0,10,"hi");
    newPCB->pageTable[0] = 6;
    int a = find_empty_page_table(newPCB);
    printf("%d\n",a);
}
*/

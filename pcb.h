#ifndef PCB_H
#define PCB_H

/*
 * Struct:  PCB 
 * --------------------
 * pid: process(task) id
 * PC: 
 * start: the first line in shell memory that belongs to this task
 * end: the last line in shell memory that belongs to this task
 * job_length_score: for EXEC AGING use only, stores the job length score
 * 
 * pageTable: an array that stores of each frame number in the frame store
 * pageTableIndex: tracks which line inside the frame the pcb is executing
 * programCounter: tracks which lines of code that we're executing in a specific frame
 */
typedef struct
{
    char* pid;
    int PC;
    int start; // not contiguous so not needed
    int end; // not contiguous so not needed
    int job_length_score;

    int pageTable[12]; // page table array
    int pageTableIndex;
    int programCounter;
}PCB;

PCB * makePCB(int start, int end, char* pid);
#endif



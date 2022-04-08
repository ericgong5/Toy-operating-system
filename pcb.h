#ifndef PCB_H
#define PCB_H

/*
 * Struct:  PCB 
 * --------------------
 * pid: process(task) id
 * PC: program counter, stores the line that the task is executing
 * start: the first line in shell memory that belongs to this task
 * end: the last line in shell memory that belongs to this task
 * job_length_score: for EXEC AGING use only, stores the job length score
 *
 * pageTable: an array that stores of each frame number in the frame store
 * pageTableIndex: tracks which which element of the page table we're at
 * frameIndex: tracks which lines of code that we're executing in a specific frame
 */
typedef struct
{
    char* pid;
    int PC;  // starts at
    int start; // not too usefull now
    int end; // meta data for knowing when to end process
    //int job_length_score;  <- not used in A3

    int pageTable[100]; // page table array
    int pageTableIndex;
    int frameIndex;
}PCB;

PCB * makePCB(int start, int end, char* pid);
#endif


int find_empty_page_table(PCB* pcb);


#include "cpu.h"
#include "pcb.h"
#include "interpreter.h"
#include "shell.h"
#include "shellmemory.h"
#include "harddisk.h"


#include <stdint.h>
#include <stdbool.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

/*
 * Struct:  CPU 
 * --------------------
 * IP: serve as a pointer to shell memory. Ex. IP = 101 means CPU is executing the 101th line in shellmemory
 ^changed to pointer into framestore for current frame
 * IR: stores the line of code that CPU is executing
 * quanta: how many lines of code could the current task run until it finishes or being switched to another task
 */
struct CPU
{
    int IP;
    char IR[1000];
    int quanta;
};
struct CPU aCPU = {.quanta =0};

int cpu_get_ip(){
    return aCPU.IP;
}

void cpu_set_ip(int pIP){
    aCPU.IP = pIP;
}

void cpu_empty(){
    aCPU.quanta = 2;
}

void load_PCB_TO_CPU(int PC){
    cpu_set_ip(PC);
}

/*
 * Function:  cpu_run 
 * 	Added in A2
 * --------------------
 * run "quanta"(first input parameter) lines of code, starting from the cpu.IP
 *
 * quanta: number of lines that the CPU will run before it switches task or the task ends
 * end: the last line of the task
 * pc: current process's program counter
 * returns: error code, -1: file reaches an end or on normal termination gives PC
 */
int cpu_run(int quanta, int end, int pc, int nextFrame, int frameIndex){
    aCPU.quanta = quanta;
    int error_code;
    int cnt=0;
    while (aCPU.quanta != 0){
        // copy line at provided frame + frameIndex
        strncpy(aCPU.IR, frame_get_value_by_line(aCPU.IP + frameIndex),1000);
        parseInput(aCPU.IR);
        // means program has ended
        if(end == pc + 1){
            error_code = -1;
            return error_code;
        }
        /*  old code
        if(end == aCPU.IP){
            error_code = 2;
            return error_code;
        }
        */

        // means have to change frame
        if(frameIndex >= 2){
            if(nextFrame == -1){
            printf("error in cpu.c no next frame\n");
            return -1;
            }
            cpu_set_ip(nextFrame);
            frameIndex = 0;            
        }else{
           frameIndex = frameIndex + 1;
        }
        //aCPU.IP = aCPU.IP + 1; // prob have to change
        aCPU.quanta -= 1;
        pc += 1;
        cnt += 1;
    }
    
    // way to report the change in pc and frameIndex
    if(frameIndex == 0 && cnt == 1){
        error_code = 10;
    }else if(frameIndex == 1 && cnt == 1){
        error_code = 11;
    }else if(frameIndex == 2 && cnt == 1){
        error_code = 12;
    }else if(frameIndex == 0 && cnt == 2){
        error_code = 20;
    }else if(frameIndex == 1 && cnt == 2){
        error_code = 21;
    }else if(frameIndex == 2 && cnt == 2){
        error_code = 22;
    }
    return error_code;
}
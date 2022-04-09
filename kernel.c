#include "pcb.h"
#include "kernel.h"
#include "cpu.h"
#include "shell.h"
#include "shellmemory.h"
#include "interpreter.h"
#include "harddisk.h"


#include <stdint.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include <unistd.h>


#define QUEUE_LENGTH 10
#define MAX_INT 2147483646
PCB* readyQueue[QUEUE_LENGTH];

void ready_queue_initialize()
{
    for (size_t i = 0; i < QUEUE_LENGTH; ++i)
    {
        readyQueue[i] = (PCB*)malloc(sizeof(PCB));
        (*readyQueue[i]).PC = -1;
        (*readyQueue[i]).start = -1;
        (*readyQueue[i]).end = -1;
        (*readyQueue[i]).pid = NULL;
        (*readyQueue[i]).pageTableIndex = -1;
        (*readyQueue[i]).frameIndex = -1;
        for (int j = 0; j<100; j++){
                (*readyQueue[i]).pageTable[j] = -1;
        }
        //(*readyQueue[i]).job_length_score = -1;
    }
}

void ready_queue_Empty(){
    for (size_t i = 0; i < QUEUE_LENGTH; ++i)
    {
        (*readyQueue[i]).PC = -1;
        (*readyQueue[i]).start = -1;
        (*readyQueue[i]).end = -1;
        (*readyQueue[i]).pid = NULL;
        (*readyQueue[i]).pageTableIndex = -1;
        (*readyQueue[i]).frameIndex = -1;

        for (int j = 0; j<100; j++){
                (*readyQueue[i]).pageTable[j] = -1;
        }
        //(*readyQueue[i]).job_length_score = -1;
    }
}

void ready_queue_destory()
{
    for (size_t i = 0; i < QUEUE_LENGTH; ++i)
    {
        free(readyQueue[i]);
    }
}

PCB ready_queue_pop(int index, bool inPlace)
{
    PCB head = (*readyQueue[index]);
    if(inPlace){
        for (size_t i = index+1; i < QUEUE_LENGTH; i++){
            (*readyQueue[i-1]).PC = (*readyQueue[i]).PC;
            (*readyQueue[i-1]).start = (*readyQueue[i]).start;
            (*readyQueue[i-1]).end = (*readyQueue[i]).end;
            (*readyQueue[i-1]).pid = (*readyQueue[i]).pid;
            (*readyQueue[i-1]).pageTableIndex = (*readyQueue[i]).pageTableIndex;
            (*readyQueue[i-1]).frameIndex = (*readyQueue[i]).frameIndex;

            for (int j = 0; j<100; j++){
                (*readyQueue[i-1]).pageTable[j] = (*readyQueue[i]).pageTable[j];
            }
            //(*readyQueue[i-1]).job_length_score = (*readyQueue[i]).job_length_score;
        }
        (*readyQueue[QUEUE_LENGTH-1]).PC = -1;
        (*readyQueue[QUEUE_LENGTH-1]).start = -1;
        (*readyQueue[QUEUE_LENGTH-1]).end = -1;
        (*readyQueue[QUEUE_LENGTH-1]).pid = NULL;
        (*readyQueue[QUEUE_LENGTH-1]).pageTableIndex = -1;
        (*readyQueue[QUEUE_LENGTH-1]).frameIndex = -1;
        for (int j = 0; j<100; j++){
                (*readyQueue[QUEUE_LENGTH-1]).pageTable[j] = -1;
        }
        //(*readyQueue[QUEUE_LENGTH-1]).job_length_score = -1;
    }
    return head;
}

void ready_queue_add_to_end(PCB *pPCB)
{
    for(int i = 0; i < QUEUE_LENGTH; i++){
        if ( (*readyQueue[i]).start == -1 ){
            (*readyQueue[i]).PC = (*pPCB).PC;
            (*readyQueue[i]).start = (*pPCB).start;
            (*readyQueue[i]).end = (*pPCB).end;
            (*readyQueue[i]).pid = (*pPCB).pid;
            (*readyQueue[i]).pageTableIndex = (*pPCB).pageTableIndex;
            (*readyQueue[i]).frameIndex = (*pPCB).frameIndex;

            for (int j = 0; j<100; j++){
                (*readyQueue[i]).pageTable[j] = (*pPCB).pageTable[j];
            }
            //(*readyQueue[i]).job_length_score = (*pPCB).job_length_score;
            break;
        }
    }
}

void ready_queue_add_to_front(PCB *pPCB){
    for (size_t i = QUEUE_LENGTH-1; i > 0; i--){
        (*readyQueue[i]).PC = (*readyQueue[i-1]).PC;
        (*readyQueue[i]).start = (*readyQueue[i-1]).start;
        (*readyQueue[i]).end = (*readyQueue[i-1]).end;
        (*readyQueue[i]).pid = (*readyQueue[i-1]).pid;
        (*readyQueue[i]).pageTableIndex = (*readyQueue[i-1]).pageTableIndex;
        (*readyQueue[i]).frameIndex = (*readyQueue[i-1]).frameIndex;

        for (int j = 0; j<100; j++){
            (*readyQueue[i]).pageTable[j] = (*readyQueue[i-1]).pageTable[j];
        }
        //(*readyQueue[i]).job_length_score = (*readyQueue[i-1]).job_length_score;
    }
    // readyQueue[0] = pPCB;
    (*readyQueue[0]).PC = (*pPCB).PC;
    (*readyQueue[0]).start = (*pPCB).start;
    (*readyQueue[0]).end = (*pPCB).end;
    (*readyQueue[0]).pid = (*pPCB).pid;
    (*readyQueue[0]).pageTableIndex = (*pPCB).pageTableIndex;
    (*readyQueue[0]).frameIndex = (*pPCB).frameIndex;

    for (int j = 0; j<100; j++){
        (*readyQueue[0]).pageTable[j] = (*pPCB).pageTable[j];
    }
    //(*readyQueue[0]).job_length_score = (*pPCB).job_length_score;
}

bool is_ready_empty(){
    for (size_t i = 0; i < QUEUE_LENGTH; ++i)
    {
        if((*readyQueue[i]).start != -1){
            return false;
        }  
    }
    return true;
}

void terminate_task_in_queue_by_index(int i){
    (*readyQueue[i]).start = -1; 
    (*readyQueue[i]).end = -1; 
    (*readyQueue[i]).PC = -1; 
    (*readyQueue[i]).pid = NULL;
    (*readyQueue[i]).pageTableIndex = -1; 
    (*readyQueue[i]).frameIndex = -1; 

    for (int j = 0; j<100; j++){
        (*readyQueue[i]).pageTable[j] = -1;
    }
    //(*readyQueue[i]).job_length_score = -1;
}


int update_pageTable_by_frameStore_index(int index){
    for(int i = 0; i < QUEUE_LENGTH; i++){
        if((*readyQueue[i]).PC == -1){
            // skip to next
        }else{
            for(int j = 0; j < 100; j++){
                if((*readyQueue[i]).pageTable[j] == index){
                    (*readyQueue[i]).pageTable[j] = -1;
                    return 0;
                }
            }
        }
    }
    return -1;
}

// not rly used anymore
int myinit(const char *filename){
    FILE* fp;
    int error_code = 0;
    int* start = (int*)malloc(sizeof(int));
    int* end = (int*)malloc(sizeof(int));
    
    fp = fopen(filename, "rt");
    if(fp == NULL){
        error_code = 11; // 11 is the error code for file does not exist
        return error_code;
    }

    //generate a random ID as file ID
    char* fileID = (char*)malloc(32);
    sprintf(fileID, "%d", rand());

    error_code = add_file_to_mem(fp, start, end, fileID);
    if(error_code != 0){
        fclose(fp);
        return error_code;
    }
    PCB* newPCB = makePCB(*start,*end,fileID);
   // newPCB -> job_length_score = 1 + *end - *start;

    ready_queue_add_to_end(newPCB);

    fclose(fp);

    return error_code;
}

int get_scheduling_policy_number(char* policy){
    if(strcmp("FCFS",policy)==0){
        return 0;
    }else if(strcmp("SJF",policy)==0){
        return 1;
    }else if(strcmp("RR",policy)==0){
        return 2;
    }else if(strcmp("AGING",policy)==0){
        return 3;
    }else{
        //error code 15
        return 15;
    }
}

// gives the pc change
int handleError_pc(int error_code){
    int ec = error_code;
    if(ec < 0){
        ec = ec * -1;
    }
    if(ec < 20){
        return 1;
    }
    return 2;
}

// gives the frameIndex change
int handleError_frameIndex(int error_code){
        int ec = error_code;
        if(ec < 0){
            ec = ec * -1;
        }   
        return ec % 10;
}

/*
 * Variable:  schedulingPolicy 
 * --------------------
 * 0: FCFS
 * 1: SJF
 * 2: RR
 * 3: AGING
 */
int scheduler(int policyNumber){
    int error_code = 0;

    int cpu_quanta_per_program = 2;

    //FCFS and SJF: running program will stop when it finishes
    if( policyNumber == 0 || policyNumber == 1 ){
        cpu_quanta_per_program = MAX_INT;
    }else if(policyNumber == 3){
        cpu_quanta_per_program = 1;
    }

    //scheduling logic for 0: FCFS and 2: RR
    if(policyNumber == 0 || policyNumber == 2){
        char* target_file = (char*)calloc(1,150);
        size_t current_directory_size = 9999;
        char* temp = "/backingStore/";
        /*
        printf("%s\n", frame_get_value_by_line(0));
        printf("%s\n", frame_get_value_by_line(1));
        printf("%s\n", frame_get_value_by_line(2));
        printf("%s\n", frame_get_value_by_line(3));
        printf("%s\n", frame_get_value_by_line(4));
        printf("%s\n", frame_get_value_by_line(5));
        printf("%s\n", frame_get_value_by_line(6));
        printf("%s\n", frame_get_value_by_line(7));
        printf("%s\n", frame_get_value_by_line(8));
        printf("%s\n", frame_get_value_by_line(9));
        printf("%s\n", frame_get_value_by_line(10));
        printf("%s\n", frame_get_value_by_line(11));
        printf("%s\n", frame_get_value_by_line(12));
        printf("%s\n", frame_get_value_by_line(13));
        printf("%s\n", frame_get_value_by_line(14));
        printf("%s\n", frame_get_value_by_line(15));
        printf("%s\n", frame_get_value_by_line(16));
        printf("%s\n", frame_get_value_by_line(17));
        */



        //keep running programs while ready queue is not empty
        while(ready_queue_pop(0,false).PC != -1){   
            int next_frame = -1;
            int frame_store_index, errCode;
            PCB firstPCB = ready_queue_pop(0,false);
            printf("PC is :%d\n", firstPCB.PC);
            printf("pageTableIndex is :%d\n", firstPCB.pageTableIndex);
            printf("frameIndex is :%d\n", firstPCB.frameIndex);

            //load_PCB_TO_CPU(firstPCB.PC);
            //int error_code_load_PCB_TO_CPU = cpu_run(cpu_quanta_per_program, firstPCB.end);

            // give frame
            int pc_in_frame_store = firstPCB.pageTable[firstPCB.pageTableIndex];

            // if we try to execute first line but its not in memory
            if(pc_in_frame_store == -1){
                printf("%s\n", "had to change frame at initial");
                //load next frame
                frame_store_index = find_empty_frame();
                //no more space in framestore
                int index_to_be_cleaned = 0;
                if(frame_store_index == -1){
                    printf(" Page fault! Victim page contents: \n %s \n %s \n %s \n End of victim page contents. \n ", 
                            frame_get_value_by_line(index_to_be_cleaned),
                            frame_get_value_by_line(index_to_be_cleaned + 1),
                            frame_get_value_by_line(index_to_be_cleaned + 2));
                    clean_one_frame(index_to_be_cleaned);
                    update_pageTable_by_frameStore_index(index_to_be_cleaned);
                    frame_store_index = find_empty_frame();
                }

                getcwd(target_file, current_directory_size);
                strncat(target_file, temp, 15);
                strncat(target_file, firstPCB.pid, 999);

                errCode = load_frame_from_disk(frame_store_index, firstPCB.PC, firstPCB.pid, target_file);
                if(errCode == 11 | errCode == 12){
		            return handleError(errCode);
	            }

                firstPCB.pageTable[firstPCB.pageTableIndex] = frame_store_index;
                ready_queue_pop(0,true);
                ready_queue_add_to_end(&firstPCB);
            }else{
                //give next frame location for if we have to switch frames
                if(firstPCB.pageTable[firstPCB.pageTableIndex + 1] != -1){
                    next_frame = firstPCB.pageTable[firstPCB.pageTableIndex + 1];
                }
                load_PCB_TO_CPU(pc_in_frame_store);   
                //^ current line is at the ith pageTable index frame
                
                int error_code_load_PCB_TO_CPU = cpu_run(cpu_quanta_per_program, firstPCB.end, firstPCB.PC, next_frame, firstPCB.frameIndex);
            
                // will have to be updated
                if(error_code_load_PCB_TO_CPU == -1){
                    //the head PCB program has been done, time to reclaim the shell mem
                    //clean_mem(firstPCB.start, firstPCB.end);
                    ready_queue_pop(0,true);
                }
                // means page fault
                if(error_code_load_PCB_TO_CPU < -1){
                    firstPCB.PC = firstPCB.PC + handleError_pc(error_code_load_PCB_TO_CPU);
                    // if old frameIndex is bigger than the new one, it means we changed frames
                    firstPCB.pageTableIndex = firstPCB.pageTableIndex + 1;
                    firstPCB.frameIndex = handleError_frameIndex(error_code_load_PCB_TO_CPU);
                    //load next frame
                    frame_store_index = find_empty_frame();

                    getcwd(target_file, current_directory_size);
                    strncat(target_file, temp, 15);
                    strncat(target_file, firstPCB.pid, 999);

                    errCode = load_frame_from_disk(frame_store_index, firstPCB.PC, firstPCB.pid, target_file);
                    if(errCode == 11 | errCode == 12){
                        return handleError(errCode);
                    }

                    firstPCB.pageTable[firstPCB.pageTableIndex] = frame_store_index;
                    ready_queue_pop(0,true);
                    ready_queue_add_to_end(&firstPCB);
                }
                
                if(error_code_load_PCB_TO_CPU > 0){
                    //the head PCB program has finished its quanta, it need to be put to the end of ready queue
                    firstPCB.PC = firstPCB.PC + handleError_pc(error_code_load_PCB_TO_CPU);
                    // if old frameIndex is bigger than the new one, it means we changed frames
                    if(firstPCB.frameIndex > handleError_frameIndex(error_code_load_PCB_TO_CPU)){
                        firstPCB.pageTableIndex = firstPCB.pageTableIndex + 1;
                    }
                    firstPCB.frameIndex = handleError_frameIndex(error_code_load_PCB_TO_CPU);

                    ready_queue_pop(0,true);
                    ready_queue_add_to_end(&firstPCB);
                }
            }
        }
        free(target_file);
    }

    //scheduling policy for 1: SJF
    /* <- not used so commented out
    if(policyNumber == 1){
        while(!is_ready_empty())
        {
            //task with the lowest lines of codes runs first
            int task_index_with_the_least_lines;
            int task_lines = MAX_INT;
            //get the lowest job length 
            for(int i = 0; i < QUEUE_LENGTH; i++){
                if((*readyQueue[i]).start != -1 && 1 + (*readyQueue[i]).end - (*readyQueue[i]).start < task_lines){
                    task_lines = 1 + (*readyQueue[i]).end - (*readyQueue[i]).start;
                    task_index_with_the_least_lines = i;
                }
            }

            PCB current_task_PCB = (*readyQueue[task_index_with_the_least_lines]);
            load_PCB_TO_CPU(current_task_PCB.PC);
            
            int error_code_load_PCB_TO_CPU = cpu_run(cpu_quanta_per_program, current_task_PCB.end);
            
            //the head PCB program has been done, time to reclaim the shell mem
            clean_mem(current_task_PCB.start, current_task_PCB.end);
            //put the current PCB into invalid mode
            terminate_task_in_queue_by_index(task_index_with_the_least_lines);
        }
    }
    */
    //scheduling policy for 3: Aging   <- not used so commented out
    /*
    if(policyNumber == 3){
        int task_index_least_job_length_score;
        int task_job_length_score = MAX_INT;

        //find job with the lowest job score
        for(int i = 0; i < QUEUE_LENGTH; i++){
            //get the lowest job length score
            if((*readyQueue[i]).start != -1 && (*readyQueue[i]).job_length_score < task_job_length_score){
                task_job_length_score = (*readyQueue[i]).job_length_score;
                task_index_least_job_length_score = i;
            }
        }
        //move the task with the lowest job score to the front of the queue
        PCB job_with_lowest_job_score = ready_queue_pop(task_index_least_job_length_score,true);
        ready_queue_add_to_front(&job_with_lowest_job_score);
        
        while(!is_ready_empty())
        {
            //task with the lowest job length score runs first
            //in this case, the task with the lowest job length score is the first task in queue
            task_job_length_score = (*readyQueue[0]).job_length_score;
            task_index_least_job_length_score = 0;

            PCB current_task_PCB = (*readyQueue[task_index_least_job_length_score]);
            load_PCB_TO_CPU(current_task_PCB.PC);
            
            int error_code_load_PCB_TO_CPU = cpu_run(cpu_quanta_per_program, current_task_PCB.end);

            if(error_code_load_PCB_TO_CPU == 2){
                //the head PCB program has been done, time to reclaim the shell mem
                clean_mem((*readyQueue[task_index_least_job_length_score]).start, (*readyQueue[task_index_least_job_length_score]).end);
                ready_queue_pop(task_index_least_job_length_score, true);
                task_job_length_score = MAX_INT;
            }

            if(error_code_load_PCB_TO_CPU == 0){
                //the head PCB program has finished its quanta
                (*readyQueue[task_index_least_job_length_score]).PC = cpu_get_ip(); // update the PC for the PCB
                //Age all the tasks (other than the current executing task) in queue by 1
                for(int i = 0; i < QUEUE_LENGTH; i++){
                    //get the lowest job length score
                    if((*readyQueue[i]).start != -1 && (*readyQueue[i]).job_length_score > 0 && i != task_index_least_job_length_score){
                        (*readyQueue[i]).job_length_score -= 1;
                    }
                }
            }
            
            //if the first task job score is not the lowest, 
            //then move the frst task to the end 
            //and the lowest job score task to the front
            for(int i = 0; i < QUEUE_LENGTH; i++){
                //get the lowest job length score
                if((*readyQueue[i]).start != -1 && (*readyQueue[i]).job_length_score < task_job_length_score){
                    task_job_length_score = (*readyQueue[i]).job_length_score;
                    task_index_least_job_length_score = i;
                }
            }
            if(task_index_least_job_length_score != 0){
                //pop the task with the lowest job score 
                PCB lowest_job_score_task = ready_queue_pop(task_index_least_job_length_score, true);
                //move the frst task to the end
                PCB first_pcb = ready_queue_pop(0, true);
                ready_queue_add_to_end(&first_pcb);
                //move the lowest job score task to the front
                ready_queue_add_to_front(&lowest_job_score_task);
            }
        
        }
    }
    */
    //clean up
    ready_queue_Empty();
    cpu_empty();

    return error_code;
}


/*
int main(int argc, char *argv[]) {
  
    ready_queue_initialize();
    int i = ready_queue_pop(0, false).PC;
    printf("%d", i);


}
*/
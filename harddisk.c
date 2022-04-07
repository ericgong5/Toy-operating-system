#include <stdio.h>
#include <stdlib.h>
#include <string.h> 
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <stdbool.h>

#include "pcb.h"
#include "kernel.h"
#include "cpu.h"
#include "shell.h"
#include "shellmemory.h"
#include "interpreter.h"

struct stat st = {0};

int create_backing_store(){

    if (stat("backingStore", &st) == 0) {
        system("rm -r backingStore");
    }   
    return mkdir("backingStore", 0777);
}   

void delete_backing_store(){
    if (stat("backingStore", &st) == 0) {
        system("rm -r backingStore");
    }   
}


/*  
Takes in filename to be stored in backing store and puts it there

returns: error code, 12: couldnt open file in backing store   
*/ 
int load_script_to_backing_store(const char *filename, char *file_and_path){
    FILE* fp, *target;
    int error_code = 0;
    char ch;// source_file[20], target_file[20];
    
    fp = fopen(filename, "rt");
    if(fp == NULL){
        error_code = 11; // 11 is the error code for file does not exist
        return error_code;
    }

    target = fopen(file_and_path, "w");
    if (target == NULL) {
        fclose(fp);
        error_code = 12; // 12 is the error code for file couldnt be stored
        return error_code;
    }
    while ((ch = fgetc(fp)) != EOF){
        fputc(ch, target);
    }
    fclose(fp);
    fclose(target);
    return error_code; 
}

int count_file_lines(char *filename){
    // count the number of lines in the file called filename                                    
    FILE *fp = fopen(filename,"r");
    int ch=0;
    int lines=0;

    if (fp == NULL){
        return -1;
    }
    lines++;
    while ((ch = fgetc(fp)) != EOF){
        if (ch == '\n'){
            lines++;
        }
    }
    fclose(fp);
    return lines;

}



/*
int main(int argc, char *argv[]) {
    int lines = count_file_lines("file1.txt");
    printf("%d", lines);

}
*/




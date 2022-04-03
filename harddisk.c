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


/*  returns: error code, 12: couldnt open file in backing store   
*/ 
int load_script_to_backing_store(const char *filename){
    FILE* fp, *target;
    char* target_file = (char*)calloc(1,150);
    int error_code = 0;
    char ch;// source_file[20], target_file[20];
    size_t current_directory_size = 9999;
    char* str1 = "/backingStore/";

    fp = fopen(filename, "rt");
    if(fp == NULL){
        error_code = 11; // 11 is the error code for file does not exist
        return error_code;
    }

    //might bug
    getcwd(target_file, current_directory_size);
    strncat(target_file, str1, 15);
    strncat(target_file, filename, 999);


    target = fopen(target_file, "w");
    if (target == NULL) {
        fclose(fp);
        error_code = 12;
        return error_code;
    }
    while ((ch = fgetc(fp)) != EOF){
        fputc(ch, target);
    }
    fclose(fp);
    fclose(target);
    return error_code; 
}
    


int main(int argc, char *argv[]) {
    /*char* target_file = (char*)calloc(1,150);
    size_t current_directory_size = 9999;
    char* filename = "file2.txt";
    char* str1 = "/backingStore/";
    getcwd(target_file, current_directory_size);
    strncat(target_file, str1, 15);
    strncat(target_file, filename, 999);
*/
   
   //load_script_to_backing_store("file1.txt");


}




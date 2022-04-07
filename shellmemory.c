#include<stdlib.h>
#include<string.h>
#include<stdio.h>
#include<stdbool.h>

#define SHELL_MEM_LENGTH 1000
#define FRAME_STORE_LENGTH 30


struct memory_struct{
	char *var;
	char *value;	
};


//maybe put a frame num
struct memory_struct framestore[FRAME_STORE_LENGTH];

struct memory_struct shellmemory[SHELL_MEM_LENGTH];

// Shell memory functions

void mem_init(){

	int i;
	for (i=0; i<SHELL_MEM_LENGTH; i++){		
		shellmemory[i].var = "none";
		shellmemory[i].value = "none";
	}

	for (i=0; i<FRAME_STORE_LENGTH; i++){		
		framestore[i].var = "none";
		framestore[i].value = "none";
	}

}

//variable store part

// Set key value pair
void mem_set_value(char *var_in, char *value_in) {

	int i;

	for (i=0; i<SHELL_MEM_LENGTH; i++){
		if (strcmp(shellmemory[i].var, var_in) == 0){
			shellmemory[i].value = strdup(value_in);
			return;
		} 
	}

	//Value does not exist, need to find a free spot.
	for (i=0; i<SHELL_MEM_LENGTH; i++){
		if (strcmp(shellmemory[i].var, "none") == 0){
			shellmemory[i].var = strdup(var_in);
			shellmemory[i].value = strdup(value_in);
			return;
		} 
	}

	return;

}


//get value based on input key
char *mem_get_value(char *var_in) {
	int i;

	for (i=0; i<SHELL_MEM_LENGTH; i++){
		if (strcmp(shellmemory[i].var, var_in) == 0){
			return strdup(shellmemory[i].value);
		} 
	}
	return "Variable does not exist";

}

char* mem_get_value_by_line(int line){
	return shellmemory[line].value;
}


void clean_mem(int start, int end){
    for(int i = start; i <= end; i ++){
        shellmemory[i].var = "none";
		shellmemory[i].value = "none";
    }
}

void clean_all_mem(){
  for(int i = 0; i <= SHELL_MEM_LENGTH; i ++){
        shellmemory[i].var = "none";
		shellmemory[i].value = "none";
    }
}

//framestore part

int get_frame_store_length(){
	return FRAME_STORE_LENGTH;
}

// error code -1 if no empty frame found
int find_empty_frame(){
	for (int i = 0; i < FRAME_STORE_LENGTH;){
        if(strcmp(framestore[i].var,"none") == 0){
            return i;;
        }
        i = i + 3;
    }
	return -1;
}

//edge case of if program_counter is out of bounds
// error code : -1 if end of file
// frame_line = index in frame store where to add frame
// program_counter = line of next program to be loaded (starts at 0)
// filename = name of file in backing store to be loaded from 
int load_frame_from_disk(int frame_line, int program_counter, const char *filename, const char *filepath){
	int errCode = 0;
	FILE* target_file;
    char line[SHELL_MEM_LENGTH];

	target_file = fopen(filepath, "rt");
    if(target_file == NULL){
        errCode = 12; // 11 is the error code for file does not exist
        return errCode;
    }

	//skip to the line that we want
	for(int i = 0; i < program_counter;i++){
        fgets(line, 999, target_file);
	}
	//copy line into frame store
	for(int j = frame_line; j < frame_line + 3; j++){
		if(feof(target_file)){
			errCode = -1; // out of lines to copy
            break;
        }else{
			fgets(line, 999, target_file);
			framestore[j].var = strdup(filename);
    		framestore[j].value = strdup(line);
		}
	}
	fclose(target_file);
	return errCode;
}

void frame_set_value(char *var_in, char *value_in) {

	int i;

	for (i=0; i<FRAME_STORE_LENGTH; i++){
		if (strcmp(framestore[i].var, var_in) == 0){
			framestore[i].value = strdup(value_in);
			return;
		} 
	}

	//Value does not exist, need to find a free spot.
	for (i=0; i<FRAME_STORE_LENGTH; i++){
		if (strcmp(framestore[i].var, "none") == 0){
			framestore[i].var = strdup(var_in);
			framestore[i].value = strdup(value_in);
			return;
		} 
	}

	return;

}

//get value based on input key
char *frame_get_value(char *var_in) {
	int i;

	for (i=0; i<FRAME_STORE_LENGTH; i++){
		if (strcmp(framestore[i].var, var_in) == 0){
			return strdup(framestore[i].value);
		} 
	}
	return "Frame does not exist";

}

char* frame_get_value_by_line(int line){
	return framestore[line].value;
}

// cleans one frame when given the start index
void clean_one_frame(int start){
    for(int i = start; i < start + 3; i ++){
        framestore[i].var = "none";
		framestore[i].value = "none";
    }
}


/*
 * Function:  addFileToMem 
 * 	Added in A2
 * --------------------
 * Load the source code of the file fp into the shell memory:
 * 		Loading format - var stores fileID, value stores a line
 *		Note that the first 100 lines are for set command, the rests are for run and exec command
 *
 *  pStart: This function will store the first line of the loaded file 
 * 			in shell memory in here
 *	pEnd: This function will store the last line of the loaded file 
 			in shell memory in here
 *  fileID: Input that need to provide when calling the function, 
 			stores the ID of the file
 * 
 * returns: error code, 21: no space left
 */
int add_file_to_mem(FILE* fp, int* pStart, int* pEnd, char* fileID)
{
    char line[SHELL_MEM_LENGTH];
    size_t i;
    int error_code = 0;
	bool hasSpaceLeft = false;

    for (i = 100; i < SHELL_MEM_LENGTH; i++){
        if(strcmp(shellmemory[i].var,"none") == 0){
            *pStart = (int)i;
			hasSpaceLeft = true;
            break;
        }
    }

	//shell memory is full
	if(hasSpaceLeft == 0){
		error_code = 21;
		return error_code;
	}
    
    for (size_t j = i; j < SHELL_MEM_LENGTH; j++){
        if(feof(fp))
        {
            *pEnd = (int)j-1;
            break;
        }else{
            fgets(line, 999, fp);
			shellmemory[j].var = strdup(fileID);
            shellmemory[j].value = strdup(line);
        }
    }

	//no space left to load the entire file into shell memory
	if(!feof(fp)){
		error_code = 21;
		//clean up the file in memory
		for(int j = 1; i <= SHELL_MEM_LENGTH; i ++){
			shellmemory[j].var = "none";
			shellmemory[j].value = "none";
    	}
		return error_code;
	}

    return error_code;
}

/*
int main(int argc, char *argv[]) {
	/*
	mem_init();
	char *filename = "file1.txt";
	char *filepath = "/home/2020/egong1/A3/backingStore/file1.txt";
	load_frame_from_disk(0, 0, filename, filepath);
	char *line0 = frame_get_value_by_line(0);
	char *line1 = frame_get_value_by_line(1);
	char *line2 = frame_get_value_by_line(2);
	printf("%s\n", line0);
	printf("%s\n", line1);
	printf("%s\n", line2);
	
	load_frame_from_disk(3, 3, filename, filepath);
	char *line3 = frame_get_value_by_line(3);
	char *line4 = frame_get_value_by_line(4);
	char *line5 = frame_get_value_by_line(5);
	printf("%s\n", line3);
	printf("%s\n", line4);
	printf("%s\n", line5);



}
*/
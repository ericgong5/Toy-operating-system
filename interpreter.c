#include <stdio.h>
#include <stdlib.h>
#include <string.h> 
#include <unistd.h>
#include <stdbool.h>

#include "harddisk.h"
#include "shellmemory.h"
#include "shell.h"
#include "kernel.h"
#include "pcb.h"


int MAX_ARGS_SIZE = 7;

int help();
int quit();
int badcommand();
int badcommandTooManyTokens();
int bad_command_file_does_not_exist();
int badcommand_file_could_not_be_stored();
int badcommand_scheduling_policy_error();
int badcommand_no_mem_space();
int badcommand_ready_queue_full();
int badcommand_same_file_name();
int handleError(int error_code);
int set(char* var, char* value);
int print(char* var);
int run(char* script);
int exec(char *fname1, char *fname2, char *fname3, char* policy);
int my_ls();
int echo();
int resetmem();

int interpreter(char* command_args[], int args_size){
	int i;

	if ( args_size < 1 || args_size > MAX_ARGS_SIZE){
		if (strcmp(command_args[0], "set")==0 && args_size > MAX_ARGS_SIZE) {
			return badcommandTooManyTokens();
		}
		return badcommand();
	}

	for ( i=0; i<args_size; i++){ //strip spaces new line etc
		command_args[i][strcspn(command_args[i], "\r\n")] = 0;
	}

	if (strcmp(command_args[0], "help")==0){
	    //help
	    if (args_size != 1) return badcommand();
	    return help();
	
	} else if (strcmp(command_args[0], "quit")==0) {
		//quit
		if (args_size != 1) return badcommand();
		return quit();

	} else if (strcmp(command_args[0], "set")==0) {
		//set
		if (args_size < 3) return badcommand();
		char* value = (char*)calloc(1,150);
		char spaceChar = ' ';

		for(int i = 2; i < args_size; i++){
			strncat(value, command_args[i], 30);
			if(i < args_size-1){
				strncat(value, &spaceChar, 1);
			}
		}
		return set(command_args[1], value);
	
	} else if (strcmp(command_args[0], "print")==0) {
		if (args_size != 2) return badcommand();
		return print(command_args[1]);
	
	} else if (strcmp(command_args[0], "run")==0) {
		if (args_size != 2) return badcommand();
		return run(command_args[1]);
	
	} else if (strcmp(command_args[0], "exec")==0) {
		if (args_size <= 2 || args_size >5) return badcommand();
		if(args_size == 3){
			return exec(command_args[1],NULL,NULL,command_args[2]);
		}else if(args_size == 4){
			return exec(command_args[1],command_args[2],NULL,command_args[3]);
		}else if(args_size == 5){
			return exec(command_args[1],command_args[2],command_args[3],command_args[4]);
		}
	} else if (strcmp(command_args[0], "my_ls")==0) {
		if (args_size > 2) return badcommand();
		return my_ls();
	
	}else if (strcmp(command_args[0], "echo")==0) {
		if (args_size > 2) return badcommand();
		return echo(command_args[1]);
	
	}if (strcmp(command_args[0], "resetmem")==0){
	    if (args_size != 1) return badcommand();
	    return resetmem();
	
	}else return badcommand();
}

int help(){
	char help_string[] = "COMMAND			DESCRIPTION\n \
help			Displays all the commands\n \
quit			Exits / terminates the shell with “Bye!”\n \
set VAR STRING		Assigns a value to shell memory\n \
print VAR		Displays the STRING assigned to VAR\n \
run SCRIPT.TXT		Executes the file SCRIPT.TXT\n ";
	printf("%sFrame Store Size = %d; Variable Store Size = %d\n\n", help_string, framesize, varmemsize);
	return 0;
}

int quit(){
	delete_backing_store();
	printf("%s\n", "Bye!");
	exit(0);
}

int badcommand(){
	printf("%s\n", "Unknown Command");
	return 1;
}

int badcommandTooManyTokens(){
	printf("%s\n", "Bad command: Too many tokens");
	return 1;
}

int bad_command_file_does_not_exist(){
	printf("%s\n", "Bad command: File not found");
	return 1;
}

int badcommand_backing_store_file_does_not_exist(){
	printf("%s\n", "Bad command: File in backing store not found");
	return 1;
}

int badcommand_scheduling_policy_error(){
	printf("%s\n", "Bad command: scheduling policy incorrect");
	return 1;
}

int badcommand_no_mem_space(){
	printf("%s\n", "Bad command: no space left in shell memory");
	return 1;
}

int badcommand_ready_queue_full(){
	printf("%s\n", "Bad command: ready queue is full");
	return 1;
}

int badcommand_same_file_name(){
	printf("%s\n", "Bad command: same file name");
	return 1;
}

int handleError(int error_code){
	//Note: badcommand-too-man-token(), badcommand(), and badcommand-same-file-name needs to be raised by programmer, not this function
	if(error_code == 11){
		return bad_command_file_does_not_exist();
	}else if (error_code == 21)
	{
		return badcommand_no_mem_space();
	}else if (error_code == 14)
	{
		return badcommand_ready_queue_full();
	}else if (error_code == 15){
		return badcommand_scheduling_policy_error();
	}else if (error_code == 12){
		return badcommand_backing_store_file_does_not_exist();
	}else{
		return 0;
	}
}

int set(char* var, char* value){
	mem_set_value(var, value);
	return 0;
}

int print(char* var){
	printf("%s\n", mem_get_value(var)); 
	return 0;
}

int run(char* script){
	//errCode 11: bad command file does not exist
	int errCode = 0;
    char* target_file = (char*)calloc(1,150);
	size_t current_directory_size = 9999;
    char* temp = "/backingStore/";
	int frame_store_index, program_counter;

	//might bug <- works for now
	//creates the file path for the backing store file
    getcwd(target_file, current_directory_size);
    strncat(target_file, temp, 15);
    strncat(target_file, script, 999);

	//load script into backing store
	errCode = load_script_to_backing_store(script, target_file);
	if(errCode == 11 | errCode == 12){
		return handleError(errCode);
	}

	//find start and end for PCB <- will have to change when not contiguous
	frame_store_index = find_empty_frame();
	int end = count_file_lines(target_file);
	// make pcb for loaded program and put in queue
	PCB* newPCB = makePCB(0,end,script);


	// takes in pc of where it left off and end of program if pc starts at 0
	program_counter = newPCB->PC;
	//end_of_program  = ready_queue_pop(0, false).end - ready_queue_pop(0, false).start;
	// load the page into the frame store
	//while(program_counter + 1 <= newPCB->end){
	for(int k = 0; k < 2; k++){
		if(program_counter + 1 > newPCB->end){
			break;
		}
		//do error code <- will become frame fault later
		frame_store_index = find_empty_frame();
		// prob will have to change <- is page fault later should never happen
		if( frame_store_index == -1){
			printf("%s\n", "error in interpreter.c, ran out of frame space, should never happen");
			errCode = 13; // 13 is the error code for frame out of space
			return errCode;
		}
		

		errCode = load_frame_from_disk(frame_store_index, program_counter, script, target_file);
		int woops = find_empty_page_table(newPCB);
		if(woops == -1){ // means ran out of page table space <- should never happen
			errCode = 0;
			printf("%s\n", "ran out of page table space, should never happen!!!");
			break;
		}
		newPCB->pageTable[woops] = frame_store_index;
		if(errCode == -1){ // means reached end of file
			errCode = 0;
			break;
		}
		program_counter = program_counter + 3;
	}
	ready_queue_add_to_end(newPCB);
	//run with RR <- to be modified prob
	scheduler(2);

	free(target_file);
	return errCode;
}

int exec(char *fname1, char *fname2, char *fname3, char* policy){
    int error_code = 0;
	int frame_store_index, program_counter, end_of_program;

	int policyNumber = get_scheduling_policy_number(policy);
	if(policyNumber == 15){
		return handleError(policyNumber);
	}

    if(fname1 != NULL){
        //error_code = myinit(fname1);
		int errCode = 0;
		char* target_file = (char*)calloc(1,150);
		size_t current_directory_size = 9999;
		char* temp = "/backingStore/";
		//might bug <- works for now
		//creates the file path for the backing store file
		getcwd(target_file, current_directory_size);
		strncat(target_file, temp, 15);
		strncat(target_file, fname1, 999);

		error_code = load_script_to_backing_store(fname1, target_file);
		if(error_code != 0){
			return handleError(error_code);
		}

		//find start and end for PCB 
		frame_store_index = find_empty_frame();
		int end = count_file_lines(target_file);
		// make pcb for loaded program and put in queue
		PCB* newPCB = makePCB(0, end,fname1);


		// takes in pc of where it left off and end of program if pc starts at 0
		program_counter = newPCB->PC;
		// load the page into the frame store
		//while(program_counter + 1 <= newPCB->end){
		for(int k = 0; k < 2; k++){
			if(program_counter + 1 > newPCB->end){
				break;
			}
			frame_store_index = find_empty_frame();
			
			errCode = load_frame_from_disk(frame_store_index, program_counter, fname1, target_file);
			int woops = find_empty_page_table(newPCB);
			if(woops == -1){ // means ran out of page table space <- should never happen
				errCode = 0;
				printf("%s\n", "ran out of page table space fname1, should never happen!!!");
				break;
			}
			newPCB->pageTable[woops] = frame_store_index;
			if(errCode == -1){ // means reached end of file
				errCode = 0;
				break;
			}
			program_counter = program_counter + 3;
		}
		ready_queue_add_to_end(newPCB);
		free(target_file);
    }
    if(fname2 != NULL){
        //error_code = myinit(fname1);
		int errCode = 0;
		char* target_file = (char*)calloc(1,150);
		size_t current_directory_size = 9999;
		char* temp = "/backingStore/";
		//might bug <- works for now
		//creates the file path for the backing store file
		getcwd(target_file, current_directory_size);
		strncat(target_file, temp, 15);
		strncat(target_file, fname2, 999);

		error_code = load_script_to_backing_store(fname2, target_file);
		if(error_code != 0){
			return handleError(error_code);
		}

		//find start and end for PCB 
		frame_store_index = find_empty_frame();
		int end = count_file_lines(target_file);
		// make pcb for loaded program and put in queue
		PCB* newPCB = makePCB(0, end,fname2);


		// takes in pc of where it left off and end of program if pc starts at 0
		program_counter = newPCB->PC;
		// load the page into the frame store
		//while(program_counter + 1 <= newPCB->end){

		for(int k = 0; k < 2; k++){
							            	//printf("%s\n", "hi");

			if(program_counter + 1 > newPCB->end){
				break;
			}
			//do error code <- will become frame fault later
			frame_store_index = find_empty_frame();
			errCode = load_frame_from_disk(frame_store_index, program_counter, fname2, target_file);

			int woops = find_empty_page_table(newPCB);
			if(woops == -1){ // means ran out of page table space <- should never happen
				errCode = 0;
				printf("%s\n", "ran out of page table space fname1, should never happen!!!");
				break;
			}

			newPCB->pageTable[woops] = frame_store_index;

			if(errCode == -1){ // means reached end of file
				errCode = 0;
				break;
			}
			program_counter = program_counter + 3;
		}

		ready_queue_add_to_end(newPCB);
		free(target_file);
    }
    if(fname3 != NULL){
		int errCode = 0;
		char* target_file = (char*)calloc(1,150);
		size_t current_directory_size = 9999;
    	char* temp = "/backingStore/";

        //error_code = myinit(fname1);

		//might bug <- works for now
		//creates the file path for the backing store file
		getcwd(target_file, current_directory_size);
		strncat(target_file, temp, 15);
		strncat(target_file, fname3, 999);

		error_code = load_script_to_backing_store(fname3, target_file);
		if(error_code != 0){
			return handleError(error_code);
		}

		//find start and end for PCB 
		frame_store_index = find_empty_frame();
		int end = count_file_lines(target_file);
		// make pcb for loaded program and put in queue
		PCB* newPCB = makePCB(0, end,fname3);


		// takes in pc of where it left off and end of program if pc starts at 0
		program_counter = newPCB->PC;
		// load the page into the frame store
		//while(program_counter + 1 <= newPCB->end){
		for(int k = 0; k < 2; k++){
			if(program_counter + 1 > newPCB->end){
				break;
			}
			frame_store_index = find_empty_frame();
			errCode = load_frame_from_disk(frame_store_index, program_counter, fname3, target_file);
			int woops = find_empty_page_table(newPCB);
			if(woops == -1){ // means ran out of page table space <- should never happen
				errCode = 0;
				printf("%s\n", "ran out of page table space fname1, should never happen!!!");
				break;
			}
			newPCB->pageTable[woops] = frame_store_index;
			if(errCode == -1){ // means reached end of file
				errCode = 0;
				break;
			}
			program_counter = program_counter + 3;
		}
		ready_queue_add_to_end(newPCB);
		free(target_file);
    }

	scheduler(2);
	return error_code;
}

int my_ls(){
	int errCode = system("ls | sort");
	return errCode;
}

int echo(char* var){
	if(var[0] == '$'){
		var++;
		printf("%s\n", mem_get_value(var)); 
	}else{
		printf("%s\n", var); 
	}
	return 0; 
}

int resetmem(){
	clean_all_mem();
	return 0;
}



/*
int main(int argc, char *argv[]) {
	create_backing_store();
    mem_init();
	ready_queue_initialize();

	int a = run("file1.txt");
	int b = run("file2.txt");
	
	char *line0 = frame_get_value_by_line(0);
	char *line1 = frame_get_value_by_line(1);
	char *line2 = frame_get_value_by_line(2);
	printf("%s\n", line0);
	printf("%s\n", line1);
	printf("%s\n", line2);
	char *line3 = frame_get_value_by_line(3);
	char *line4 = frame_get_value_by_line(4);
	char *line5 = frame_get_value_by_line(5);
	printf("%s\n", line3);
	printf("%s\n", line4);
	printf("%s\n", line5);
	char *line6 = frame_get_value_by_line(6);
	char *line7 = frame_get_value_by_line(7);
	char *line8 = frame_get_value_by_line(8);
	printf("%s\n", line6);
	printf("%s\n", line7);
	printf("%s\n", line8);
	char *line9 = frame_get_value_by_line(9);
	char *line10 = frame_get_value_by_line(10);
	char *line11 = frame_get_value_by_line(11);
	printf("%s\n", line9);
	printf("%s\n", line10);
	printf("%s\n", line11);
	

}

*/
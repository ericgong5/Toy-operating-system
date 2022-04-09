#include<stdlib.h>
#include<stdio.h>

void mem_init();
char *mem_get_value(char *var);
char* mem_get_value_by_line(int line);
void mem_set_value(char *var, char *value);
void clean_mem(int start, int end);
int add_file_to_mem(FILE* fp, int* pStart, int* pEnd, char* fileID);
void clean_all_mem();
int find_empty_frame(); 
int load_frame_from_disk(int frame_line, int program_counter, char *filename, char *filepath);
char *frame_get_value_by_line(int line);
void clean_one_frame(int start);

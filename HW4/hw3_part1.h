#ifndef HW3_PART1_H
#define HW3_PART1_H

unsigned long find_symbol(char* symbol_name, char* exe_file_name, int* error_val);
unsigned long get_dyn_adress(char* symbol_name, char* exe_file_name);

#endif /* HW3_PART1_H */
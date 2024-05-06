#include <stdio.h>
#include <stdarg.h>
#include <stdlib.h>
#include <signal.h>
#include <syscall.h>
#include <sys/ptrace.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/reg.h>
#include <sys/user.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <stdbool.h>
#include "elf64.h"
#include <fcntl.h>
#include <sys/mman.h>

#define	ET_NONE	0	//No file type 
#define	ET_REL	1	//Relocatable file 
#define	ET_EXEC	2	//Executable file 
#define	ET_DYN	3	//Shared object file 
#define	ET_CORE	4	//Core file 

/* symbol_name		- The symbol (maybe function) we need to search for.
 * exe_file_name	- The file where we search the symbol in.
 * error_val		- If  1: A global symbol was found, and defined in the given executable.
 * 			- If -1: Symbol not found.
 *			- If -2: Only a local symbol was found.
 * 			- If -3: File is not an executable.
 * 			- If -4: The symbol was found, it is global, but it is not defined in the executable.
 * return value		- The address which the symbol_name will be loaded to, if the symbol was found and is global.
 */
unsigned long find_symbol(char* symbol_name, char* exe_file_name, int* error_val)
{
    *error_val = -1;

    //open file for reading
    int file_disp = open(exe_file_name, O_RDONLY);
    if (file_disp == -1)
    {
        printf("%s", "could not open the file");
    }
    //get elf file header
    void* elf = mmap(NULL, lseek(file_disp, 0, SEEK_END), PROT_READ, MAP_PRIVATE, file_disp, 0);
    Elf64_Ehdr* header = (Elf64_Ehdr*)elf;
    //check if file is an executable
    if (header->e_type != ET_EXEC)
    {
        *error_val = -3;
        close(file_disp);
        return 0;
    }

    //get section header table with offset from header
    Elf64_Shdr* shdr_arr = (Elf64_Shdr*)((char*)elf + header->e_shoff);
    //get strings section
    Elf64_Shdr shdr_string_index = shdr_arr[header->e_shstrndx];
    //get strings table
    char* shdr_string_table = (char*)elf + shdr_string_index.sh_offset;
    //get sections number
    Elf64_Half sections_num = header->e_shnum;

    //init vars
    int symbols_num = 0;
    char* strtab = NULL;
    Elf64_Sym* symtab = NULL;

    //iterate over all the sections in the header
    for (int i = 0; i < sections_num; i++)
    {
        //get section name (string) from string table
        char* section = shdr_string_table + shdr_arr[i].sh_name;
        //is cuurent section the symtab?
        if (!strcmp(".symtab", section) || shdr_arr[i].sh_type == 2)
        {
            //get symbol table
            symtab = (Elf64_Sym*)((char*)elf + shdr_arr[i].sh_offset);
            //calculate number of symbols with (overall size / one entire size)
            symbols_num = shdr_arr[i].sh_size / shdr_arr[i].sh_entsize;
        }
        //is current section the strtab?
        else if (!strcmp(".strtab", section) && shdr_arr[i].sh_type == 3)
        {
            if ((char*)elf + shdr_arr[i].sh_offset != shdr_string_table)
            {
                //get strtab
                strtab = ((char*)elf + shdr_arr[i].sh_offset);
            }
        }
    }


    //iterate over all stmbols in symtab
    for (int i = 0; i < symbols_num; i++)
    {
        //get symbol name
        char* sym = strtab + symtab[i].st_name;
        //find the symbol we want
        if (!strcmp(symbol_name, sym))
        {
            //check if global symbol
            if (ELF64_ST_BIND(symtab[i].st_info) == 1)
            {
                //check if global and defined in a shared object
                if (symtab[i].st_shndx == SHN_UNDEF)
                {
                    *error_val = -4;
                    close(file_disp);
                    return 0;
                }
                else
                {
                    //we found our symbol, it is global and defined in the exe
                    close(file_disp);
                    *error_val = 1;
                    return symtab[i].st_value;
                }
            }
            else
            {
                *error_val = -2;
                close(file_disp);
                return 0;
            }
        }
    }

    close(file_disp);
    return 0;
}



unsigned long get_dyn_adress(char* symbol_name, char* exe_file_name)
{
    //open file for reading
    int file_disp = open(exe_file_name, O_RDONLY);
    if (file_disp == -1)
    {
        printf("%s", "could not open the file");
    }
    //get elf file header
    void* elf = mmap(NULL, lseek(file_disp, 0, SEEK_END), PROT_READ, MAP_PRIVATE, file_disp, 0);
    Elf64_Ehdr* header = (Elf64_Ehdr*)elf;

    //get section header table with offset from header
    Elf64_Shdr* shdr_arr = (Elf64_Shdr*)((char*)elf + header->e_shoff);
    //get strings section
    Elf64_Shdr shdr_string_index = shdr_arr[header->e_shstrndx];
    //get strings table
    char* shdr_string_table = (char*)elf + shdr_string_index.sh_offset;
    //get sections number
    Elf64_Half sections_num = header->e_shnum;

    //init vars
    Elf64_Rela* relaplt=NULL;
    int relaplt_size = 0;
    int dynsym_size = 0;
    char* dynstr = NULL;
    Elf64_Sym* dynsym=NULL;
    //iterate over all the sections in the header
    for (int i = 0; i < sections_num; i++)
    {
        /*
        // find rela.plt section header
        if (shdr_arr[i].sh_type == 4) 
        {
            fseek(file_disp, shdr_arr[header->e_shstrndx].sh_offset + shdr_arr[i].sh_name, SEEK_SET);
            char* buff[10];
            fread(&buff, 10, 10, file_disp);
            // ensure second string is also finished
            if (!strcmp(".rela.plt", buff))
            {
                relaplt_shdr = shdr_arr[i];
                relaplt_size = shdr_arr[i].sh_size / sizeof(Elf64_Rela);
            }
        }
        */

        //get section name (string) from string table
        char* section = shdr_string_table + shdr_arr[i].sh_name;
        //is current section the rela plt?
       if (!strcmp(".rela.plt", section) || shdr_arr[i].sh_type == 4)
        {
            relaplt = (Elf64_Rela*)((char*)elf + shdr_arr[i].sh_offset);
            relaplt_size = shdr_arr[i].sh_size / sizeof(Elf64_Rela);
        }
        //is current section the dynsym?
        else if (!strcmp(".dynsym", section) || shdr_arr[i].sh_type == 11)
        {
            //get dynamic symbol table
            dynsym = (Elf64_Sym*)((char*)elf + shdr_arr[i].sh_offset);
            //calculate number of symbols with (overall size / one entire size)
            dynsym_size = shdr_arr[i].sh_size / shdr_arr[i].sh_entsize;
        }
        //is current section the dynstr?
        else if (!strcmp(".dynstr", section) && shdr_arr[i].sh_type == 3)
        {
            if ((char*)elf + shdr_arr[i].sh_offset != shdr_string_table)
            {
                //get string table
                dynstr = ((char*)elf + shdr_arr[i].sh_offset);
            }
        }
    }

    unsigned long int func_index=0;
    //iterate over all stmbols in dynsym
    for (int i = 0; i < dynsym_size; i++)
    {
        //get symbol name
        char* sym = dynstr + dynsym[i].st_name;
        //find our function
        if (!strcmp(symbol_name, sym))
        {
            func_index = i;
        }
    }
    //iterate over all stmbols in rela.plt
    for (int i = 0; i < relaplt_size; i++)
    {
        if (ELF64_R_SYM(relaplt[i].r_info) == func_index)
        {
            close(file_disp);
            return relaplt[i].r_offset;
        }
    }
    close(file_disp);
    return -1;
}

/*
int main(int argc, char* const argv[]) {
    int err = 0;
    unsigned long addr = find_symbol(argv[1], argv[2], &err);

    if (err > 0)
        printf("%s will be loaded to 0x%lx\n", argv[1], addr);
    else if (err == -2)
        printf("%s is not a global symbol! :(\n", argv[1]);
    else if (err == -1)
        printf("%s not found!\n", argv[1]);
    else if (err == -3)
        printf("%s not an executable! :(\n", argv[2]);
    else if (err == -4)
        printf("%s is a global symbol, but will come from a shared library\n", argv[1]);
    return 0;
}
*/
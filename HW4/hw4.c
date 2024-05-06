#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>
#include <signal.h>
#include <syscall.h>
#include <sys/ptrace.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/reg.h>
#include <sys/user.h>
#include <string.h>
#include <unistd.h>
#include <sys/mman.h>
#include <errno.h>
#include <fcntl.h>
#include "elf64.h"
#include "hw3_part1.h"



void run_sys_debugger(pid_t child_pid, unsigned long func_addr, bool is_dyn)
{
    int wait_status;
    struct user_regs_struct regs;
    int cnt = 0;
	
	/* Wait for child to stop on its first instruction */
    wait(&wait_status);
	
    /*if its a dynamic library function, we need to wait for the linker to fetch the address*/
    if (is_dyn)
    {
        /*set a breakpoint at caller to the debugged function*/

        /* Look at the word at the address we're interested in */
		unsigned long plt_entry_addr = ptrace(PTRACE_PEEKTEXT, child_pid, (void*)func_addr, NULL);
        unsigned long data = ptrace(PTRACE_PEEKTEXT, child_pid, (void*)plt_entry_addr, NULL);

        /* Write the trap instruction 'int 3' into the address */
        unsigned long data_trap = (data & 0xFFFFFFFFFFFFFF00) | 0xCC;
        ptrace(PTRACE_POKETEXT, child_pid, (void*)plt_entry_addr, (void*)data_trap);

        /* Let the child run to the breakpoint and wait for it to reach it */
        ptrace(PTRACE_CONT, child_pid, NULL, NULL);

        wait(&wait_status);
        /* See where the child is now */
        ptrace(PTRACE_GETREGS, child_pid, 0, &regs);

        /* Remove the breakpoint by restoring the previous data and save rsp */
        ptrace(PTRACE_POKETEXT, child_pid, (void*)plt_entry_addr, (void*)data);
        regs.rip -= 1;
        unsigned long init_rsp = regs.rsp;
        Elf64_Addr return_address = ptrace(PTRACE_PEEKTEXT, child_pid, init_rsp, NULL);
        ptrace(PTRACE_SETREGS, child_pid, 0, &regs);

        /*set a breakpoint at the return address*/
        unsigned long return_data = ptrace(PTRACE_PEEKTEXT, child_pid, return_address, NULL);
        unsigned long return_data_trap = (return_data & 0xFFFFFFFFFFFFFF00) | 0xCC;
        ptrace(PTRACE_POKETEXT, child_pid, (void*)return_address, (void*)return_data_trap);

        /* Let the child run to the breakpoint and wait for it to reach it */
        ptrace(PTRACE_CONT, child_pid, NULL, NULL);

        wait(&wait_status);
        /* See where the child is now */
        ptrace(PTRACE_GETREGS, child_pid, 0, &regs);

        /* Remove the breakpoint by restoring the previous data */
        ptrace(PTRACE_POKETEXT, child_pid, (void*)return_address, (void*)return_data);
        regs.rip -= 1;
        unsigned long fin_rsp = regs.rsp;
        ptrace(PTRACE_SETREGS, child_pid, 0, &regs);
		
		func_addr = ptrace(PTRACE_PEEKTEXT, child_pid, (void*)func_addr, NULL); //Now linker already ran. Get func addr.

        /*continue until rsp = init_rsp+8*/
        while (!fin_rsp == init_rsp + 8)
        {
            /*make the child execute another instruction*/
            ptrace(PTRACE_SINGLESTEP, child_pid, NULL, NULL);

            /*set a breakpoint at the return address*/
            unsigned long return_data = ptrace(PTRACE_PEEKTEXT, child_pid, return_address, NULL);
            unsigned long return_data_trap = (return_data & 0xFFFFFFFFFFFFFF00) | 0xCC;
            ptrace(PTRACE_POKETEXT, child_pid, (void*)return_address, (void*)return_data_trap);

            /* Let the child run to the breakpoint and wait for it to reach it */
            ptrace(PTRACE_CONT, child_pid, NULL, NULL);

            wait(&wait_status);
            /* See where the child is now */
            ptrace(PTRACE_GETREGS, child_pid, 0, &regs);

            /* Remove the breakpoint by restoring the previous data */
            ptrace(PTRACE_POKETEXT, child_pid, (void*)return_address, (void*)return_data);
            regs.rip -= 1;
            unsigned long fin_rsp = regs.rsp;
            ptrace(PTRACE_SETREGS, child_pid, 0, &regs);
        }

        ptrace(PTRACE_GETREGS, child_pid, 0, &regs);
        cnt++;
        printf("PRF:: run #%d returned with %lld\n", cnt, regs.rax);
    }

    do
    {
        /*set a breakpoint at caller to the debugged function*/

        /* Look at the word at the address we're interested in */
        unsigned long data = ptrace(PTRACE_PEEKTEXT, child_pid, (void*)func_addr, NULL);

        /* Write the trap instruction 'int 3' into the address */
        unsigned long data_trap = (data & 0xFFFFFFFFFFFFFF00) | 0xCC;
        ptrace(PTRACE_POKETEXT, child_pid, (void*)func_addr, (void*)data_trap);

        /* Let the child run to the breakpoint and wait for it to reach it */
        ptrace(PTRACE_CONT, child_pid, NULL, NULL);

        wait(&wait_status);
		
		if (WIFEXITED(wait_status))
		{
			break;
		}
		
        /* See where the child is now */
        ptrace(PTRACE_GETREGS, child_pid, 0, &regs);

        /* Remove the breakpoint by restoring the previous data and save rsp */
        ptrace(PTRACE_POKETEXT, child_pid, (void*)func_addr, (void*)data);
        regs.rip -= 1;
        unsigned long init_rsp = regs.rsp;
        Elf64_Addr return_address = ptrace(PTRACE_PEEKTEXT, child_pid, init_rsp, NULL);
        ptrace(PTRACE_SETREGS, child_pid, 0, &regs);

        /*set a breakpoint at the return address*/
        unsigned long return_data = ptrace(PTRACE_PEEKTEXT, child_pid, (void*)return_address, NULL);
        unsigned long return_data_trap = (return_data & 0xFFFFFFFFFFFFFF00) | 0xCC;
        ptrace(PTRACE_POKETEXT, child_pid, (void*)return_address, (void*)return_data_trap);

        /* Let the child run to the breakpoint and wait for it to reach it */
        ptrace(PTRACE_CONT, child_pid, NULL, NULL);

        wait(&wait_status);
        /* See where the child is now */
        ptrace(PTRACE_GETREGS, child_pid, 0, &regs);

        /* Remove the breakpoint by restoring the previous data */
        ptrace(PTRACE_POKETEXT, child_pid, (void*)return_address, (void*)return_data);
        regs.rip -= 1;
        unsigned long fin_rsp = regs.rsp;
        ptrace(PTRACE_SETREGS, child_pid, 0, &regs);

        /*continue until rsp = init_rsp+8*/
        while (!fin_rsp == init_rsp + 8)
        {
            /*make the child execute another instruction*/
            ptrace(PTRACE_SINGLESTEP, child_pid, NULL, NULL);

            /*set a breakpoint at the return address*/
            unsigned long return_data = ptrace(PTRACE_PEEKTEXT, child_pid, return_address, NULL);
            unsigned long return_data_trap = (return_data & 0xFFFFFFFFFFFFFF00) | 0xCC;
            ptrace(PTRACE_POKETEXT, child_pid, (void*)return_address, (void*)return_data_trap);

            /* Let the child run to the breakpoint and wait for it to reach it */
            ptrace(PTRACE_CONT, child_pid, NULL, NULL);

            wait(&wait_status);
            /* See where the child is now */
            ptrace(PTRACE_GETREGS, child_pid, 0, &regs);

            /* Remove the breakpoint by restoring the previous data */
            ptrace(PTRACE_POKETEXT, child_pid, (void*)return_address, (void*)data);
            regs.rip -= 1;
            unsigned long fin_rsp = regs.rsp;
            ptrace(PTRACE_SETREGS, child_pid, 0, &regs);
        }

        /*now rsp==init_rsp+8*/
        ptrace(PTRACE_GETREGS, child_pid, 0, &regs);
        cnt++;
        printf("PRF:: run #%d returned with %lld\n", cnt, regs.rax);

    } while (!WIFEXITED(wait_status));

}

pid_t run_target(const char* func, char* const argv[]) {
    pid_t pid = fork();

    if (pid > 0) {
        return pid;
    }
    else if (pid == 0) {
        if (ptrace(PTRACE_TRACEME, 0, NULL, NULL) < 0) {
            perror("ptrace");
            exit(1);
        }
        execv(func, (argv + 2));
    }
    else {
        perror("fork");
        exit(1);
    }
}


int main(int argc, char** argv) {
    unsigned int error_val = 0;
    char* func_name = argv[1];
    char* prog_name = argv[2];
    bool is_dyn = false;
    unsigned long addr = find_symbol(func_name, prog_name, &error_val);
    //file is not an executable
    if (error_val == -3)
    {
        printf("PRF:: %s not an executable! :(\n", prog_name);
        return 0;
    }
    //symbol does not exit
    else if (error_val == -1)
    {
        printf("PRF:: %s not found!\n", func_name);
        return 0;
    }
    //symbol exists but is local
    else if (error_val == -2)
    {
        printf("PRF:: %s is not a global symbol! :(\n", func_name);
        return 0;
    }
    //symbol exsits, is global, but is not in the exeutable file (it is in a shred object)
    else if (error_val == -4)
    {
        // stage 5
        addr = get_dyn_adress(func_name, prog_name);
        is_dyn = true;
        //return 0;
    }
    //else error_val == 1 - symbole exists, is global, is in our exe file
    //and addr contains the correct address
    //stage 6 - we have the right address, we want to check when does rsp return to its initial posotion
    
    pid_t child_pid = run_target(prog_name, argv);
    run_sys_debugger(child_pid, addr, is_dyn);
    return 0;
}
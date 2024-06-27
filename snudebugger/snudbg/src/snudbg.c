#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/ptrace.h>
#include <sys/wait.h>
#include <sys/user.h>
#include <unistd.h>
#include <sys/personality.h>

#include "snudbg.h"
#include "procmaps.h"

int num_bps = 0;
breakpoint_t bps[MAX_BPS];

/* HINT: No need to change this function */
void die(char* message) {
    WARN("Failed with message: '%s'\n", message);
    exit(-1);
}

/* HINT: No need to change this function */
void handle_regs(struct user_regs_struct *regs) {
    fprintf(stdout, "\t");
    PRINT_REG(rax);
    PRINT_REG(rbx);
    PRINT_REG(rcx);
    PRINT_REG(rdx);
    fprintf(stdout, "\n");

    fprintf(stdout, "\t");
    PRINT_REG(rbp);
    PRINT_REG(rsp);
    PRINT_REG(rsi);
    PRINT_REG(rdi);
    fprintf(stdout, "\n");

    fprintf(stdout, "\t");
    PRINT_REG(r8);
    PRINT_REG(r9);
    PRINT_REG(r10);
    PRINT_REG(r11);
    fprintf(stdout, "\n");

    fprintf(stdout, "\t");
    PRINT_REG(r12);
    PRINT_REG(r13);
    PRINT_REG(r14);
    PRINT_REG(r15);
    fprintf(stdout, "\n");

    fprintf(stdout, "\t");
    PRINT_REG(rip);
    PRINT_REG(eflags);
    fprintf(stdout, "\n");
}

/* HINT: No need to change this function */
void no_aslr(void) {
    unsigned long pv = PER_LINUX | ADDR_NO_RANDOMIZE;

    if (personality(pv) < 0) {
        if (personality(pv) < 0) {
            die("Failed to disable ASLR");
        }
    }
    return;
}

/* HINT: No need to change this function */
void tracee(char* cmd[]) {
    LOG("Tracee with pid=%d\n", getpid());

    no_aslr();
    
    if(ptrace(PTRACE_TRACEME, NULL, NULL, NULL)<0){
        die("Error traceing myself");
    }

    LOG("Loading the executable [%s]\n", cmd[0]);
    execvp(cmd[0], cmd);
}

/* INSTRUCTION: YOU SHOULD NOT CHANGE THIS FUNCTION */    
void dump_addr_in_hex(const ADDR_T addr, const void* data, size_t size) {
    uint i;
    for (i=0; i<size/16; i++) {
        printf("\t %llx ", addr+(i*16));
        for (uint j=0; j<16; j++) {
            printf("%02x ", ((unsigned char*)data)[i*16+j]);
        }
        printf("\n");
    }

    if (size%16 != 0) {
        // the rest
        printf("\t %llx ", addr+(i*16));
        for (uint j=0; j<size%16; j++) {
            printf("%02x ", ((unsigned char*)data)[i*16+j]);
        }
        printf("\n");
    }
}

/* HINT: No need to change this function */
void handle_help(void) {
    LOG("Available commands: \n");
    LOG("\t regs | get [REG] | set [REG] [value]\n");
    LOG("\t read [addr] [size] | write [addr] [value] [size]\n");
    LOG("\t step | continue | break [addr]\n");
    LOG("\t help\n");
    return;
}

void set_debug_state(int pid, enum debugging_state state) {
    if(state == SINGLE_STEP) {
        if(ptrace(PTRACE_SINGLESTEP, pid, NULL, NULL)<0) {
            die("Error tracing syscalls");
        }
    } else if (state == NON_STOP) {
        // TODO
        if(ptrace(PTRACE_CONT, pid, NULL, NULL)<0) {
            die("Error tracing syscalls");
        }
    }
    return;
}


/* 
   Read the memory from @pid at the address @addr with the length @len.
   The data read from @pid will be written to @buf.
*/
void handle_read(int pid, ADDR_T addr, unsigned char *buf, size_t len) {
    // TODO: Use the function dump_addr_in_hex() to print the memory data
    long val;
    size_t i;
    for(i = 0; i < len; i++){
        val = ptrace(PTRACE_PEEKDATA, pid, addr + i, NULL);
        val = val % 256;
        buf[i] = (char)(val);
    }
    buf[len] = '\0';
    dump_addr_in_hex(addr, buf, len);
    return;
}

/* 
   Write the memory to @pid at the address @addr with the length @len.
   The data to be written is placed in @buf.
*/
void handle_write(int pid, ADDR_T addr, unsigned char *buf, size_t len) {
    // TODO
    //printf("%lx\n", strtoul((char*)buf, NULL, 16));
    size_t padding_len = sizeof(long) - len % sizeof(long);
    long val;
    long num[256];
    for(size_t i = 0; i < padding_len; i++){
        val = ptrace(PTRACE_PEEKDATA, pid, addr + len + i, NULL);
        val = val % 256;
        buf[len + i] = (unsigned char)(val);
    }
    buf[len + padding_len] = '\0';
    for(size_t i = 0; i < len / sizeof(long) + 1; i++){
        num[i] = 0;
        for(int j = sizeof(long) - 1; j >= 0; j--){
            num[i] *= 256;
            num[i] += (long)(buf[i*sizeof(long) + j]);
        }
    }
    for(size_t i = 0; i < len / sizeof(long) + 1; i++){
        if(ptrace(PTRACE_POKEDATA, pid, addr + i * sizeof(long), (long)(num[i])) < 0){
            die("Error tracing syscalls");
        }
    }
    return;
}

/* 
   Install the software breakpoint at @addr to pid @pid.
*/
void handle_break(int pid, ADDR_T addr) {
    // TODO
    if(num_bps >= MAX_BPS) return;
    unsigned long val;
    val = ptrace(PTRACE_PEEKDATA, pid, addr, NULL);
    unsigned long orig_value = val % 256;
    bps[num_bps].addr = addr;
    bps[num_bps].orig_value = (unsigned char) orig_value;
    num_bps++;
    val = val - orig_value + 0xcc; //0xcc
    if((ptrace(PTRACE_POKEDATA, pid, addr, val)) < 0){
        die("Error tracing syscalls");
    }
}

#define CMPGET_REG(REG_TO_CMP)                   \
    if (strcmp(reg_name, #REG_TO_CMP)==0) {      \
        printf("\t");                            \
        PRINT_REG(REG_TO_CMP);                   \
        printf("\n");                            \
    }

/* HINT: No need to change this function */
void handle_get(char *reg_name, struct user_regs_struct *regs) {
    CMPGET_REG(rax); CMPGET_REG(rbx); CMPGET_REG(rcx); CMPGET_REG(rdx);
    CMPGET_REG(rbp); CMPGET_REG(rsp); CMPGET_REG(rsi); CMPGET_REG(rdi);
    CMPGET_REG(r8);  CMPGET_REG(r9);  CMPGET_REG(r10); CMPGET_REG(r11);
    CMPGET_REG(r12); CMPGET_REG(r13); CMPGET_REG(r14); CMPGET_REG(r15);
    CMPGET_REG(rip); CMPGET_REG(eflags);
    return;
}


/*
  Set the register @reg_name with the value @value.
  @regs is assumed to be holding the current register values of @pid.
*/

#define CMPSET_REG(REG_TO_CMP, VAL)             \
    if (strcmp(reg_name, #REG_TO_CMP)==0){      \
        regs->REG_TO_CMP = VAL;                 \
    }

void handle_set(char *reg_name, unsigned long value,
                struct user_regs_struct *regs, int pid) {
    // TODO
    CMPSET_REG(rax, value); CMPSET_REG(rbx, value); CMPSET_REG(rcx, value); CMPSET_REG(rdx, value);
    CMPSET_REG(rbp, value); CMPSET_REG(rsp, value); CMPSET_REG(rsi, value); CMPSET_REG(rdi, value);
    CMPSET_REG(r8, value);  CMPSET_REG(r9, value);  CMPSET_REG(r10, value); CMPSET_REG(r11, value);
    CMPSET_REG(r12, value); CMPSET_REG(r13, value); CMPSET_REG(r14, value); CMPSET_REG(r15, value);
    CMPSET_REG(rip, value); CMPSET_REG(eflags, value);
    set_registers(pid, regs);
    return;
}

void prompt_user(int child_pid, struct user_regs_struct *regs,
                 ADDR_T baseaddr) {

    const char* prompt_symbol = ">>> ";

    for(;;) {
        fprintf(stdout, "%s", prompt_symbol);
        char action[1024];
        scanf("%1024s", action);
        if(strcmp("regs", action)==0) {
            LOG("HANDLE CMD: regs\n");
            handle_regs(regs);
            continue;
        }

        if(strcmp("help", action)==0 || strcmp("h", action)==0) {
            handle_help();
            continue;
        }

        if(strcmp("get", action)==0) {
            // TODO
            char argv[1024];
            scanf("%1024s", argv);
            LOG("HANDLE CMD: get [%s]\n", argv);
            handle_get(argv, regs);
            continue;
        }

        if(strcmp("set", action)==0) {
            char argv[1024];
            char val[1024];
            scanf("%1024s", argv); scanf("%1024s", val);
            // TODO
            LOG("HANDLE CMD: set [%s] [%llx]\n", argv, strtoull(val, NULL, 16));
            handle_set(argv, strtoul(val, NULL, 16), regs, child_pid);
            continue;
        }

        if(strcmp("read", action)==0 || strcmp("r", action)==0) {
            // TODO
            char addr[1024];
            char size[1024];
            unsigned char b[1024];
            scanf("%1024s", addr); scanf("%1024s", size);
            LOG("HANDLE CMD: read [%llx][%llx] [%lx]\n", strtoull(addr, NULL, 16), baseaddr+strtoull(addr, NULL, 16), strtoul(size, NULL, 16));
            handle_read(child_pid, baseaddr+strtoull(addr, NULL, 16), b, strtoul(size, NULL, 16));
            continue;
        }

        if(strcmp("write", action)==0 || strcmp("w", action)==0) {
            // TODO
            char addr[1024];
            char val[1024];
            char size[1024];
            //char a[1] = "0";
            scanf("%1024s", addr); scanf("%1024s", val); scanf("%1024s", size);
            char *ptr = val;
            if(val[0] == '0' && val[1] == 'x')
                ptr = ptr+2;
            if(strlen(ptr) % 2)
                {ptr -= 1; ptr[0] = '0';}
            size_t len = strtoul(size, NULL, 16);
            LOG("HANDLE CMD: write [%llx][%llx] [%s]<= %#lx\n", strtoull(addr, NULL, 16), baseaddr+strtoull(addr, NULL, 16), ptr, len);
            
            unsigned char buf[1024];
            char st[2];
            for (size_t i = 0; i < len; i++){
                strncpy(st, ptr + 2*(len - i - 1), 2);
                long val1 = strtol(st, NULL, 16);
                buf[i] = (unsigned char) val1;
            }
            buf[len] = '\0';
            handle_write(child_pid, baseaddr+strtoull(addr, NULL, 16), buf, strtoul(size, NULL, 16));
            
            continue;
        }

        if(strcmp("break", action)==0 || strcmp("b", action)==0) {
            // TODO
            char addr[1024];
            scanf("%1024s", addr);
            LOG("HANDLE CMD: break [%llx][%llx]\n", strtoull(addr, NULL, 16), baseaddr+strtoull(addr, NULL, 16));
            handle_break(child_pid, baseaddr+strtoull(addr, NULL, 16));
            continue;
        }

        if(strcmp("step", action)==0 || strcmp("s", action)==0) {
            // TODO
            LOG("HANDLE CMD: step\n");
            set_debug_state(child_pid, SINGLE_STEP);
            break;
        }

        if(strcmp("continue", action)==0 || strcmp("c", action)==0) {
            // TODO
            LOG("HANDLE CMD: continue\n");
            set_debug_state(child_pid, NON_STOP);
            break;
        }

        if(strcmp("quit", action)==0 || strcmp("q", action)==0) {
            LOG("HANDLE CMD: quit\n");
            exit(0);
        }

        WARN("Not available commands\n");
    }
}


/*
  Get the current registers of @pid, and store it to @regs.
*/
void get_registers(int pid, struct user_regs_struct *regs) {
    if(ptrace(PTRACE_GETREGS, pid, NULL, regs)<0) {
        die("Error getting registers");
    }
    return;
}


/*
  Set the registers of @pid with @regs.
*/
void set_registers(int pid, struct user_regs_struct *regs) {
    // TODO
    if(ptrace(PTRACE_SETREGS, pid, NULL, regs)<0) {
        die("Error setting registers");
    }
    return;
}


/*
  Get the base address of the main binary image, 
  loaded to the process @pid.
  This base address is the virtual address.
*/
ADDR_T get_image_baseaddr(int pid) {
    hr_procmaps** procmap = construct_procmaps(pid);
    ADDR_T baseaddr = 0;
    // TODO
    baseaddr = procmap[0]->addr_begin;
    return baseaddr;
}

/*
  Perform the job if the software breakpoint is fired.
  This includes to restore the original value at the breakpoint address.
*/
void handle_break_post(int pid, struct user_regs_struct *regs) {
    // TODO
    if(num_bps == 0)
        return;
    ADDR_T addr; long orig_value; size_t flag = 0;
    for(int i = 0; i < num_bps; i++){
        if(flag) {bps[i-1].addr = bps[i].addr; bps[i-1].orig_value = bps[i].orig_value;}
        else if(regs->rip == bps[i].addr+1){
            addr = bps[i].addr;
            orig_value = (unsigned long)(bps[i].orig_value);
            flag = 1;
            regs->rip = bps[i].addr;
            set_registers(pid, regs);
        }
    }
    if(!flag) return;
    num_bps--;
    LOG("\tFOUND MATCH BP: [%d] [%llx][%lx]\n", 0, addr, orig_value);
    
    unsigned long val;
    
    val = ptrace(PTRACE_PEEKDATA, pid, addr, NULL);
    val = val - val%256 + orig_value;
    if((ptrace(PTRACE_POKEDATA, pid, addr, val)) < 0){
        die("Error tracing syscalls");
    }
}


/* HINT: No need to change this function */
void tracer(int child_pid) {
    int child_status;

    LOG("Tracer with pid=%d\n", getpid());

    wait(&child_status);

    ADDR_T baseaddr = get_image_baseaddr(child_pid);

    int steps_count = 0;
    struct user_regs_struct tracee_regs;
    set_debug_state(child_pid, SINGLE_STEP);

    while(1) {
        wait(&child_status);
        steps_count += 1;

        if(WIFEXITED(child_status)) {
            LOG("Exited in %d steps with status=%d\n",
                steps_count, child_status);
            break;
        }
        get_registers(child_pid, &tracee_regs);

        LOG("[step %d] rip=%llx child_status=%d\n", steps_count,
            tracee_regs.rip, child_status);

        handle_break_post(child_pid, &tracee_regs);
        prompt_user(child_pid, &tracee_regs, baseaddr);
    }
}

/* HINT: No need to change this function */
int main(int argc, char* argv[]) {
    char* usage = "USAGE: ./snudbg <cmd>";

    if (argc < 2){
        die(usage);
    }

    int pid = fork();

    switch (pid) {
    case -1:
        die("Error forking");
        break;
    case 0:
        tracee(argv+1);
        break;
    default:
        tracer(pid);
        break;
    }
    return 0;
}

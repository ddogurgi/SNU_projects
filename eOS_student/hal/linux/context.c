#include <core/eos.h>
#include <core/eos_internal.h>
#include "emulator_asm.h"

typedef struct _os_context {
	/* low address */
	int32u_t oldebp;
	int32u_t oldeip;
	int32u_t edi;
	int32u_t esi;
	int32u_t ebx;
	int32u_t edx;
	int32u_t ecx;
	int32u_t eax;
	int32u_t _eflags;
	int32u_t resume_point;
	/* high address */
} _os_context_t;

void print_context(addr_t context) {
	if(context == NULL) return;
	_os_context_t *ctx = (_os_context_t *)context;
	//PRINT("reg1  =0x%x\n", ctx->reg1);
	//PRINT("reg2  =0x%x\n", ctx->reg2);
	//PRINT("reg3  =0x%x\n", ctx->reg3);
	//...
}

addr_t _os_create_context(addr_t stack_base, size_t stack_size, void (*entry)(void *), void *arg) {
	size_t *stack_end = (size_t*)((int8u_t*)stack_base + stack_size - 1); //end of stack(high address)
	*stack_end = (int32u_t)arg;
	*(stack_end - 1) = NULL;
	*(stack_end - 2) = (int32u_t)entry;
	*(stack_end - 3) = 1;
	for (int i = 4; i <= 11; i++)
		*(stack_end - i) = NULL;
	return (addr_t)(stack_end - 11);
}

void _os_restore_context(addr_t sp) {
	//popa : pop eax, ecx, edx, ebx, esp, ebp, esi, edi
	//ret : pop %eip, jmp %eip -> jumps to resume_point
	__asm__ __volatile__("\
		movl %0, %%esp;\
		popa;\
		pop _eflags;\
		ret;\
		"
		::"m"(sp));
}

addr_t _os_save_context() {
	//pusha : push eax, ecx, edx, ebx, esp, ebp, esi, edi
	//leave : movl %ebp %esp, pop %ebp
	//ret : pop %eip, jmp %eip
	//colon (:) : Label - place to jump
	//when restore context, jumps to resume_point:
	__asm__ __volatile__("\
		push $resume_point;\
		push _eflags;\
		pusha;\
		movl $0, -12(%ebp);\
		movl %esp, %eax;\
		push 4(%ebp);\
		push (%ebp);\
		movl %esp, %ebp;\
		leave;\
		ret;\
		resume_point:\
		");
	return NULL;
}

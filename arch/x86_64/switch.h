/*
 * Kernel context switching.
 */
#ifndef X86_64_SWITCH_H
#define X86_64_SWITCH_H

#include <stdint.h>
#include <asm/mm.h>

struct process;

struct context {
	/* stack frame */
	virt_addr_t sp;
	//virt_addr_t rbp; // not necessary

	int intena;
	int noff;

	/* general purpose registers (not callee-saved between functions) */
	uint64_t rbx;
	uint64_t r12;
	uint64_t r13;
	uint64_t r14;
	uint64_t r15;

	// ffu, mmx, see registers etc. (not currently used)
};

// old_sp points to where we should save the outgoing stack pointer, so it
// itself has to be a pointer. defined in asm/swtch.S
void swtch(virt_addr_t *old_sp, virt_addr_t new_sp, phys_addr_t new_cr3);

void ctx_switch(struct process *new);

#endif

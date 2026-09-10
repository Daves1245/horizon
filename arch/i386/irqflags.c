#include <stdint.h>
#include <arch/i386/irqflags.h>

uint32_t read_eflags(void) {
	uint32_t flags;
	asm volatile("pushfl; popl %0" : "=r"(flags));
	return flags;
}

// the 'memory' clobber keeps the compiler from moving loads or stores
// across the cli/sti — without it a write inside the critical section is
// free to sink below the sti. see arch/x86_64/irqflags.c for the longer
// note on why this is a compiler barrier and not a cpu one.
uint32_t irq_save(void) {
	uint32_t flags = read_eflags() & FL_IF;

	asm volatile("cli" ::: "memory");
	return flags;
}

void irq_restore(uint32_t flags) {
	if (flags & FL_IF) {
		asm volatile("sti" ::: "memory");
	}
}

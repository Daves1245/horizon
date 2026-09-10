#include <stdio.h>
#include <stdint.h>
#include <kernel/tty.h>
#include <halt.h>
#include "test_vm.h"
#include "memory/paging.h"

void test_vm(void) {
#ifdef DEBUG
	log_info("Advanced VM Tests\n");
#endif

	// Test 1: Non-identity mapping (virt != phys)
	uint32_t test_virt = 0xA0000000;
	uint32_t test_phys = 0x500000; // 5MB physical

	map_page(test_virt, test_phys, 1, 1, read_cr3());

	// Write to virtual address
	*(volatile uint32_t *)test_virt = 0xDEADBEEF;

	// Read from physical address (identity mapped)
	uint32_t phys_value = *(volatile uint32_t *)test_phys;

	if (phys_value == 0xDEADBEEF) {
#ifdef DEBUG
		log_success("[vm_test]: Test 1\n");
#endif
	} else {
		log_error("[vm_test]: Test 1 FAILED");
		printf(" - got 0x%x, expected 0xDEADBEEF\n", phys_value);
		halt();
	}

	// Test 2: Remapping same virtual address
	unmap_page(test_virt, read_cr3());

	uint32_t test_phys2 = 0x600000; // Different physical address
	map_page(test_virt, test_phys2, 1, 1, read_cr3());

	*(volatile uint32_t *)test_virt = 0xCAFEBABE;
	uint32_t phys2_value = *(volatile uint32_t *)test_phys2;

	if (phys2_value == 0xCAFEBABE) {
#ifdef DEBUG
		log_success("[vm_test]: Test 2 PASSED\n");
#endif
	} else {
		log_error("[vm_test]: Test 2 FAILED\n");
		halt();
	}

	// Test 3: is_page_mapped check
	if (is_page_mapped(test_virt, read_cr3()) &&
	    !is_page_mapped(0xC0000000, read_cr3())) {
#ifdef DEBUG
		log_success("[vm_test]: Test 3 PASSED\n");
#endif
	} else {
		log_error("[vm_test]: Test 3 FAILED\n");
		halt();
	}

	log_success("[vm_test]: all tests passed\n");
}

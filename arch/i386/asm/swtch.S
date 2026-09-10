# swtch(virt_addr_t *old->sp, virt_addr_t new->sp, phys_addr_t new->cr3)
# i386 cdecl passes everything on the stack, so read the three arguments out
# before the pushes below move %esp out from under them.
.global swtch
swtch:
    mov 4(%esp), %eax  # &old->context.sp
    mov 8(%esp), %edx  # new->context.sp
    mov 12(%esp), %ecx # new->cr3

    # save old stack. eax/ecx/edx are caller-saved, so the four below are
    # the whole callee-saved set the i386 System V ABI asks us to carry
    push %ebp
    push %ebx
    push %esi
    push %edi

    mov %esp, (%eax)
    mov %edx, %esp # load new stack
    mov %ecx, %cr3

    # TODO(multicore) insert TLB flush here

    pop %edi
    pop %esi
    pop %ebx
    pop %ebp
    ret # the instruction where it happens

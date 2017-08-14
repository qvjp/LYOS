
.set IRQ_BASE, 0x20

.section .text

.extern _ZN4lyos21hardwarecommunication16InterruptManager15HandleInterruptEhj

.macro HandleException num
.global _ZN4lyos21hardwarecommunication16InterruptManager19HandleException\num\()Ev
_ZN4lyos21hardwarecommunication16InterruptManager19HandleException\num\()Ev:
    movb $\num, (interruptnumber)
    jmp int_bottom
.endm

.macro HandleInterruptRequest num
.global _ZN4lyos21hardwarecommunication16InterruptManager26HandleInterruptRequest\num\()Ev
_ZN4lyos21hardwarecommunication16InterruptManager26HandleInterruptRequest\num\()Ev:
    movb $\num + IRQ_BASE, (interruptnumber)
    jmp int_bottom
.endm

HandleInterruptRequest 0x00 
HandleInterruptRequest 0x01 

HandleException 0x00
HandleException 0x01

int_bottom:

    pusha       #全部16位通用寄存器入栈
    pushl %ds
    pushl %es
    pushl %fs
    pushl %gs


    pushl %esp
    push (interruptnumber)
    call _ZN4lyos21hardwarecommunication16InterruptManager15HandleInterruptEhj
    # addl %5, %esp
    movl %eax, %esp

    popl %gs
    popl %fs
    popl %es
    popl %ds
    popa

.global _ZN4lyos21hardwarecommunication16InterruptManager22IgnoreInterruptRequestEv
_ZN4lyos21hardwarecommunication16InterruptManager22IgnoreInterruptRequestEv:
    iret

.data
    interruptnumber: .byte 0
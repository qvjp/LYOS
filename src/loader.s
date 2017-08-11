/* 声明多重引导头常数. */
.set ALIGN,    1<<0             /* align loaded modules on page boundaries */
.set MEMINFO,  1<<1             /* provide memory map */
.set FLAGS,    ALIGN | MEMINFO  /* this is the Multiboot 'flag' field */
.set MAGIC,    0x1BADB002       /* 'magic number' lets bootloader find the header */
.set CHECKSUM, -(MAGIC + FLAGS) /* checksum of above, to prove we are multiboot */

.section .text
	.long MAGIC
	.long FLAGS
	.long CHECKSUM

.section .text
.extern kernelMain
.extern callConstructors
.global loader					/*入口*/

loader:
	mov $kernel_stack, %esp
	call callConstructors
	push %eax
	push %ebx
	call kernelMain				/*入口里调用c++的kernelMail函数*/

_stop:
	cli
	hlt
	jmp _stop					/*循环*/

.section .bss
.space 2*1024*1024 /*2M内核空间*/
kernel_stack:

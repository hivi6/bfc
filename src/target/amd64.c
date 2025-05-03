#include "../utils.h"

void gen_preamble()
{
	assemble(".section .data");
	assemble("tape:");
	assemble("	.zero 30000");
	assemble("pointer:");
	assemble("	.quad 0");
	assemble("");
	assemble(".section .text");
	assemble(".global _start");
	assemble("");
	assemble("_start:");
}

void gen_pointer_inc()
{
	assemble("	# > : pointer increase");
	assemble("	incq pointer");
	assemble("");
}

void gen_pointer_dec()
{
	assemble("	# < : pointer decrease");
	assemble("	decq pointer");
	assemble("");
}

void gen_value_inc()
{
	assemble("	# + : value increase");
	assemble("	mov pointer, %rax");
	assemble("	incb tape(%rax)");
	assemble("");
}

void gen_value_dec()
{
	assemble("	# - : value decrease");
	assemble("	mov pointer, %rax");
	assemble("	decb tape(%rax)");
	assemble("");
}

void gen_while_start(int label)
{
	assemble("	# [ : while start");
	assemble("	movq pointer, %rcx");
	assemble("	movzbq tape(%rcx), %rax");
	assemble("	test %%al, %%al");
	assemble("	je EXIT_%d", label);
	assemble("ENTER_%d:", label);
	assemble("");
}

void gen_while_stop(int label)
{
	assemble("	# ] : while stop");
	assemble("	movq pointer, %rcx");
	assemble("	movzbq tape(%rcx), %rax");
	assemble("	test %%al, %%al");
	assemble("	jne ENTER_%d", label);
	assemble("EXIT_%d:", label);
	assemble("");
}

void gen_read()
{
	assemble("	# , : read a character");
	assemble("	movq pointer, %rcx");
	assemble("	lea tape(%rcx), %rsi");
	assemble("	mov $0, %rdi");
	assemble("	mov $1, %rdx");
	assemble("	mov $0, %rax");
	assemble("	syscall");
	assemble("");
}

void gen_write()
{
	assemble("	# . : write a character");
	assemble("	movq pointer, %rcx");
	assemble("	lea tape(%rcx), %rsi");
	assemble("	mov $1, %rdi");
	assemble("	mov $1, %rdx");
	assemble("	mov $1, %rax");
	assemble("	syscall");
	assemble("");
}

void gen_epilogue()
{
	assemble("	mov $60, %rax");
	assemble("	xor %rdi, %rdi");
	assemble("	syscall");
}

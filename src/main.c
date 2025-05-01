#include <stdio.h>
#include <stdlib.h>

void gen_preamble()
{
	printf(".section .data\n");
	printf("tape:\n");
	printf("	.zero 30000\n");
	printf("pointer:\n");
	printf("	.quad 0\n");
	printf("\n");
	printf(".section .text\n");
	printf(".global _start");
	printf("\n");
	printf("_start:\n");
}

void gen_pointer_inc()
{
	printf("	# > : pointer increase\n");
	printf("	incq pointer\n");
	printf("\n");
}

void gen_pointer_dec()
{
	printf("	# < : pointer decrease\n");
	printf("	decq pointer\n");
	printf("\n");
}

void gen_value_inc()
{
	printf("	# + : value increase\n");
	printf("	mov pointer, %rax\n");
	printf("	incb tape(%rax)\n");
	printf("\n");
}

void gen_value_dec()
{
	printf("	# - : value decrease\n");
	printf("	mov pointer, %rax\n");
	printf("	decb tape(%rax)\n");
	printf("\n");
}

void gen_while_start(int label)
{
	printf("	# [ : while start\n");
	printf("	movq pointer, %rcx\n");
	printf("	movzbq tape(%rcx), %rax\n");
	printf("	test %%al, %%al\n");
	printf("	je EXIT_%d\n", label);
	printf("ENTER_%d:\n", label);
	printf("\n");
}

void gen_while_stop(int label)
{
	printf("	# ] : while stop\n");
	printf("	movq pointer, %rcx\n");
	printf("	movzbq tape(%rcx), %rax\n");
	printf("	test %%al, %%al\n");
	printf("	jne ENTER_%d\n", label);
	printf("EXIT_%d:\n", label);
	printf("\n");
}

void gen_read()
{
	printf("	# , : read a character\n");
	printf("	movq pointer, %rcx\n");
	printf("	lea tape(%rcx), %rsi\n");
	printf("	mov $0, %rdi\n");
	printf("	mov $1, %rdx\n");
	printf("	mov $0, %rax\n");
	printf("	syscall\n");
	printf("\n");
}

void gen_write()
{
	printf("	# . : write a character\n");
	printf("	movq pointer, %rcx\n");
	printf("	lea tape(%rcx), %rsi\n");
	printf("	mov $1, %rdi\n");
	printf("	mov $1, %rdx\n");
	printf("	mov $1, %rax\n");
	printf("	syscall\n");
	printf("\n");
}

void gen_epilogue()
{
	printf("	mov $60, %rax\n");
	printf("	xor %rdi, %rdi\n");
	printf("	syscall\n");
}

int POINTER = 0;
int LABEL = 0;
void gen_asm(const char *program)
{
	while (program[POINTER])
	{
		switch (program[POINTER])
		{
		case '>':
			gen_pointer_inc();
			break;
		case '<':
			gen_pointer_dec();
			break;
		case '+':
			gen_value_inc();
			break;
		case '-':
			gen_value_dec();
			break;
		case '[':
		{
			int temp = LABEL;
			gen_while_start(temp);
			POINTER++;
			LABEL++;
			gen_asm(program);
			gen_while_stop(temp);
			break;
		}
		case ']':
			return;
		case ',':
			gen_read();
			break;
		case '.':
			gen_write();
			break;
		default:
		}
		POINTER++;
	}
}

int main()
{

	int label = 0;
	// prints "Hello, World!"
	const char *program = 
		">++++++++[<+++++++++>-]"
		"<.>++++[<+++++++>-]<+.+"
		"++++++..+++.>>++++++[<+"
		"++++++>-]<++.----------"
		"--.>++++++[<+++++++++>-"
		"]<+.<.+++.------.------"
		"--.>>>++++[<++++++++>-]<+.";

	gen_preamble();
	gen_asm(program);
	gen_epilogue();

	return 0;
}

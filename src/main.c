#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>

FILE *OUTPUT;
void assemble(const char *fmt, ...)
{
	va_list args;
	va_start(args, fmt);
	vfprintf(OUTPUT, fmt, args);
	fprintf(OUTPUT, "\n");
	va_end(args);
}

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

int main(int argc, char **argv)
{
	if (argc <= 1)
	{
		fprintf(stderr, "Error: no program file provided\n");
		exit(1);
	}

	char *output_file = "a.out";
	for (int i = 1; i < argc - 1; i++)
	{
		if (strcmp(argv[i], "-o") == 0)
		{
			if (i + 1 < argc)
			{
				output_file = argv[++i];
			}
			else
			{
				fprintf(stderr, "Error: expected filename after -o\n");
				exit(1);
			}
		}
		else
		{
			fprintf(stderr, "Error: unknown option '%s'\n", argv[i]);
			exit(1);
		}
	}

	// Get the input file
	FILE *fd = fopen(argv[argc - 1], "r");
	if (fd == NULL)
	{
		fprintf(stderr, "Error: something went wrong with opening the file\n");
		exit(1);
	}

	// Read the content of the program to a character array
	fseek(fd, 0, SEEK_END);
	long fd_size = ftell(fd);
	fseek(fd, 0, SEEK_SET);
	char *program = malloc(fd_size + 1);
	int n = 0;
	while (1)
	{
		char ch = fgetc(fd);
		if (ch == EOF) break;
		program[n++] = ch;
	}
	program[n] = 0;
	fclose(fd);

	// generate the assembly
	OUTPUT = fopen("/tmp/bfc.s", "w");
	gen_preamble();
	gen_asm(program);
	gen_epilogue();
	fclose(OUTPUT);

	free(program);

	// generate the .o file
	system("as -o /tmp/bfc.o /tmp/bfc.s");

	// generate the executable file
	char cmd[1024];
	sprintf(cmd, "ld -o %s /tmp/bfc.o", output_file);
	system(cmd);

	return 0;
}

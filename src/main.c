#include <stdio.h>
#include <stdlib.h>
#include <string.h>

FILE *OUTPUT;
void gen_preamble()
{
	fprintf(OUTPUT, ".section .data\n");
	fprintf(OUTPUT, "tape:\n");
	fprintf(OUTPUT, "	.zero 30000\n");
	fprintf(OUTPUT, "pointer:\n");
	fprintf(OUTPUT, "	.quad 0\n");
	fprintf(OUTPUT, "\n");
	fprintf(OUTPUT, ".section .text\n");
	fprintf(OUTPUT, ".global _start");
	fprintf(OUTPUT, "\n");
	fprintf(OUTPUT, "_start:\n");
}

void gen_pointer_inc()
{
	fprintf(OUTPUT, "	# > : pointer increase\n");
	fprintf(OUTPUT, "	incq pointer\n");
	fprintf(OUTPUT, "\n");
}

void gen_pointer_dec()
{
	fprintf(OUTPUT, "	# < : pointer decrease\n");
	fprintf(OUTPUT, "	decq pointer\n");
	fprintf(OUTPUT, "\n");
}

void gen_value_inc()
{
	fprintf(OUTPUT, "	# + : value increase\n");
	fprintf(OUTPUT, "	mov pointer, %rax\n");
	fprintf(OUTPUT, "	incb tape(%rax)\n");
	fprintf(OUTPUT, "\n");
}

void gen_value_dec()
{
	fprintf(OUTPUT, "	# - : value decrease\n");
	fprintf(OUTPUT, "	mov pointer, %rax\n");
	fprintf(OUTPUT, "	decb tape(%rax)\n");
	fprintf(OUTPUT, "\n");
}

void gen_while_start(int label)
{
	fprintf(OUTPUT, "	# [ : while start\n");
	fprintf(OUTPUT, "	movq pointer, %rcx\n");
	fprintf(OUTPUT, "	movzbq tape(%rcx), %rax\n");
	fprintf(OUTPUT, "	test %%al, %%al\n");
	fprintf(OUTPUT, "	je EXIT_%d\n", label);
	fprintf(OUTPUT, "ENTER_%d:\n", label);
	fprintf(OUTPUT, "\n");
}

void gen_while_stop(int label)
{
	fprintf(OUTPUT, "	# ] : while stop\n");
	fprintf(OUTPUT, "	movq pointer, %rcx\n");
	fprintf(OUTPUT, "	movzbq tape(%rcx), %rax\n");
	fprintf(OUTPUT, "	test %%al, %%al\n");
	fprintf(OUTPUT, "	jne ENTER_%d\n", label);
	fprintf(OUTPUT, "EXIT_%d:\n", label);
	fprintf(OUTPUT, "\n");
}

void gen_read()
{
	fprintf(OUTPUT, "	# , : read a character\n");
	fprintf(OUTPUT, "	movq pointer, %rcx\n");
	fprintf(OUTPUT, "	lea tape(%rcx), %rsi\n");
	fprintf(OUTPUT, "	mov $0, %rdi\n");
	fprintf(OUTPUT, "	mov $1, %rdx\n");
	fprintf(OUTPUT, "	mov $0, %rax\n");
	fprintf(OUTPUT, "	syscall\n");
	fprintf(OUTPUT, "\n");
}

void gen_write()
{
	fprintf(OUTPUT, "	# . : write a character\n");
	fprintf(OUTPUT, "	movq pointer, %rcx\n");
	fprintf(OUTPUT, "	lea tape(%rcx), %rsi\n");
	fprintf(OUTPUT, "	mov $1, %rdi\n");
	fprintf(OUTPUT, "	mov $1, %rdx\n");
	fprintf(OUTPUT, "	mov $1, %rax\n");
	fprintf(OUTPUT, "	syscall\n");
	fprintf(OUTPUT, "\n");
}

void gen_epilogue()
{
	fprintf(OUTPUT, "	mov $60, %rax\n");
	fprintf(OUTPUT, "	xor %rdi, %rdi\n");
	fprintf(OUTPUT, "	syscall\n");
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

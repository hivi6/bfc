#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>

#include "utils.h"

static FILE *OUTPUT;

void load_assembly_file(FILE *fd)
{
	OUTPUT = fd;
}

void assemble(const char *fmt, ...)
{
	va_list args;
	va_start(args, fmt);
	vfprintf(OUTPUT, fmt, args);
	fprintf(OUTPUT, "\n");
	va_end(args);
}


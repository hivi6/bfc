#ifndef UTILS_H
#define UTILS_H

#include <stdio.h>

// load the final assembly file directory
void load_assembly_file(FILE *fd);

// print to the assembly file directory
// using printf syntax
void assemble(const char *fmt, ...);

#endif // UTILS_H

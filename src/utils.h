#ifndef UTILS_H
#define UTILS_H

#include <stdio.h>

/**
 * about:
 *		load the file where the final assembly is generated
 */
void load_assembly_file(FILE *fd);

/**
 * about:
 *		uses fprintf under the hood to print the assembly statement
 *		to the loaded assembly file, used by the gen.h functions
 */
void assemble(const char *fmt, ...);

#endif // UTILS_H

#ifndef GEN_H
#define GEN_H

/**
 * about:
 *     generates the preamble, i.e. start of the assembly code
 */
void gen_preamble();

/**
 * about:
 *		generates assembly for > token in brain fuck
 */
void gen_pointer_inc();

/**
 * about:
 *		generates assembly for < token in brain fuck
 */
void gen_pointer_dec();

/**
 * about:
 *		generates assembly for + token in brain fuck
 */
void gen_value_inc();

/**
 * about:
 *		generates assembly for - token in brain fuck
 */
void gen_value_dec();

/**
 * about:
 *		generates assembly for [ token in brain fuck
 *
 * arguments
 *		label:	prints the start label of the while loop with the given
 *				label number
 */
void gen_while_start(int label);

/**
 * about:
 *		generates assembly for ] token in brain fuck
 *
 * arguments
 *		label:	prints the end label of the while loop with the given
 *				label number
 */
void gen_while_stop(int label);

/**
 * about:
 *		generates assembly for . token in brain fuck
 */
void gen_read();

/**
 * about:
 *		generates assembly for , token in brain fuck
 */
void gen_write();

/**
 * about:
 *     generates the epilogue, i.e. end of the assembly code
 */
void gen_epilogue();

#endif // GEN_H

#ifndef GEN_H
#define GEN_H

void gen_preamble();
void gen_pointer_inc();
void gen_pointer_dec();
void gen_value_inc();
void gen_value_dec();
void gen_while_start(int label);
void gen_while_stop(int label);
void gen_read();
void gen_write();
void gen_epilogue();

#endif // GEN_H

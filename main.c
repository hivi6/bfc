#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "ap.h"

// utility function for reading from a file or stdin(if name == NULL)
char *read_file(const char *name);

// opcode for brain fuck
enum bf_code {
  OP_MOVE_RIGHT    = '>',
  OP_MOVE_LEFT     = '<',
  OP_INCREMENT     = '+',
  OP_DECREMENT     = '-',
  OP_OUTPUT        = '.',
  OP_INPUT         = ',',
  OP_JUMP_FORWARD  = '[',
  OP_JUMP_BACKWARD = ']',
};

// insttruction array
struct bf_instructions {
  int count;
  int capacity;
  enum bf_code *items;
};

// bf_instructions functions
struct bf_instructions *bf_init();
int bf_delete(struct bf_instructions *instructions);
int bf_append(struct bf_instructions *instructions, enum bf_code code);
int bf_parse(const char *program, struct bf_instructions *instructions);
int bf_match_brackets(struct bf_instructions *instructions);

// generating assembly stuff
void generate_assembly_move_right(FILE* file);
void generate_assembly_move_left(FILE* file);
void generate_assembly_increment(FILE* file);
void generate_assembly_decrement(FILE* file);
void generate_assembly_output(FILE* file);
void generate_assembly_input(FILE* file);
void generate_assembly_jump_forward(FILE* file, int label);
void generate_assembly_jump_backward(FILE* file, int label);
void generate_assembly(const char* name, struct bf_instructions *instructions);

int main(int argc, const char **argv) {
  struct ap_value *help = ap_value_init(AP_FLAG, "show help", "-h", "--help");
  struct ap_value *verbose = ap_value_init(AP_FLAG, "verbose", "-v", "--verbose");
  struct ap_value *input = ap_value_init(AP_FVALUE, "input file", "-f", "--input");
  struct ap_value *output = ap_value_init(AP_FVALUE, "output file", "-o", "--output");

  struct ap_parser *parser = ap_parser_init("bfc", "Brain Fuck Compiler");

  ap_parser_add_argument(parser, help);
  ap_parser_add_argument(parser, verbose);
  ap_parser_add_argument(parser, input);
  ap_parser_add_argument(parser, output);

  int err = ap_parser_parse(parser, argc, argv);
  if (err) {
    printf("Something went wrong while parsing\n");
    ap_parser_help(parser);
    exit(1);
  }

  if (help->is_exists) {
    ap_parser_help(parser);
    exit(1);
  }

  if (verbose->is_exists) {
    printf("help: %d\n", help->is_exists);
    printf("verbose: %d\n", verbose->is_exists);
    printf("input-file: %s\n", input->value == NULL ? "stdin" : input->value);
    printf("output-file: %s\n", output->value == NULL ? "stdout" : output->value);
  }

  char *program = read_file(input->value);
  if (program == NULL) {
    printf("Something went wrong while reading the input file\n");
    exit(1);
  }

  struct bf_instructions *instructions = bf_init();
  if (bf_parse(program, instructions)) {
    printf("Something went wrong while parsing the input file\n");
    exit(1);
  }

  if (bf_match_brackets(instructions)) {
    printf("Program's brackets doesnot match\n");
    exit(1);
  }

  generate_assembly(output->value, instructions);

  bf_delete(instructions);

  free(program);

  ap_parser_delete(parser);

  ap_value_delete(help);
  ap_value_delete(verbose);
  ap_value_delete(input);
  ap_value_delete(output);

  return 0;
}

char *read_file(const char *name) {
  FILE *file = stdin;
  if (name != NULL) {
    file = fopen(name, "r");
  }
  if (file == NULL) {
    return NULL;
  }

  int count = 0;
  int capacity = 0;
  char *program = NULL;
  char buffer[1024];

  while (fgets(buffer, 1024, file)) {
    int buffer_count = strlen(buffer);
    if (capacity <= count + buffer_count) {
      capacity += 1024;
      program = realloc(program, sizeof(char) * capacity);
    }
    count += buffer_count;
    strcat(program, buffer);
  }

  if (name != NULL) {
    fclose(file);
  }
  
  return program;
}

struct bf_instructions *bf_init() {
  struct bf_instructions *res = calloc(1, sizeof(struct bf_instructions));
  return res;
}

int bf_delete(struct bf_instructions *instructions) {
  if (instructions == NULL) {
    return -1;
  }

  free(instructions->items);
  free(instructions);
  return 0;
}

int bf_append(struct bf_instructions *instructions, enum bf_code code) {
  if (instructions == NULL) {
    return -1;
  }

  if (instructions->capacity <= instructions->count) {
    instructions->capacity += 8;
    instructions->items = realloc(instructions->items, 
                                  sizeof(*instructions->items) * instructions->capacity);
  }
  instructions->items[instructions->count] = code;
  instructions->count++;
  
  return 0;
}

int bf_parse(const char *program, struct bf_instructions *instructions) {
  if (program == NULL || instructions == NULL) {
    return -1;
  }

  for (int i = 0; program[i]; i++) {
    switch(program[i]) {
      case OP_MOVE_RIGHT:
        bf_append(instructions, OP_MOVE_RIGHT);
        break;
      case OP_MOVE_LEFT:
        bf_append(instructions, OP_MOVE_LEFT);
        break;
      case OP_INCREMENT:
        bf_append(instructions, OP_INCREMENT);
        break;
      case OP_DECREMENT:
        bf_append(instructions, OP_DECREMENT);
        break;
      case OP_OUTPUT:
        bf_append(instructions, OP_OUTPUT);
        break;
      case OP_INPUT:
        bf_append(instructions, OP_INPUT);
        break;
      case OP_JUMP_FORWARD:
        bf_append(instructions, OP_JUMP_FORWARD);
        break;
      case OP_JUMP_BACKWARD:
        bf_append(instructions, OP_JUMP_BACKWARD);
        break;
      default:
        break;
    }
  }

  return 0;
}

int bf_match_brackets(struct bf_instructions *instructions) {
  int count = 0;

  for (int i = 0; i < instructions->count; i++) {
    if (instructions->items[i] == OP_JUMP_FORWARD) {
      count++;
    }
    else if (instructions->items[i] == OP_JUMP_BACKWARD) {
      if (count == 0) {
        return -1;
      }

      count--;
    }
  }

  return count;
}

void generate_assembly(const char* name, struct bf_instructions *instructions) {
  FILE *file = stdout;
  if (name != NULL) {
    file = fopen(name, "w");
  }
  if (file == NULL) {
    return;
  }

  // code sections
  fprintf(file, "section .text\n");
  fprintf(file, "  global _start\n");
  fprintf(file, "\n");
  fprintf(file, "_start:\n");
  
  // main code
  int label = 0;
  int top = -1;
  int stack[instructions->count];
  for (int i = 0; i < instructions->count; i++) {
    switch(instructions->items[i]) {
      case OP_MOVE_RIGHT:
        generate_assembly_move_right(file);
        break;
      case OP_MOVE_LEFT:
        generate_assembly_move_left(file);
        break;
      case OP_INCREMENT:
        generate_assembly_increment(file);
        break;
      case OP_DECREMENT:
        generate_assembly_decrement(file);
        break;
      case OP_OUTPUT:
        generate_assembly_output(file);
        break;
      case OP_INPUT:
        generate_assembly_input(file);
        break;
      case OP_JUMP_FORWARD:
        stack[++top] = label;
        generate_assembly_jump_forward(file, label);
        label++;
        break;
      case OP_JUMP_BACKWARD:
        generate_assembly_jump_backward(file, stack[top--]);
        break;
      default:
        break;
    }
  }

  // exiting with 0
  fprintf(file, "  mov rax, 60            ; syscall for exit\n");
  fprintf(file, "  mov rdi, 0             ; exit code 0\n");
  fprintf(file, "  syscall                ; invode exit syscall\n");
  fprintf(file, "\n");

  // data section
  fprintf(file, "section .bss\n");
  fprintf(file, "tape: resb 30000\n");
  fprintf(file, "pointer: resq 1          ; pointer to current cell\n");

  if (name != NULL) {
    fclose(file);
  }
}

void generate_assembly_move_right(FILE* file) {
  fprintf(file, "  inc qword [pointer]    ; increase the pointer\n");
  fprintf(file, "\n");
}

void generate_assembly_move_left(FILE* file) {
  fprintf(file, "  dec qword [pointer]    ; decrease the pointer\n");
  fprintf(file, "\n");
}

void generate_assembly_increment(FILE* file) {
  fprintf(file, "  mov rax, tape          ; get the starting address of the tape\n");
  fprintf(file, "  add rax, [pointer]     ; move to the pointer index\n");
  fprintf(file, "  inc byte [rax]         ; increase value in the pointer index\n");
  fprintf(file, "\n");
}

void generate_assembly_decrement(FILE* file) {
  fprintf(file, "  mov rax, tape          ; get the starting address of the tape\n");
  fprintf(file, "  add rax, [pointer]     ; move to the pointer index\n");
  fprintf(file, "  dec byte [rax]         ; decrease value in the pointer index\n");
  fprintf(file, "\n");
}

void generate_assembly_output(FILE* file) {
  fprintf(file, "  mov rax, 1             ; syscall for write\n");
  fprintf(file, "  mov rdi, 1             ; providing file descriptor as 1(stdout)\n");
  fprintf(file, "  mov rsi, tape          ; load tape address\n");
  fprintf(file, "  add rsi, [pointer]     ; go to pointer address\n");
  fprintf(file, "  mov rdx, 1             ; length of the string (in this case, 1)\n");
  fprintf(file, "  syscall                ; provoke sys write\n");
  fprintf(file, "\n");
}

void generate_assembly_input(FILE* file) {
  fprintf(file, "  mov rax, 0             ; syscall for read\n");
  fprintf(file, "  mov rdi, 0             ; providing file descriptor as 0(stdin)\n");
  fprintf(file, "  mov rsi, tape          ; load tape address\n");
  fprintf(file, "  add rsi, [pointer]     ; go to pointer address\n");
  fprintf(file, "  mov rdx, 1             ; length of the string (in this case, 1)\n");
  fprintf(file, "  syscall                ; provoke sys read\n");
  fprintf(file, "\n");
}

void generate_assembly_jump_forward(FILE* file, int label) {
  fprintf(file, "  mov rax, tape          ; move tape pointer value\n");
  fprintf(file, "  add rax, [pointer]     ; move to pointer index\n");
  fprintf(file, "  cmp byte [rax], 0      ; compare current cell to 0\n");
  fprintf(file, "  je .LB%d\n", label);
  fprintf(file, "                         ; jump to opening bracket if not equal\n");
  fprintf(file, ".LF%d:\n", label);
  fprintf(file, "\n");
}

void generate_assembly_jump_backward(FILE* file, int label) {
  fprintf(file, "  mov rax, tape          ; move tape pointer value\n");
  fprintf(file, "  add rax, [pointer]     ; move to pointer index\n");
  fprintf(file, "  cmp byte [rax], 0      ; compare current cell to 0\n");
  fprintf(file, "  jne .LF%d\n", label);
  fprintf(file, "                         ; jump to opening bracket if not equal\n");
  fprintf(file, ".LB%d:\n", label);
  fprintf(file, "\n");
}

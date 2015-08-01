// codegenerator.cpp : code generator
//   author : members of the Venture group
using namespace std;
#include <list>
#include <stdio.h>
#include <stdint.h>

#include "config.h"
#include "lexical.h"
#include "locale.h"

#define EXTERN
#include "codegenerator.h"

void code_open(char *f) {
  if ((f == NULL) || (*f == '\0')) {
    // use standard output if a file isn't specified
    code_out = stdout;
  }
  else {
    code_out = fopen(f, "w");
    // make sure the file was opened successfully
    if (code_out == NULL)
      error_fatal(ER_BADOUTFILE);
    code_out_name = f;
  }
  stack_top = CODE_MINREG - 1;
  stack_count = 0;
}

void code_prologue(char *ifn) {
  fputs("\t@ prologue\n", code_out);
  fputs("\t.arch\tarmv6\n", code_out);
  fputs("\t.eabi_attribute\t27,\t3\n", code_out);
  fputs("\t.eabi_attribute\t28,\t1\n", code_out);
  fputs("\t.fpu\tvfp\n", code_out);
  fputs("\t.eabi_attribute\t20,\t1\n", code_out);
  fputs("\t.eabi_attribute\t21,\t1\n", code_out);
  fputs("\t.eabi_attribute\t23,\t3\n", code_out);
  fputs("\t.eabi_attribute\t24,\t1\n", code_out);
  fputs("\t.eabi_attribute\t25,\t1\n", code_out);
  fputs("\t.eabi_attribute\t26,\t2\n", code_out);
  fputs("\t.eabi_attribute\t30,\t6\n", code_out);
  fputs("\t.eabi_attribute\t18,\t4\n", code_out);
  if (ifn) fprintf(code_out, "\t.file\t\"%s\"\n", ifn);
  else     fputs("\t.file\t\"stdin\"\n", code_out);
  fputs("\t.text\n", code_out);
}

void code_epilogue() {
  fputs("\t@ epilogue\n", code_out);
  fputs("\t.ident\t\"GCC: (Debian 4.6.3-14+rpi1) 4.6.3\"\n", code_out);
  fputs("\t.section\t.note.GNU-stack,\"\",%progbits\n", code_out);
}

///////////////////////////////////////////////////////////
// stack machine instructions
///////////////////////////////////////////////////////////

// M[++sp] = c;
void code_pushi(int c) {
  char r = stack_top + 1;
  if (r > CODE_MAXREG) r = CODE_MINREG;

  if (stack_count >= CODE_NUMREGS)
    // the cache registers are full, so push R[r] on the RAM stack
    fprintf(code_out, "\tstr\tr%d,\t[fp,\t#-8]\n", r);
  
  fprintf(code_out, "\tmov\tr%d,\t#%d\n", r, c); 
  stack_top = r;
  stack_count += 1;    
}
//TODO: code_load
// M[sp] = M[M[sp]];
void code_load() {
  fprintf(code_out, "\tldr\tr%d,\t[r%d,\t#0]\n", stack_top, stack_top);
}

// sp = sp - 1;
void code_popd() {
  char r = stack_top--; // decrement stack pointer but save old stack_top

  if (stack_top < CODE_MINREG)
    stack_top = CODE_MAXREG;

  // cache was full, but now we have an open spot
  if (--stack_count >= CODE_NUMREGS) {
    // so load a value from memory into a register
    fprintf(code_out, "\tldr\tr%d,\t[fp,\t#8]\n", r);
  }
}
//TODO: code_pops
// temp = M[sp--];
// M[M[sp--]] = temp;
void code_pops() {

}
//TODO: code_dup
// M[++sp] = M[sp];
void code_dup() {

}

// temp = M[sp--];
// M[sp] = M[sp] + temp;
void code_add() {
  if (stack_count == 0) {
    // ???
    fprintf(code_out, "\tldr\tr%d,\t[fp,\t#8]\n", stack_top);
    stack_count += 1;
  }
  char r = stack_top - 1;
  if (r < CODE_MINREG) r = CODE_MAXREG;
  if (stack_count == 1) {
    // ???
    fprintf(code_out, "\tldr\tr%d,\t[fp,\t#8]\n", r);
    stack_count += 1;
  }
  fprintf(code_out, "\tadd\tr%d,\tr%d,\tr%d\n", r, r, stack_top);
  code_popd();
}

// temp = M[sp--];
// M[sp] = M[sp] - temp;
void code_sub() {
  if (stack_count == 0) {
    // ???
    fprintf(code_out, "\tldr\tr%d,\t[fp,\t#8]\n", stack_top);
    stack_count += 1;
  }
  char r = stack_top - 1;
  if (r < CODE_MINREG) r = CODE_MAXREG;
  if (stack_count == 1) {
    // ???
    fprintf(code_out, "\tldr\tr%d,\t[fp,\t#8]\n", r);
    stack_count += 1;
  }
  fprintf(code_out, "\tsub\tr%d,\tr%d,\tr%d\n", r, r, stack_top);
  code_popd();
}
//TODO: code_gt
// temp = M[sp--];
// M[sp] = M[sp] > temp;
void code_gt() {

}
//TODO:code_lt
// temp = M[sp--];
// M[sp] = M[sp] < temp;
void code_lt() {

}
//TODO: code_eq
// temp = M[sp--];
// M[sp] = M[sp] == temp;
void code_eq() {

}
//TODO: code_btrue
// if M[sp--] pc = dst;
void code_btrue(int dst) {

}
//TODO:bfalse
// if !M[sp--] pc = dst;
void code_bfalse(int dst) {

}
//TODO:pushla
// M[++sp] = disp + fp;
void code_pushla(int disp) {
  code_pushi(disp); 
}

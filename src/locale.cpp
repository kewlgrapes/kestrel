// locale.cpp : locale management for Kestrel compiler
//   author : Douglas Jones
#include <stdlib.h>
#include <stdint.h>
#include <stdio.h>

#include "config.h"
#include "lexical.h"
#include "kestrel.h"
#include "locale.h"

// locale management //
const char *LOC_DEFAULTNAME = "kestrel";

// this array should have help messages corresponding to members of help_code
static const char *help_message[] ={
  "HeeeeeeeeeeelpHEEEEEELLLLLLLLAPSODKPOKPAOSdHEEEEEELLLLLLLLAPSODKPOKPAOSdHEEEEEELLLLLLLLAPSODKPOKPAOSd", // H_DEFAULT
};


// help messages
void print_help( help_code topic ) {
  printf("%s\n", help_message[topic]);
}

void print_help() {
  printf("%s\n", help_message[HLP_DEFAULT]);
}

// error management //

// this array must have on eentry for each member of the error_code 
//   enumeration (in errors.h) in exactly the same order
// used in calls to error_fatal()
static const char *error_message[] = {
  // code generation
  "Cannot open output file.", // ER_BADOUTFILE

  // punctuation
  "Invalid opening symbol for grouping.", // ER_INVOPEN
  
  // string pool errors
  "Pool received a handle past the end of the pool.", // ER_POOLBOUNDS
  "Pool is full, please go swimming elsewhere.", // ER_POOLFULL

  // symbol table errors
  "Tried to add a symbol but the hash table is full.", // ER_HASHFULL

  // lexical errors
  "Cannot open input file.", // ER_BADFILE
  "Encountered an unexpected lexeme.", // ER_LEXEME
  "Encountered new line while reading string literal.", // ER_LEXSTRNL
  "Reached end of file while reading string literal.", // ER_LEXSTREOF
  "Encountered an unknown character.", // ER_LEXUNK
  "Number overflowed during accumulation.", // ER_LEXOVERFLOW
  "Digit larger than radix.", // ER_LEXDIGITRAD
  "Couldn't decode digit.", // ER_LEXDIGITBAD
  "Unsupported radix.", // ER_LEXBADRADIX
  "Tried to advance past the end of the file.", //ER_LEX_EOF

  // kestrel errors
  "No input files.", // ER_NOINPUT
  "Unknown option.", // ER_OPTION
  "Multiple output files given." // ER_MULTOUT
};

// fatal errors
  
void error_fatal( error_code er, const char *origin) {
  fprintf( stdout, "[%s] Fatal error: %s\n", origin, error_message[er]);
  exit( EXIT_FAILURE );
}

void error_fatal( error_code er, lexeme lex, const char *origin) {
  fprintf( stdout, "[%s] Fatal error (line %d, col %d): %s\n", \
    origin, lex.line, lex.col, error_message[ er ] );
  exit( EXIT_FAILURE );
}

void expected_but_got(const char *exp, lexeme lex, const char *origin) {
  fprintf( stderr, "[%s] Fatal error (line %d, col %d): Expected: \"%s\", found: \"", \
    origin, lex.line, lex.col, exp);
  lex_put(&lex, stderr);
  fputs("\"\n", stderr);
  exit( EXIT_FAILURE );
}

void error_undeclared(lexeme lex, const char *origin) {
  fprintf( stderr, "[%s] Fatal error (line %d, col %d): \"", 
    origin, lex.line, lex.col);
  lex_put(&lex, stderr);
  fprintf( stderr, "\" undeclared in current context.\n");
  exit( EXIT_FAILURE );
}

// warnings

void error_warn( error_code er, lexeme lex, const char *origin) {
  fprintf( stdout, "[%s] Warning (line %d, col %d): %s\n", \
    origin, lex.line, lex.col, error_message[er]);
}

void error_warn( error_code er, const char *origin) {
  fprintf( stdout, "[%s] Warning: %s\n", origin, error_message[er]);
}

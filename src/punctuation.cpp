// punctuation.cpp : hash of punctuation characters
//   author : members of the Venture group

#include <stdio.h>
#include <stdint.h>

#include "config.h"
#include "lexical.h"
#include "locale.h"

#define EXTERN
#include "punctuation.h"

// used to map punct_type to respective character string
const char *punct_val[] = {
  "="  /* PT_EQUALS */, ":"  /* PT_COLON  */,   
  "("  /* PT_LPAREN */, ")"  /* PT_RPAREN */, 
  "["  /* PT_LBRAKT */, "]"  /* PT_RBRAKT */, 
  "{"  /* PT_LBRACE */, "}"  /* PT_RBRACE */, 
  ","  /* PT_COMMA  */, "@"  /* PT_ATSIGN */, 
  ".." /* PT_ELIPS  */, "<>" /* PT_NOTEQL */, 
  ">"  /* PT_GT     */, ">=" /* PT_GE     */, 
  "<"  /* PT_LT     */, "<=" /* PT_LE     */, 
  "+"  /* PT_PLUS   */, "-"  /* PT_MINUS  */, 
  "*"  /* PT_TIMES  */, "/"  /* PT_DIV    */, 
  "&"  /* PT_AND    */, "|"  /* PT_OR     */, 
  "."  /* PT_DOT    */, ";"  /* PT_SEMIC  */,
  "~"  /* PT_TILDE  */ 
};

void punct_init() {
  int i;

  // initialize table to invalid
  for (i = 0; i < PUNCT_HASH_SIZE; i++) {
    _punct_table[i].p = KEY_INVALID;
    _punct_table[i].ch = '\0';
  }

  // add keywords from key_val
  for (i = PUNCT_MIN; i <= PUNCT_MAX; i++) {
    // these punctuation types are "special" because they consist
    //   of more than one character
    if (i == PT_ELIPS || i == PT_GE || i == PT_LE || i == PT_NOTEQL)
      continue;

    char ch = *punct_val[i]; // first character of punct
    int j = punct_hash(ch);

    // find an empty spot in the keyword table near j
    // this will terminate because there are empty spots in table
    while (_punct_table[j].p != PUNCT_INVALID) {
      //printf("collision %d %c\n", i, ch);
      j++;
      if (j == PUNCT_HASH_SIZE) j = 0;
    }
    _punct_table[j].p = i;
    _punct_table[j].ch = ch;
  }
}

punct_handle punct_lookup(char ch){
  int j = punct_hash(ch);

  while ((_punct_table[j].ch != ch) && (_punct_table[j].p != PUNCT_INVALID)) {
    j++;
    if (j == PUNCT_HASH_SIZE) j = 0;
  }
  return _punct_table[j].p;
}

punct_type punct_closing_symbol(punct_type opening) {
  switch (opening) {
    case PT_LPAREN: return PT_RPAREN;
    case PT_LBRAKT: return PT_RBRAKT;
    case PT_LBRACE: return PT_RBRACE;
  }
  error_fatal(ER_INVOPEN, lex_this);
}

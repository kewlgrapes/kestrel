// keywords.cpp : extends symbol table with a keyword table
//   author : Ken Jakubzak
#include <stdio.h>
#include <stdint.h>

#include "config.h"
#include "stringpool.h"
#include "symboltable.h"
#include "lexical.h"
#include "locale.h"

#define EXTERN
#include "keywords.h"

// used to map elements of key_type to strings
const char *keyword_val[] = {
  "array",    "of" ,
  "case",     "null",
  "const",    "private", 
  "do",       "procedure",
  "else",     "raise",
  "end",      "record",
  "enum",     "select",
  "exception",  "then",
  "external", "try",
  "for",      "type",
  "function", "until",
  "handle",   "var",
  "if",       "while", 
  "in" 
};

void keyword_init() {
  int i;
  
  // initialize table to invalid
  for (i = 0; i < KEY_REHASH_SIZE; i++) {
    _key_table[i].key = KEY_INVALID;
    _key_table[i].sym = SYMBOL_INVALID;
  }

  // add keywords from key_val
  for (i = KEY_MIN; i <= KEY_MAX; i++) {
    symbol_handle s = symbol_define(keyword_val[i]);

    // rehash function uses last 6 bits of symbol handle
    int j = keyword_hash(s);

    // find an empty spot in the keyword table near j
    while (_key_table[j].key != KEY_INVALID) {
      j++;
      if (j == KEY_REHASH_SIZE) j = 0;
    }
    _key_table[j].key = i;
    _key_table[j].sym = s;
  }
}

// tries to find a keyword associated with a symbol handle s
// returns the key handle if successful, otherwise returns KEY_INVALID
keyword_handle keyword_lookup(symbol_handle s){
  int j = keyword_hash(s);

  // we know the keyword table has empty spots, so this will terminate
  while ((_key_table[j].sym != s) && (_key_table[j].sym != SYMBOL_INVALID)) {
    j++;
    if (j == KEY_REHASH_SIZE) j = 0;
  }
  return _key_table[j].key;
}

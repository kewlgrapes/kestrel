// symboltable.cpp : implementation of the symbol table
//   author : Douglas Jones
//   modified by : members of the Venture group
#include <stdio.h>
#include <stdint.h>
#include <string.h>

#include "config.h"
#include "stringpool.h"
#include "lexical.h"
#include "locale.h"

#define EXTERN
#include "symboltable.h"

void symbol_init() {
  for (int i = 0; i < SYMBOL_SIZE; i++)
    _symbol_table[i] = STRING_NULL;
}

symbol_handle symbol_define(const char *s){
  symbol_start();
  while(*s != '\000'){
    symbol_append(*s);
    s++;
  }
  return symbol_lookup();
}

symbol_handle symbol_lookup() {
  string_done();
  symbol_handle place = _symbol_hash;

  for (;;) {
    if (_symbol_table[place] == STRING_NULL) {
      // add symbol to table
      _symbol_table[place] = _symbol_string;
      string_accept();
      return place;
    }
    if (!string_eq(_symbol_table[place], _symbol_string)) {
      // symbol is already in the table
      string_reject();
      return place;
    }
    place += 1;
    if (place == SYMBOL_SIZE) place = 0;
    if (place == _symbol_hash) error_fatal(ER_HASHFULL, "Symbol table");
  }
}

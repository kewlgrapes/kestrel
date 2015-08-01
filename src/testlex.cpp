// testlex.cpp : test framework for lexical analyzer
//   author : Douglas W. Jones
//   modified by : Nick Becker
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

#include "config.h"
#include "lexall.h"
#include "locale.h"

int main( int argc, char * argv[] ) {
  //test();
  lex_open( argv[1] );
  while (!(lex_this.type == ENDFILE)) {
    lex_put( &lex_this, stdout );
    putchar('\n');
    lex_advance();
  }
  lex_put( &lex_this, stdout);
  putchar('\n');
}

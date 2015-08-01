// testparse.cpp : test framework for parser
//   author : members of the Venture group
#include <stdio.h>
#include <stdint.h>
#include <list>
using namespace std;

#include "config.h"
#include "sets.h"
#include "lexall.h"
#include "parseall.h"
#include "locale.h"

int main( int argc, char * argv[] ) {
  lex_open(argv[1]);
  code_open(NULL);
  Block *p = parse_program();
  p->put(stdout);
  fputc('\n', stdout);
}

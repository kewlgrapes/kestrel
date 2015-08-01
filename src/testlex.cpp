// testlex.cpp : test framework for lexical analyzer
//   author : Douglas W. Jones
//   modified by: members of the Venture group
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

#include "config.h"
#include "lexall.h"
#include "locale.h"

/*
void test();
string_handle add_string(const char *string);
*/

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

/*
// to test things
void test() {
  string_init();

  string_handle h1 = add_string("hello");
  printf("string_adding() = %d\n", string_adding());
  string_accept();
  printf("string_adding() = %d\n", string_adding());
  string_start();
  printf("string_adding() = %d\n", string_adding());
  string_reject();
  printf("string_adding() = %d\n", string_adding());

  string_handle h2 = add_string("world");
  string_accept();
  string_handle h3 = add_string("hello");
  string_accept();
  string_handle h4 = add_string("jojo");
  string_accept();
  
  printf("string_eq(h1, h2) = %d\n", string_eq(h1, h2));
  printf("string_eq(h1, h3) = %d\n", string_eq(h1, h3));
  printf("string_eq(h2, h4) = %d\n", string_eq(h2, h4));

  exit(EXIT_FAILURE);
}

void test2() {
  
  lex_put( &lex_this, stdout );
  putchar('\n');
  lex_put( &lex_next, stdout );
  putchar('\n');
  putchar('\n');

  lex_set( lex_this, NUMBER, 10, 5, 10);
  lex_put( &lex_this, stdout );
  putchar('\n');
  lex_put( &lex_next, stdout );
  putchar('\n');
  putchar('\n');

  lex_copy( lex_this, lex_next );
  lex_put( &lex_this, stdout );
  putchar('\n');
  lex_put( &lex_next, stdout );
  putchar('\n');
  putchar('\n');
  
  exit(1);
}

// stringpool testing

// adds a string to the string pool
string_handle add_string(const char *str) {
  printf("h = string_start()\n");
  string_handle h = string_start();
  printf("h = %d\n", h);
  const char *ch = str;
  while (*ch) {
    printf("string_append(%c)\n", *ch);
    string_append(*ch++);
  }
  printf("string_done()\n");
  string_done();
  return h;
}

void test_stringpool() {
  char test1[] = "hello world!";
  char test2[] = "venture bros unite!";
  char test3[] = "hello world!";
  char test4[] = "we will reject this one";
  char test5[] = "we will also reject this one";
  char test6[] = "we will now accept this one";

  string_init();
  
  // add strings to pool
  string_handle h1 = add_string(test1);
  printf("string_accept()\n");
  string_accept();
  printf("_string_limit = %d\n", _string_limit);
  printf("_string_pos = %d\n", _string_pos);
  string_handle h2 = add_string(test2);
  printf("string_accept()\n");
  string_accept();
  printf("_string_limit = %d\n", _string_limit);
  printf("_string_pos = %d\n", _string_pos);
  string_handle h3 = add_string(test3);
  printf("string_accept()\n");
  string_accept();
  printf("_string_limit = %d\n", _string_limit);
  printf("_string_pos = %d\n", _string_pos);

  // add some strings then reject them
  string_handle h4 = add_string(test4);
  printf("string_reject()\n");
  string_reject();
  printf("_string_limit = %d\n", _string_limit);
  printf("_string_pos = %d\n", _string_pos);
  string_handle h5 = add_string(test5);
  printf("string_reject()\n");
  string_reject();
  printf("_string_limit = %d\n", _string_limit);
  printf("_string_pos = %d\n", _string_pos);

  // add and accept one more string
  string_handle h6 = add_string(test6);
  printf("string_accept()\n");
  string_accept();
  printf("_string_limit = %d\n", _string_limit);
  printf("_string_pos = %d\n", _string_pos);

  // try to print them
  printf("string_put(h1, NULL)\n");
  string_put(h1, NULL);
  printf("\n");
  printf("string_put(h2, NULL)\n");
  string_put(h2, NULL);
  printf("\n");
  printf("string_put(h3, fopen(\"test.txt\", \"w\"))\n");
  string_put(h3, fopen("test.txt", "w"));
  printf("string_put(h4, NULL)\n");
  string_put(h4, NULL);
  printf("\n");
  printf("string_put(h5, NULL)\n");
  string_put(h5, NULL);
  printf("\n");
  printf("string_put(h6, NULL)\n");
  string_put(h6, NULL);
  printf("\n");

  // compare some strings
  printf("string_eq(h1, h2) = %d\n", string_eq(h1, h2));
  printf("string_eq(h2, h3) = %d\n", string_eq(h2, h3));
  printf("string_eq(h1, h3) = %d\n", string_eq(h1, h3));
  printf("string_eq(h1, h6) = %d\n", string_eq(h1, h6));

  // print length of strings
  printf("string_len(h1) = %d\n", string_len(h1));
  printf("string_len(h2) = %d\n", string_len(h2));
  printf("string_len(h3) = %d\n", string_len(h3));
  printf("string_len(h6) = %d\n", string_len(h6));

  // these should be the same as h6
  printf("string_len(h4) = %d\n", string_len(h4));
  printf("string_len(h5) = %d\n", string_len(h5));

  // some more tests
  printf("string_put(200, NULL)\n");
  string_put(200, NULL);
  printf("string_len(200) = %d\n", string_len(200));
  printf("string_eq(200, 400) = %d\n", string_eq(200, 400));

  // try filling up the pool
  printf("string_pool_full() = %d\n", string_pool_full());
  printf("adding a really long string...\n");
  string_handle h = string_start();
  while (! string_pool_full()) {
    string_append('a');
    //printf("%d\n", _string_pos);
  }
  string_done();
  string_accept();
  printf("string_pool_full() = %d\n", string_pool_full());
  printf("_string_pos = %d\n", _string_pos);
  printf("_string_limit = %d\n", _string_limit);

  // this results in a fatal error
  printf("string_len(POOL_SIZE)\n");
  string_len(POOL_SIZE);
  printf("this shouldn't print\n");
}
*/

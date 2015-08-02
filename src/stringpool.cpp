// stringpool.cpp : Kestrel string pool management
#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>

#include "config.h"
#include "lexical.h"
#include "locale.h"

#define EXTERN
#include "stringpool.h"

// output the string to the human-readable file
void string_put( string_handle h, FILE * f ) {
  check_bounds(h);
  
  // use standard output if file is null
  if (f == NULL) {
    f = stdout;
  }

  // write the string to the file
  while ((h < _string_limit) && (_string_pool[h] != '\0'))
    fputc(_string_pool[h++], f);
}

// compare the strings h1 and h2. returns 0 if the strings have identical
// text. otherwise returns a negative value if string h1 is before h2
// alphabetically, and vice-versa
int string_eq( string_handle h1, string_handle h2 ) {
  check_bounds(h1);
  check_bounds(h2);

  // loop while characters are equal
  for ( ; _string_pool[h1] == _string_pool[h2]; h1++, h2++)
    if (_string_pool[h1] == '\0')
      return 0;
  return _string_pool[h1] - _string_pool[h2];
}

// returns the length of the string h
int string_len( string_handle h ) {
  check_bounds(h);

  // loop until a null-terminating character is found
  int len = 0;
  while (_string_pool[h+len] != '\0') 
    len++;
  return len;
}

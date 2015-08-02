// lexical.cpp : Kestrel lexical analyzer
//   author : D.W.Jones
//   modifed by : Nick Becker
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>

#include "config.h"
#include "punctuation.h"
#include "stringpool.h"
#include "symboltable.h"
#include "keywords.h"

#define EXTERN
#include "lexical.h"

#include "locale.h"

FILE *infile; // the file that we are reading from
int ch_this;  // the current character (type char) or EOF
int ch_next;  // the character following the current character
#define nextchar() { ch_this = ch_next;        \
                     ch_next = getc( infile ); \
                     lex_col += 1; }
// note: it is parse_whitespace()'s job to increment line count / reset col

// a mapping of characters (ASCII7) to their types
static const char_type char_class[256] = {
  // NUL SOH STX ETX EOT ENQ ACK BEL BS  HT  LF  VT  FF  CR  SO  SI
     OTH,OTH,OTH,OTH,OTH,OTH,OTH,OTH,OTH,WIT,WIT,WIT,WIT,WIT,OTH,OTH,
  // DLE DC1 DC2 DC3 DC4 NAK SYN ETB CAN EM  SUB ESC FS  GS  RS  US
     OTH,OTH,OTH,OTH,OTH,OTH,OTH,OTH,OTH,OTH,OTH,OTH,OTH,OTH,OTH,OTH,
  //      !   "   #   $   %   &   '   (   )   *   +   ,   -   .   /
     WIT,OTH,OTH,OTH,OTH,OTH,PUN,OTH,PUN,PUN,PUN,PUN,PUN,PUN,PUN,PUN,
  //  0   1   2   3   4   5   6   7   8   9   :   ;   <   =   >   ?
     DIG,DIG,DIG,DIG,DIG,DIG,DIG,DIG,DIG,DIG,PUN,PUN,PUN,PUN,PUN,OTH,
  //  @   A   B   C   D   E   F   G   H   I   J   K   L   M   N   O
     PUN,LET,LET,LET,LET,LET,LET,LET,LET,LET,LET,LET,LET,LET,LET,LET,
  //  P   Q   R   S   T   U   V   W   X   Y   Z   [   \   ]   ^   _
     LET,LET,LET,LET,LET,LET,LET,LET,LET,LET,LET,PUN,OTH,PUN,OTH,LET,
  //  `   a   b   c   d   e   f   g   h   i   j   k   l   m   n   o
     OTH,LET,LET,LET,LET,LET,LET,LET,LET,LET,LET,LET,LET,LET,LET,LET,
  //  p   q   r   s   t   u   v   w   x   y   z   {   |   }   ~  DEL
     LET,LET,LET,LET,LET,LET,LET,LET,LET,LET,LET,PUN,PUN,PUN,PUN,OTH,
  // beyond ASCII
     OTH,OTH,OTH,OTH,OTH,OTH,OTH,OTH,OTH,OTH,OTH,OTH,OTH,OTH,OTH,OTH,
     OTH,OTH,OTH,OTH,OTH,OTH,OTH,OTH,OTH,OTH,OTH,OTH,OTH,OTH,OTH,OTH,
     OTH,OTH,OTH,OTH,OTH,OTH,OTH,OTH,OTH,OTH,OTH,OTH,OTH,OTH,OTH,OTH,
     OTH,OTH,OTH,OTH,OTH,OTH,OTH,OTH,OTH,OTH,OTH,OTH,OTH,OTH,OTH,OTH,
     OTH,OTH,OTH,OTH,OTH,OTH,OTH,OTH,OTH,OTH,OTH,OTH,OTH,OTH,OTH,OTH,
     OTH,OTH,OTH,OTH,OTH,OTH,OTH,OTH,OTH,OTH,OTH,OTH,OTH,OTH,OTH,OTH,
     OTH,OTH,OTH,OTH,OTH,OTH,OTH,OTH,OTH,OTH,OTH,OTH,OTH,OTH,OTH,OTH,
     OTH,OTH,OTH,OTH,OTH,OTH,OTH,OTH,OTH,OTH,OTH,OTH,OTH,OTH,OTH,OTH
};

// open the file f for reading
void lex_open( char * f ) {
  // initialize subsidiary packages on which we depend
  string_init();
  symbol_init();
  punct_init();
  keyword_init();

  // open the file
  if ((f == NULL) || (*f == '\0')) {
    // use standard input if a file wasn't specified
    infile = stdin;
  }
  else {
    infile = fopen(f, "r");
    // make sure the file was opened successfully
    if (infile == NULL)
      error_fatal(ER_BADFILE);
  }
  
  // read the first two characters
  nextchar();
  nextchar();
  
  // initialize these
  lex_col = 1;
  lex_line = 1;

  // read the first two lexemes
  lex_advance();
  lex_advance();
}

// writes the current lexeme type to f
void lex_put( lexeme * lex, FILE * f ) {
  switch (lex->type) {
    case IDENT:
      symbol_put(lex->value, f)
      break;
    case KEYWORD:
      keyword_put(lex->value, f);
      break;
    case NUMBER:
      fprintf(f, "%d", lex->value);
      break;
    case STRING:
      symbol_put(lex->value, f);
      break;
    case PUNCT:
      punct_put(lex->value, f);
      break;
    case ENDFILE:
      fprintf(f,"EOF");
      break;
    case UNKNOWN:
      fprintf( f, "Unknown character(s): ");
      symbol_put(lex->value, f);
  }
}

// used by lex_force below
const char *lex_type_strings[] = {
  "identifier",
  "keyword",
  "number",
  "string",
  "punctuation",
  "EOF",
  "UNK"
};

// make sure the curent lexeme matches the type specified by t
void lex_force(lex_type t) {
  if (lex_this.type != t) {
    expected_but_got(lex_type_strings[(int)t], lex_this);
    error_fatal(ER_LEXEME, lex_this);
  }
}

// also check value
void lex_force(lex_type t, uint32_t v, const char *exp) {
  if ((lex_this.type != t) || (lex_this.value != v)) {
    expected_but_got(exp, lex_this);
    error_fatal(ER_LEXEME, lex_this);
  }
}

// all parsing is handled below this line //
// maybe move these into a separate file? //

/////////////////////////////////////////////////////////////////////
// punctuation
/////////////////////////////////////////////////////////////////////

// <punctuation> ::= <lots of stuff>
void parse_punctuation() {
  punct_handle punct_this = punct_lookup(ch_this);
  
  // if next character is punctuation, might be a two char punctuation
  if (lex_is_class(ch_next, PUN)) {
    punct_handle punct_next = punct_lookup(ch_next);
    
    switch (punct_this) {
      // '.' or '..'
      case PT_DOT:
        if (punct_next == PT_DOT) {
          nextchar();
          punct_this = PT_ELIPS;
        }
        else
          punct_this = PT_DOT;
        break;

      // '>' or '>='
      case PT_GT:
        if (punct_next == PT_EQUALS) {
          nextchar();
          punct_this = PT_GE;
        }
        else
          punct_this = PT_GT;
        break;

      // '<', '<=', or '<>'
      case PT_LT:
        if (punct_next == PT_EQUALS) {
          nextchar();
          punct_this = PT_LE;
        }
        else if (punct_next == PT_GT) {
          nextchar();
          punct_this = PT_NOTEQL;
        }
        else
          punct_this = PT_LT;
        break;

      default:
        punct_this = punct_this;
        break;
    } 
  }
  lex_set(lex_next, PUNCT, punct_this, lex_line, lex_col);
}

/////////////////////////////////////////////////////////////////////
// numbers
/////////////////////////////////////////////////////////////////////

// returns integer representation of a character ch
char digit_of_char(unsigned char ch) {
  // '0' - '9'
  if (lex_is_class(ch, DIG))
    return ch - '0';
  else if (lex_is_class(ch, LET)) {
    if ((ch == 'o') || (ch == 'O')) return 0;
    if ((ch == 'i') || (ch == 'I') || (ch == 'l') || (ch == 'L')) return 1;
    if ((ch == 'u') || (ch == 'U')) return 36;

    // there are 'holes' we have to account for
    char val = 10 + ch - ((ch >= 'a') ? 'a' : 'A');
    val -= (val > 17) + (val > 20) + (val > 23) + (val > 29);
    return val;
  }
  // should only happen if a non-alphanumeric character was passed
  error_warn(ER_LEXDIGITBAD, lex_this);
}

// <extended_number> ::= <extended_digit> { <extended_digit> }
uint32_t parse_extended_number(uint32_t radix) {
  bool baddigit=false;
  bool overflow=false;
  char digit;
  uint32_t value = 0;

  // verify that it is a supported radix
  if ((radix < LEX_MINRAD) || (radix > LEX_MAXRAD))
    error_warn(ER_LEXBADRADIX, lex_this);
 
  // now we can read digits or letters
  while (lex_is_class(ch_next, DIG | LET)) {
    nextchar();
    digit = digit_of_char(ch_this);
    
    // verify digit is < radix
    baddigit = baddigit | (digit >= radix);
    
    // verify value won't overflow
    overflow = overflow || (value > (LEX_MAXVAL - digit) / radix);
    
    value = (value * radix) + digit;
  }

  if (baddigit) error_warn(ER_LEXDIGITRAD, lex_this);
  if (overflow) error_warn(ER_LEXOVERFLOW, lex_this);
  return value;
}

// <number> ::= <decimal_number> [ '#' <extended_number> ]
void parse_number() {
  bool baddigit;
  bool overflow=false;
  char digit;
  uint32_t radix = 10;
  uint32_t value;

  // handle the first character
  digit = digit_of_char(ch_this);
  baddigit = digit >= radix;
  value = digit;

  // read while the next ch is a digit
  while (lex_is_class(ch_next, DIG) || (ch_next == '#')) {
    if (ch_next == '#') {
      // value was actually the radix, now parse the rest
      nextchar(); // read the next number
      value = parse_extended_number(value);
      break;
    }
    nextchar(); // consume the current character and read the next
    digit = digit_of_char(ch_this);
    
    // verify the digit is < radix
    baddigit = baddigit || (digit >= radix); 

    // verify value won't overflow
    overflow = overflow || (value > ((LEX_MAXVAL - digit) / radix));
    
    // accumulate the value
    value = (value * radix) + digit;

  }
  lex_set(lex_next, NUMBER, value, lex_line, lex_col);
  
  // print some error messages if something went wrong
  if (baddigit) error_warn(ER_LEXDIGITRAD, lex_this);
  if (overflow) error_warn(ER_LEXOVERFLOW, lex_this);
}

/////////////////////////////////////////////////////////////////////
// strings
/////////////////////////////////////////////////////////////////////

// <string> ::= '"' { <anything_but_"> } '"' | "'" { <anything_but_'} "'"
void parse_string() {
  int quote_type = ch_this;
  symbol_start();
  nextchar();

  // read until we find the closing quote symbol
  while (ch_this != quote_type) {
    symbol_append(ch_this);
    nextchar();
    if (ch_this == '\n')
      error_fatal(ER_LEXSTRNL, lex_this);
    if (ch_this == EOF)
      error_fatal(ER_LEXSTREOF, lex_this);
  }

  // get the symbol handle
  symbol_handle h = symbol_lookup();
  lex_set(lex_next, STRING, h, lex_line, lex_col);
}

/////////////////////////////////////////////////////////////////////
// identifiers
/////////////////////////////////////////////////////////////////////

// <identifier> ::= <letter> { <letter> | <digit> }
void parse_ident() {
  symbol_start();
  symbol_append(ch_this);

  // read until we find a character that can't be in the identifier
  while (lex_is_class(ch_next, LET | DIG)) {
    nextchar();
    symbol_append(ch_this)
  }

  // get the symbol handle
  symbol_handle h = symbol_lookup();
  keyword_handle k = keyword_lookup(h);
  
  // determine if it is a keyword or identifier
  if( k != KEY_INVALID ){
    lex_set(lex_next, KEYWORD, k, lex_line, lex_col);
  }
  else
    lex_set(lex_next, IDENT, h, lex_line, lex_col);
}

/////////////////////////////////////////////////////////////////////
// whitespace / comments/ unknown characters
/////////////////////////////////////////////////////////////////////

// <comment> ::= "--" { <anything_but_newline> }
void parse_comment() {
  while ((ch_next != '\n') && (ch_next != EOF))
    nextchar();
}

// <white_space> ::= <space> | <tab> | <newline> | <comment> | <newline>
void parse_whitespace() {
  //printf("parse whitespace\n");
  while (lex_is_class(ch_this, WIT) || ((ch_this == '-') && (ch_next == '-'))) {
    if (ch_this == '\n') {
      lex_line += 1;
      lex_col = 0;
    }
    else if (ch_this == '-')
      parse_comment();
    nextchar();
  }
}

// reads through characters that we don't know how to handle
void parse_unknown() {
  symbol_start();
  symbol_append(ch_this);
  while (char_class[ch_next] == OTH) {
    nextchar();
    symbol_append(ch_this);
  }
  symbol_handle h = symbol_lookup();
  lex_set(lex_next, UNKNOWN, h, lex_line, lex_col);
}

/////////////////////////////////////////////////////////////////////
// here's where things actually happen
/////////////////////////////////////////////////////////////////////

// <kestrel program> ::= { { <white_space> } <lexeme> }
// <lexeme> ::= <identifier> | <number> | <string> | 
//              <punctuation> | <end_of_file>
void lex_advance() {
  // can't advance if we're at the end of the file...
  if (lex_this.type == ENDFILE) 
    error_fatal(ER_LEXEOF, lex_this);

  // current lexeme is now the old next lexeme
  lex_copy(lex_this, lex_next);
  
  // nothing left to do if current lexeme is EOF
  if (lex_this.type == ENDFILE)
    return;

  // read through leading whitespace
  parse_whitespace();
  
  // check to see if we're at the end of file
  if (ch_this == EOF) {
    lex_next.type = ENDFILE;
    fclose(infile);
    return;
  }

  // check for an identifier
  if (lex_is_class(ch_this, LET))
    parse_ident();
  
  // check for a number
  else if (lex_is_class(ch_this, DIG))
    parse_number();
  
  // check for a string
  else if ((ch_this == '"') || (ch_this == '\''))
    parse_string();

  // check for punctuation
  else if (lex_is_class(ch_this, PUN))
    parse_punctuation();
  
  // delve into the unknown
  else
    parse_unknown();

  // consume the character just handled
  nextchar();
}

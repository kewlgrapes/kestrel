// lexical.h : Kestrel lexical analyzer interface spec
//   author : D.W.Jones

#ifndef EXTERN
  #define EXTERN extern
#endif

// the lexeme type

typedef enum lex_type { 
  IDENT, KEYWORD, NUMBER, STRING, PUNCT, ENDFILE, UNKNOWN 
} lex_type;

typedef struct lexeme {
  lex_type type;
  uint32_t value;
  int line;
  int col;
} lexeme;

// global lexical variables

EXTERN lexeme lex_this; // the current lexeme
EXTERN lexeme lex_next; // the next lexeme
EXTERN int lex_line;    // current line number
EXTERN int lex_col;     // current column

// types of characters
typedef enum char_type { 
  OTH=0, /* other */
  WIT=1, /* whitespace */
  LET=2, /* letter */
  DIG=4, /* digit */
  PUN=8  /* punctuation */
} char_type;

// primary interface

// open lexical analyzer to read from a file named f, or stdin if f is null
void lex_open( char * f );

// advance lex_this and lex_next one lexeme
void lex_advance();

// output lexeme to f with no decorations
void lex_put( lexeme * lex, FILE * f );

// forces current lexeme to be type t (otherwise complains)
void lex_force( lex_type t );

// same as previous, but with argument to pass to expected_but_got()
void lex_force( lex_type t, uint32_t v, const char *exp = "you should specify a string to display here when you call lex_force() XD");

// returns true if the lexeme lex is the requested type and value
// bool lex_is( lexeme lex, lex_type t, uint_32t v );
#define lex_is( lex, t, v ) (((lex).type == (t)) && ((lex).value == (v)))

// returns true if the lexeme lex is of the requested type
// bool lex_is_type (lexeme lex, lex_type t)
#define lex_is_type( lex, t ) ((lex).type == (t))

// returns true if lexeme lex is in the set
// bool lex_in_set(lexeme lex, set32_t set)
#define lex_in_set( lex, t, set ) ( \
  ((lex).type == (t)) &&            \
  (IN_SET32( (lex).value, set ) ) ) 

// returns true if the char ch is of the char_type cls
// bool lex_is_class(char ch, char_type cls);
#define lex_is_class(ch,cls) (ch == -1 ? 0 : char_class[ch]&(cls))

// convenience define to set attributes of a lexeme
// void lex_set(lexeme lex, lex_type t, uint32_t v)
#define lex_set(lex, t, v, l, c) { lex.type = t;  \
                                   lex.value = v; \
                                   lex.line = lex_line;  \
                                   lex.col = lex_col; }

// convenience define to copy attributes from lex2 to lex1
// void lex_copy(lexeme *lex1, lexeme *lex2)
#define lex_copy(lex1, lex2) { lex1.type = lex2.type;   \
                               lex1.value = lex2.value; \
                               lex1.line = lex2.line;   \
                               lex1.col = lex2.col; }
#undef EXTERN

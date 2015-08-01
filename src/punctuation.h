// punctuation.h : user interface specifications for the punctuation table

#ifndef EXTERN
  #define EXTERN extern
#endif

typedef uint32_t punct_handle;

typedef enum punct_type {
  PT_EQUALS,  PT_COLON,
  PT_LPAREN,  PT_RPAREN,
  PT_LBRAKT,  PT_RBRAKT,
  PT_LBRACE,  PT_RBRACE,
  PT_COMMA,   PT_ATSIGN,
  PT_ELIPS,   PT_NOTEQL,
  PT_GT,      PT_GE,
  PT_LT,      PT_LE,
  PT_PLUS,    PT_MINUS,
  PT_TIMES,   PT_DIV,
  PT_AND,     PT_OR,
  PT_DOT,     PT_SEMIC,
  PT_TILDE
} punct_type;

typedef struct {
  punct_handle p;
  char ch;
} punct_elem;

EXTERN punct_elem _punct_table[PUNCT_HASH_SIZE];

// maps elements of punct_type to character strings
extern const char *punct_val[PUNCT_HASH_SIZE];

void punct_init();

// returns punct_type if ch represents a known character
// otherwise returns PUNCT_INVALID
punct_handle punct_lookup(char ch);

// gets the closing symbol for the given grouping symbol ( '(' -> ')' etc )
punct_type punct_closing_symbol(punct_type opening);

// 2 collisions with hash size of 64
#define punct_hash( ch ) (((ch * ch) >> 2) % PUNCT_HASH_SIZE)

#define punct_put( k, f ) { fprintf(f, "%s", punct_val[k]);}

#undef EXTERN

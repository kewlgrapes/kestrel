// config.h : configuration options for Kestrel compiler
//   author : Douglas Jones
//   modified by : members of the Venture group

// locale
#define LOC_FLAGCHAR '-'


// code generation
#define CODE_MINREG 3
#define CODE_MAXREG 12
#define CODE_NUMREGS 10


// string pool
#define POOL_SIZE 40000     // 40 KB


// symbol table
#define SYMBOL_HASH 64
#define SYMBOL_SIZE 5001
#define SYMBOL_INVALID -1


// keywords
#define KEY_REHASH_SIZE 64
#define KEY_MIN 0
#define KEY_MAX 26
#define KEY_INVALID -1

// punctuation
#define PUNCT_HASH_SIZE 64
#define PUNCT_MIN 0
#define PUNCT_MAX 24
#define PUNCT_INVALID -1

// lexical
#define LEX_MAXVAL 0x7FFFFFFF
#define LEX_MINVAL 0x80000000
#define LEX_MINRAD 2
#define LEX_MAXRAD 32

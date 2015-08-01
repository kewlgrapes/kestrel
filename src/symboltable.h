// symboltable.h : user interface specifications for the symbol table.
//   author : Douglas Jones

#ifndef EXTERN
  #define EXTERN extern
#endif

typedef uint32_t symbol_handle;
// the symbol handle type.  C and C++ don't let us be
// more specific, but it is actually values between
// 0 and SYMBOL_SIZE-1, inclusive.

EXTERN string_handle _symbol_table[ SYMBOL_SIZE ];
// a private definition:  Nobody outside this package should use this

EXTERN void symbol_init();
// initializer for the symbol table package
// -- a real function, it must set all entries in the symbol table

EXTERN symbol_handle _symbol_hash;
// the hash code of the symbol currently being accumulated
// a private definition:  Nobody outside this package should use this

EXTERN string_handle _symbol_string;
// the tentative string handle for the symbol being accumulated.
// a private definition:  Nobody outside this package should use this

EXTERN symbol_handle symbol_define(const char *s);
//Function that will return the symbol_handle to the parsed string.

//void symbol_start();
#define symbol_start() { _symbol_hash = 0; _symbol_string = string_start(); }
// setup to accumulate one string into the symbol table

//void symbol_append( char ch )
#define symbol_append(ch) {                            \
        _symbol_hash = ((_symbol_hash * SYMBOL_HASH ) \
                        + (ch)) % SYMBOL_SIZE;         \
        string_append(ch);                             \
}
// append one character to the symbol currently being accumulated

symbol_handle symbol_lookup();
// finish the lookup of the symbol currently being accumulated
// this looks it up in the hashed symbol table and returns its handle
// if the symbol is new, it is added to the table and retained in the pool

// note:
// to add a symbol to the table
//   symbol_start()
//   for each character ch in the string { symbol_append(ch) }
//   handle = symbol_lookup()

//void symbol_put( symbol_handle s, FILE * f );
#define symbol_put( s, f ) { string_put( _symbol_table[ s ], f ); }
// output the symbol to the human-readable file

/*BUG*/ // the code generator will need a way to put symbols into object files
/*BUG*/ // perhaps symbol_chars(s,thunk()) calls string_chars().

#undef EXTERN

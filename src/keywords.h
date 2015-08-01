// keywords.h : user interface specifications for the keyword table
//   author : Ken Jakubzak
//   modified by : members of the Venture group

#ifndef EXTERN
  #define EXTERN extern
#endif

typedef uint32_t symbol_handle;
typedef uint32_t keyword_handle;

typedef enum key_type {
  KEY_ARRAY  /* array     */,  KEY_OF    /* of        */,
  KEY_CASE   /* case      */,  KEY_NULL  /* null      */,
  KEY_CONST  /* const     */,  KEY_PRIV  /* private   */,
  KEY_DO     /* do        */,  KEY_PROC  /* prodedure */,
  KEY_ELSE   /* else      */,  KEY_RAISE /* raise     */,
  KEY_END    /* end       */,  KEY_REC   /* record    */,
  KEY_ENUM   /* enum      */,  KEY_SEL   /* select    */,
  KEY_EXCEPT /* exception */,  KEY_THEN  /* then      */,
  KEY_EXT    /* external  */,  KEY_TRY   /* try       */,
  KEY_FOR    /* for       */,  KEY_TYPE  /* type      */,
  KEY_FUNC   /* function  */,  KEY_UNTIL /* until     */,
  KEY_HAND   /* handle    */,  KEY_VAR   /* var       */,
  KEY_IF     /* if        */,  KEY_WHILE /* while     */,
  KEY_IN     /* in        */
} key_type;

typedef struct {
  keyword_handle key;
  symbol_handle sym;
} key_elem;

// a table relating key_types to symbol handles
// the index is the keyword's "keyword_handle"
EXTERN key_elem _key_table[KEY_REHASH_SIZE];

extern const char* keyword_val[KEY_REHASH_SIZE];

EXTERN void keyword_init();

// tries to find a keyword related to a symbol handle s
// returns KEY_INVALID if unsuccesful
EXTERN keyword_handle keyword_lookup ( symbol_handle s );

// key_handle keyword_hash(symbol_handle s)
#define keyword_hash( s ) ((s) % KEY_REHASH_SIZE)
// rehash function for symbol handles of keywords

// void keyword_put( key_handle k, FILE * f );
#define keyword_put( k, f ) { fprintf( f, "%s", keyword_val[ k ] ); }
// output the keyword to the human-readable file

#undef EXTERN

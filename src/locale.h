// locale.h : interface specification and configuration for locale.c
//   author : Douglas Jones
//   modified by : members of the Venture group

// locale settings //
extern const char *LOC_DEFAULTNAME;

// to encode help messages for various topics
typedef enum help_code {
  HLP_DEFAULT
} help_code;

// print help information for various topics
void print_help(help_code topic);
void print_help();

// error management //

// an enumeration type to encode the error messages
typedef enum error_code {
  // code generation
  ER_BADOUTFILE,

  // punctuation
  ER_INVOPEN,

  // string pool errors
  ER_POOLBOUNDS,
  ER_POOLFULL,

  // symbol table errors
  ER_HASHFULL,

  // lexical errors
  ER_BADFILE,
  ER_LEXEME,
  ER_LEXSTRNL,
  ER_LEXSTREOF,
  ER_LEXUNK,
  ER_LEXOVERFLOW,
  ER_LEXDIGITRAD,
  ER_LEXDIGITBAD,
  ER_LEXBADRADIX,
  ER_LEXEOF,

  // kestrel errors
  ER_NOINPUT,
  ER_OPTION,
  ER_MULTOUT
} error_code;

// fatal errors

// output error message to stderr and terminate
void error_fatal( error_code er, const char *origin=LOC_DEFAULTNAME );

void error_fatal( error_code er, lexeme lex, const char *origin=LOC_DEFAULTNAME );

void error_undeclared( lexeme lex, const char *origin=LOC_DEFAULTNAME );

// warnings

// output error message to stderr
void error_warn( error_code er, lexeme lex, const char *origin=LOC_DEFAULTNAME );

void error_warn( error_code er, const char *origin=LOC_DEFAULTNAME );

void expected_but_got(const char *exp, lexeme lex, const char *origin=LOC_DEFAULTNAME);

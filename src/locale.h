// locale.h : interface specs for error reporting / locale settings
//   author : Douglas JOnes
//   modified : Nick Becker

// compiler name
extern const char *LOC_DEFAULTNAME;

// to encode help messages
typedef enum help_code {
  HLP_DEFAULT
} help_code;

// to encode error messages
typedef enum error_code {
  // string pool
  ER_POOLBOUNDS,
  ER_POOLFULL,

  // symbol table
  ER_HASHFULL,

  // punctuation
  ER_INVOPEN,

  // keywords
  // ???

  // lexical
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
} error_code;

// print help information for topic
void print_help(help_code topic);

// warnings
void error_warn(error_code er, const char *origin=LOC_DEFAULTNAME);

void error_warn(error_code er, lexeme lex, const char *origin=LOC_DEFAULTNAME);

void expected_but_got(const char *exp, lexeme lex, const char *origin=LOC_DEFAULTNAME);

// fatal errors
void error_fatal(error_code er, const char *origin=LOC_DEFAULTNAME);

void error_fatal(error_code er, lexeme lex, const char *origin=LOC_DEFAULTNAME);

void error_undeclared(lexeme lex, const char *origin=LOC_DEFAULTNAME);

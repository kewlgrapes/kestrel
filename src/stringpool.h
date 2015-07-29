// stringpool.h : user interface specifications for the stirng pool
//   author : Douglas Jones
//   modified : Nick Becker

#ifndef EXTERN
  #define EXTERN extern
#endif

// takes on values between 0 and POOL_SIZE - 1
typedef uint32_t string_handle;

// buffer for strings in _string_pool
EXTERN char _string_pool[POOL_SIZE];

// index of first unused location in _string_pool
EXTERN string_handle _string_limit;

// position to store new characters added to _string_pool
EXTERN string_handle _string_pos;

// initializes string pool
// STRING_NULL at position 0 in _string_pool
#define string_init() {_string_limit = 1;}

// setup to accumulate a new string and return its handled
#define string_start() (_string_pos = _string_limit, _string_limit)

// returns true if a string is being accumulated
#define string_pool_adding() (_string_pos > _string_limit)

// append one character to string
#define string_append(ch) {                         \
  if (_string_pos > POOL_SIZE - 2) error_fatal(ER_POOLFULL); \
  _string_pool[ _string_pos++] = ch;                \
}

// mark end of the string
#define string_done() {                                        \
  if (_string_pos > POOL_SIZE - 1) error_fatal(ER_POOLFULL); \
  _string_pool[_string_pos] = '\0'; \
  _string_pos++;                    \
}

// accept new string
#define string_accept() {_string_limit = _string_pos;}

// reject new string
#define string_reject() {_string_pos = _string_limit;}

// make sure string handle h is in bounds of pool
# define check_bounds(h) {if (h > POOL_SIZE -1) error_fatal(ER_POOLBOUNDS);}

// output the string to human readable file
void string_put(string_handle h, FILE *f);

// compare the string h1 and h2, return values similar to strcmp()
int string_eq(string_handle h1, string_handle h2);

// returns the length of the string h
int string_len(string_handle h);

#undef EXTERN

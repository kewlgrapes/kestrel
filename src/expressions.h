// expressions.h : classes and methods for parsing expressions
//   author : members of the venture group

// forward references
class Block;
class Declaration;

// this file declares...
class Value;
class Term;
class Factor;
class Comparand;
class Expression;
class Identifier;
class Reference;

Expression *parse_expression(Block *context);
Reference *parse_reference(Block *context);
Identifier *parse_identifier(Block *context);

///////////////////////////////////////////////////////////
// OPERATORS
///////////////////////////////////////////////////////////

// to multiply factors
typedef enum {
  MULT_TIMES,
  MULT_DIV,
  MULT_AND,
  MULT_NULL
} mult_op;

// for factors
typedef enum {
  UN_NEG,
  UN_NOT,
  UN_NULL
} un_op;

// to add terms
typedef enum {
  ADD_PLUS,
  ADD_MINUS,
  ADD_OR,
  ADD_NULL
} add_op;

// to compare two comparands
typedef enum {
  CMP_EQUAL,
  CMP_NEQUAL,
  CMP_GT,
  CMP_GE,
  CMP_LT,
  CMP_LE,
  CMP_NULL
} cmp_op;

///////////////////////////////////////////////////////////
// SETS
///////////////////////////////////////////////////////////

extern set32_t UN_OP_SET;
extern set32_t MULT_OP_SET;
extern set32_t ADD_OP_SET;
extern set32_t CMP_OP_SET;
extern set32_t GROUPING_SET;
extern set32_t EXPR_START_SET;

#define in_expr_start_set(lex)                  \
  ( lex_in_set((lex), PUNCT, EXPR_START_SET) || \
    lex_is_type((lex), IDENT)                || \
    lex_is_type((lex), NUMBER)               || \
    lex_is_type((lex), STRING)               || \
    lex_is((lex), KEYWORD, KEY_NULL) )

#define force_expr_start_set(lex)                   \
  if (!in_expr_start_set(lex))                      \
    expected_but_got("start of expression", (lex));

///////////////////////////////////////////////////////////
// CLASSES
///////////////////////////////////////////////////////////

// types of values
typedef enum {
  VAL_NUM,
  VAL_STR,
  VAL_REF,
  VAL_SUBEXPR,
  VAL_NULL
} val_type;

// base unit for expressions
class Value {
  public:
    Factor *factor; // parent of this value
    val_type type;  // type of value
    un_op op;       // unary operator to apply to this value

    lexeme lex;       // if value is from a lexeme
    Reference *ref;   // if value is a reference
    Expression *expr; // if value is a subexpression

    list<lexeme> more_strings; // a string value can be the concatentation
                               //   of many string literals

    Value(Factor *f, lexeme l, val_type t, un_op o = UN_NULL);
    Value(Factor *f, Reference *r, un_op o = UN_NULL);
    Value(Factor *f, Expression *e, un_op o = UN_NULL);

    void put(FILE *f);
};

// base unit for multiplications
class Factor {
  public:
    Value *value; // value of this factor
    mult_op op;   // prev factor times this value
    
    Term *term;   // parent of this value
    
    Factor(Term *t, mult_op o = MULT_NULL);
    void put(FILE *f);
};

// base unit for additions
class Term {
  public:
    Comparand *comparand; // parent of this term
    add_op op;            // prev term plus this value
    
    list<Factor*> factors; // list of factors that make up this term
    
    Term(Comparand *c, add_op o = ADD_NULL);
    void put(FILE *f);
};

// base unit for comparisons
class Comparand {
  public:
    Expression *expr; // parent of this comparand
    cmp_op op;        // prev comparand compared to this comparand
    
    list<Term*> terms; // list of terms that make up this comparand
    
    Comparand(Expression *e, cmp_op o = CMP_NULL);
    void put(FILE *f);
};

class Expression {
  public:
    int *evaluated;
    list<Comparand*> comparands;

    Expression();
    Expression(int e);
    void put(FILE *f);
};

class Identifier {
  public:
    lexeme lex;
    Declaration *dec;

    Identifier(lexeme l);
    Identifier(const char *str);
    void put(FILE *f);
};

typedef enum {
  REF_IDENT,
  REF_POINT,
  REF_FIELD,
  REF_FUNC
} ref_type;

class Reference {
  public:
    Identifier *ident;
    Reference *ref;
    list<Expression*> expressions;
    ref_type type;

    Reference(Identifier *i);
    Reference(Reference *r, Identifier *i);
    Reference(Reference *r, ref_type = REF_POINT);
    void put(FILE *f);
};

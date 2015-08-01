// expressions.cpp : classes and methods for parsing expressions
//   author : members of the venture group
using namespace std;
#include <list>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

#include "sets.h"
#include "config.h"
#include "lexall.h"
#include "locale.h"

#include "blocks.h"
#include "expressions.h"
#include "codegenerator.h"

// these correspond to enums in expressions.h (for printing)

static const char mult_val[4] = {'*', '/', '&', ' '};
static const char un_val[3]   = {'-', '~', ' '};
static const char add_val[4]  = {'+', '-', '|', ' '};
static const char *cmp_val[7] = {"=", "<>", ">", ">=", "<", "<=", ""};

///////////////////////////////////////////////////////////
// SETS
///////////////////////////////////////////////////////////

set32_t UN_OP_SET = TO_SET32_2(PT_MINUS, PT_TILDE);

set32_t MULT_OP_SET = TO_SET32_3(PT_TIMES, PT_DIV, PT_AND);

set32_t ADD_OP_SET = TO_SET32_3(PT_PLUS, PT_MINUS, PT_OR);

set32_t CMP_OP_SET = TO_SET32_3(PT_EQUALS, PT_NOTEQL, PT_GT) |
                     TO_SET32_3(PT_GE, PT_LT, PT_LE);

set32_t GROUPING_SET = TO_SET32_3(PT_LPAREN, PT_LBRAKT, PT_LBRACE);

set32_t EXPR_START_SET = UN_OP_SET | GROUPING_SET;

///////////////////////////////////////////////////////////
// CLASSES
///////////////////////////////////////////////////////////

Value::Value(Factor *f, lexeme l, val_type t, un_op o) {
  factor = f;
  lex_copy(lex, l);
  type = t;
  op = o;
  f->value = this;
}
Value::Value(Factor *f, Reference *r, un_op o) {
  factor = f;
  type = VAL_REF;
  ref = r;
  op = o;
  f->value = this;
}
Value::Value(Factor *f, Expression *e, un_op o) {
  factor = f;
  type = VAL_SUBEXPR;
  expr = e;
  op = o;
  f->value = this;
}
void Value::put(FILE *f) {
  list<lexeme>::iterator s; 
  
  if (op != UN_NULL) fputc(un_val[op], f);
  switch (type) {
    case VAL_NUM:
      fprintf(f, "%d", lex.value);
      break;
    case VAL_STR:
      fputc('"', f);
      symbol_put(lex.value, f);
 
      s = more_strings.begin();
      while (s != more_strings.end()) {
        symbol_put((*s).value, f);
        s++;
      }

      fputc('"', f);
      break;
    case VAL_REF:
      ref->put(f);
      break;
    case VAL_SUBEXPR:
      expr->put(f);
      break;
    case VAL_NULL:
      fputs("Null", f);
      break;
  }
}

Factor::Factor(Term *t, mult_op o) { 
  term = t; 
  op = o;
  t->factors.push_back(this);
}
void Factor::put(FILE *f) {
  if (value) value->put(f);
}

Term::Term(Comparand *c, add_op o) { 
  comparand = c;
  op = o;
  c->terms.push_back(this);
}
void Term::put(FILE *f) {
  if (factors.size() > 1) fputc('(', f);

  list<Factor*>::iterator fact;
  fact = factors.begin();
  if (fact != factors.end()) {
    (*fact)->put(f);
    while (++fact != factors.end()) {
      fputc(mult_val[(*fact)->op], f);
      (*fact)->put(f);
    }
  }
  
  if (factors.size() > 1) fputc(')', f);
}

Comparand::Comparand(Expression *e, cmp_op o) {
  expr = e;
  op = o;
  e->comparands.push_back(this);
}
void Comparand::put(FILE *f) {
  if (terms.size() > 1) fputc('(', f);
  list<Term*>::iterator t;
  t = terms.begin();
  if (t != terms.end()) {
    (*t)->put(f);
    while (++t != terms.end()) {
      fputc(add_val[(*t)->op], f);
      (*t)->put(f);
    }
  }
  if (terms.size() > 1) fputc(')', f);
}

Expression::Expression() { evaluated = NULL; }
Expression::Expression(int e) {
  evaluated = (int *)malloc(sizeof(int));
  *evaluated = e;
}
void Expression::put(FILE *f) {
  if (evaluated) {
    fprintf(f, "%d", *evaluated);
    return;
  }

  if (comparands.size() > 1) fputc('(', f);

  list<Comparand*>::iterator c;
  c = comparands.begin();
  if (c != comparands.end()) {
    (*c)->put(f);
    while (++c != comparands.end()) {
      fputs(cmp_val[(*c)->op], f);
      (*c)->put(f);
    }
  }

  if (comparands.size() > 1) fputc(')', f);
}

Identifier::Identifier(lexeme l) { lex_copy(lex, l); }
Identifier::Identifier(const char *str) {
  lex.type = STRING;
  lex.value = symbol_define(str);
}
void Identifier::put(FILE *f) {
  fputc('_', f);
  symbol_put(lex.value, f);
  fputc('_', f);
}

Reference::Reference(Identifier *i) {
  ident = i;
  type = REF_IDENT;
}
Reference::Reference(Reference *r, Identifier *i) {
  ident = i;
  ref = r;
  type = REF_FIELD;
}
Reference::Reference(Reference *r, ref_type t) {
  ref = r;
  type = t;
}
void Reference::put(FILE *f) {
  switch (type) {
    case REF_IDENT:
      ident->put(f);
      break;
    case REF_POINT:
      ref->put(f);
      fputc('@', f);
      break;
    case REF_FIELD:
      ref->put(f);
      fputc('.', f);
      ident->put(f);
      break;
    case REF_FUNC:
      ref->put(f);
      fputc('(', f);
      
      list<Expression*>::iterator e;
      e = expressions.begin();
      if (e != expressions.end()) {
        (*e)->put(f);
        while (++e != expressions.end()) {
          fputc(',', f);
          (*e)->put(f);
        }
      }
      fputc(')', f);
      break;
  }
}

///////////////////////////////////////////////////////////
// FUNCTIONS
///////////////////////////////////////////////////////////

Value *parse_value(Block *context, Factor *f) {
  Value *v;
  if lex_is_type(lex_this, NUMBER) {
    v = new Value(f, lex_this, VAL_NUM);
    lex_advance();
  }
  else if lex_is_type(lex_this, STRING) {
    v = new Value(f, lex_this, VAL_STR);
    while (lex_is_type(lex_next, STRING)) {
      v->more_strings.push_back(lex_next);
      lex_advance();
    }
    lex_advance();
  }
  else if (lex_is(lex_this, KEYWORD, KEY_NULL)) {
    v = new Value(f, lex_this, VAL_NULL);
    lex_advance();
  }
  else if (lex_is_type(lex_this, IDENT)) {
    Reference *r = parse_reference(context);
    v = new Value(f, r);
  }
  else if (lex_in_set(lex_this, PUNCT, GROUPING_SET)) {
    // get the closing symbol
    uint32_t c = punct_closing_symbol((punct_type)lex_this.value); 
    lex_advance();

    // parse the sub expression
    Expression *e = parse_expression(context);
    v = new Value(f, e);

    // make sure a closing symbol is present
    lex_force(PUNCT, c, punct_val[c]);
    lex_advance();
  }
  else
    expected_but_got("number, string, \"null\", or start of expression", lex_this);
  return v; 
}

Factor *parse_factor(Block *context, Term *t) {
  Factor *f = new Factor(t);
  Value *v;

  // check for a unary operator
  if lex_is(lex_this, PUNCT, PT_MINUS) {
    lex_advance();
    v = parse_value(context, f);
    v->op = UN_NEG;
  }
  else if lex_is(lex_this, PUNCT, PT_TILDE) {
    lex_advance();
    v = parse_value(context, f);
    v->op = UN_NOT;
  }
  else {
    v = parse_value(context, f);
  }
  return f;
}

Term *parse_term(Block *context, Comparand *c) {
  Term *t = new Term(c);
  Factor *f = parse_factor(context, t);
  
  // a term contains multiple factors
  mult_op o;
  while (lex_in_set(lex_this, PUNCT, MULT_OP_SET)) {
    if (lex_is(lex_this, PUNCT, PT_TIMES)) o = MULT_TIMES;
    else if (lex_is(lex_this, PUNCT, PT_DIV)) o = MULT_DIV;
    else o = MULT_AND;
    lex_advance();
    f = parse_factor(context, t);
    f->op = o;
  }
  return t;
}

Comparand *parse_comparand(Block *context, Expression *e) {
  Comparand *c = new Comparand(e);
  Term *t = parse_term(context, c);

  // a comparand contains many terms
  add_op o;
  while (lex_in_set(lex_this, PUNCT, ADD_OP_SET)) {
    if (lex_is(lex_this, PUNCT, PT_PLUS)) o = ADD_PLUS;
    else if (lex_is(lex_this, PUNCT, PT_MINUS)) o = ADD_MINUS;
    else o = ADD_OR;
    lex_advance();
    t = parse_term(context, c);
    t->op = o;
  }
  return c;
}

Expression *parse_expression(Block *context) {
  Expression *e = new Expression();
  Comparand *c = parse_comparand(context, e);

  // an expression contains many comparands
  cmp_op o;
  while (lex_in_set(lex_this, PUNCT, CMP_OP_SET)) {
    switch (lex_this.value) {
      case PT_EQUALS: o = CMP_EQUAL;  break;
      case PT_NOTEQL: o = CMP_NEQUAL; break;
      case PT_GT:     o = CMP_GT;     break;
      case PT_GE:     o = CMP_GE;     break;
      case PT_LT:     o = CMP_LT;     break;
      case PT_LE:     o = CMP_LE;     break;
    }
    lex_advance();
    c = parse_comparand(context, e);
    c->op = o;
  }
  return e;
}

Identifier *parse_identifier(Block *context) {
  lex_force(IDENT);
  Identifier *i = new Identifier(lex_this);
  lex_advance();
  return i;
}

/*
x x@ x@@ x@.x@@ x.y x.y.z x.y.z(3) x@@@ x@.y x[1].y
*/
Reference *parse_reference(Block *context, Reference *r) {
  Identifier *i;
  if (lex_is(lex_this, PUNCT, PT_ATSIGN)) {
    lex_advance();
    r = parse_reference(context, new Reference(r, REF_POINT));
  }
  else if (lex_is(lex_this, PUNCT, PT_DOT)) {
    lex_advance();
    i = parse_identifier(context);
    r = parse_reference(context, new Reference(r, i));
  }
  else if (lex_in_set(lex_this, PUNCT, GROUPING_SET)) {
    // get closing symbol
    uint32_t c = punct_closing_symbol((punct_type)lex_this.value);
    lex_advance();

    // an expression must follow
    Expression *e = parse_expression(context);
    r = new Reference(r, REF_FUNC);
    r->expressions.push_back(e);
    
    // there can be more 
    while (lex_is(lex_this, PUNCT, PT_COMMA)) {
      lex_advance();
      e = parse_expression(context);
      r->expressions.push_back(e);
    }
    
    // make sure closing symbol is present
    lex_force(PUNCT, c, punct_val[c]);
    lex_advance();
    r = parse_reference(context, r);
  }
  return r;
}
Reference *parse_reference(Block *context) {
  Identifier *i = parse_identifier(context);
  if (!context->lookup(i))
    error_undeclared(i->lex);
  return parse_reference(context, new Reference(i));
}

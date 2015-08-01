// declarations.cpp : classes and methods for parsing declarations
//   author : members of the venture group
using namespace std;
#include <stdio.h>
#include <stdint.h>
#include <list>

#include "config.h"
#include "sets.h"
#include "lexall.h"
#include "parseall.h"
#include "locale.h"

Parameter::Parameter(Identifier *i, Type *t, int ic){
  ident = i;
  type = t;
  is_const = ic;
}
void Parameter::put(FILE *f){
  fputs("Parameter(", f);
  if (is_const) 
    fputs("const", f);
  else 
    fputs("var", f);
  ident->put(f);
  fputc(':', f);
  type->put(f);
  fputc(')', f);
}

Declaration::Declaration(Block *b, Identifier *i, int ip): BlockElement( b ) {
  ident = i;
  is_private = ip;
}
void Declaration::put(FILE *f) { fputs("Declaration()", f); }

ConstDeclaration::ConstDeclaration(Block *b, Identifier *i, Expression *e, int ip) : Declaration(b, i, ip) {
  expr = e;
} 
void ConstDeclaration::put(FILE *f) {
  fputs("Const(", f);
  ident->put(f);
  fputc(',', f);
  expr->put(f);
  fputc(')', f);
}

TypeDeclaration::TypeDeclaration(Block *b, Identifier *i, Type *t, int ip) : Declaration(b, i, ip) {
  type = t;
}
void TypeDeclaration::put(FILE *f) {
  fputs("Type(", f);
  ident->put(f);
  fputc(':', f);
  type->put(f);
  fputc(')',f);
}

ExceptDeclaration::ExceptDeclaration(Block *b, Identifier *i, int ip) : Declaration(b, i, ip) { }
void ExceptDeclaration::put(FILE *f) { 
  fputs("Except(", f);
  ident->put(f);
  fputc(')', f);
}

VarDeclaration::VarDeclaration(Block *b, Identifier *i, Type *t, int ip) : Declaration(b, i, ip){ 
  type = t;
}
void VarDeclaration::put(FILE *f) {
  fputs("Var(", f);
  ident->put(f);
  fputc(',', f);
  type->put(f);
  fputc(')', f);
}

ProcDeclaration::ProcDeclaration(Block *b, Identifier *i, int ip) : Declaration(b, i, ip) { }
void ProcDeclaration::put(FILE *f) {
  fputs("Proc(", f);
  ident->put(f);
  fputc(',', f);
  list<Parameter*>::iterator p = parameters.begin();
  if (p != parameters.end()) {
    (*p)->put(f);
    while (++p != parameters.end()) {
      fputc(',', f);
      (*p)->put(f);
    }
  }
  fputc(',',f);
  if (body)
    body->put(f);
  else
    fputs("external", f);
  fputc(')',f);
}

FuncDeclaration::FuncDeclaration(Block *b, Identifier *i, Type *t, int ip) : ProcDeclaration(b, i, ip) { 
  type = t;  
}
void FuncDeclaration::put(FILE *f) {
  fputs("Func(", f);
  ident->put(f);
  fputc(',', f);
  type->put(f);
  fputc(',', f);

  list<Parameter*>::iterator p = parameters.begin();
  if (p != parameters.end()) {
    (*p)->put(f);
    while (++p != parameters.end()) {
      fputc(',', f);
      (*p)->put(f);
    }
  }
  fputc(',', f);
  if (body)
    body->put(f);
  else
    fputs("external", f);
  fputc(')',f);
}

///////////////////////////////////////////////////////////
// FUNCTIONS
///////////////////////////////////////////////////////////

void parse_parameters(Block *context, ProcDeclaration *p) {
  // get closing symbol
  uint32_t c;
  if (lex_in_set(lex_this, PUNCT, GROUPING_SET))
    c = punct_closing_symbol((punct_type)lex_this.value);
  else
    return;
  lex_advance();

  // parse parameters while we see an identifier
  Type *t;
  Identifier *i;
  while (lex_is_type(lex_this, IDENT)) {
    i = parse_identifier(context);
    lex_force(PUNCT, PT_COLON, ":");
    lex_advance();
    
    // <ident> : var <type>
    if (lex_is(lex_this, KEYWORD, KEY_VAR)) {
      lex_advance();
      t = parse_type(context);
      p->parameters.push_back(new Parameter(i, t, 0));
    }
    // <ident> : const <type>
    else if (lex_is(lex_this, KEYWORD, KEY_CONST)) {
      lex_advance();
      t = parse_type(context);
      p->parameters.push_back(new Parameter(i, t, 1));
    }
    else
      expected_but_got("var or const", lex_this);
    
    // more parameters...
    if (lex_is(lex_this, PUNCT, PT_COMMA))
      lex_advance();
  }

  // make sure closing symbol is present
  lex_force(PUNCT, c, punct_val[c]);
  lex_advance();
}

Declaration *parse_declaration(Block *context) {
  Identifier *i = parse_identifier(context);
  lex_advance(); // skip ":"

  // optionally private
  int ip = 0;
  if (lex_is(lex_this, KEYWORD, KEY_PRIV)) {
    ip = 1;
    lex_advance();
  }

  Expression *e;
  Type *t;
  Declaration *d;
  ProcDeclaration *p;

  lex_force(KEYWORD);
  switch (lex_this.value) {
    case KEY_CONST:
      lex_advance();
      force_expr_start_set(lex_this);
      e = parse_expression(context);
      d = new ConstDeclaration(context, i, e, ip);
      break;
    case KEY_TYPE:
      lex_advance();
      t = parse_type(context);
      d = new TypeDeclaration(context, i, t, ip);
      break;
    case KEY_EXCEPT:
      lex_advance();
      d = new ExceptDeclaration(context, i, ip);
      break;
    case KEY_VAR:
      lex_advance();
      t = parse_type(context);
      d = new VarDeclaration(context, i, t, ip);
      break;
    case KEY_PROC:
      lex_advance();
      p = new ProcDeclaration(context, i, ip);
      parse_parameters(context, p);
      if (lex_is(lex_this, KEYWORD, KEY_EXT)) 
        lex_advance();
      else {
        p->body = parse_block(context);
        lex_force(KEYWORD, KEY_END, "end");
        lex_advance();
      }
      d = p;
      break;
    case KEY_FUNC:
      lex_advance();
      t = parse_type(context);
      p = new FuncDeclaration(context, i, t, ip);
      parse_parameters(context, p);
      if (lex_is(lex_this, KEYWORD, KEY_EXT)) 
        lex_advance();
      else {
        p->body = parse_block(context);
        lex_force(KEYWORD, KEY_END, "end");
        lex_advance();
      }
      d = p;
      break;
    default:
      expected_but_got("\"const\", \"type\", \"except\", \"var\", \"proc\", \"func\"", lex_this);
  }
  return d;
}

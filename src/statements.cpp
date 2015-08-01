// statements.cpp : classes and methods for parsing statements
//   author : members of the venture group
using namespace std;
#include <list>
#include <stdio.h>
#include <stdint.h>

#include "config.h"
#include "sets.h"
#include "lexall.h"
#include "parseall.h"
#include "locale.h"

///////////////////////////////////////////////////////////
// CLASSES
///////////////////////////////////////////////////////////

Statement::Statement(Block *b): BlockElement(b) {
}
void Statement::put(FILE *f) { }

If::If(Block *b, Expression *e, Block *ib, Block *eb): Statement(b) {
  expr = e;
  if_block = ib;
  else_block = eb;
}
void If::put(FILE *f) {
  fputs("If(", f);
  expr->put(f);
  fputc(',', f);
  if_block->put(f);
  fputc(',', f);
  if (else_block) {
    else_block->put(f);
    fputc(')', f);
  }
}

CaseBlock::CaseBlock(){
}
void CaseBlock::put(FILE *f) {
  fputs("CaseBlock(", stdout);
  list<CaseLabel*>::iterator cl;
  for (cl = caselabels.begin(); cl != caselabels.end(); cl++) {
    (*cl)->expr1->put(stdout);
    if ((*cl)->expr2) {
      fputs("..", stdout);
      (*cl)->expr2->put(stdout);
    }
  }
  fputc(',', stdout);
  block->put(stdout);
  fputc(')', stdout);
}

CaseLabel::CaseLabel(Expression *e1, Expression *e2) {
  expr1 = e1;
  expr2 = e2;
}

Case::Case(Block *b, Expression *e): Statement(b) {
  expr = e;
}
void Case::put(FILE *f) {
  fputs("Case(", f);
  expr->put(stdout);
  fputs(",(", stdout);
  list<CaseBlock*>::iterator cb;
  for (cb = caseblocks.begin(); cb != caseblocks.end(); cb++) {
    (*cb)->put(stdout);
    fputc(',', stdout);
  }
  if (else_block) {
    fputs("),", stdout);
    else_block->put(stdout);
  }
  fputc(')', stdout);
}

Loop::Loop(Block *b): Statement(b) {
}
void Loop::put(FILE *f) { }

WhileLoop::WhileLoop(Block *b, Expression *e, Block *l): Loop(b){
  expr = e; 
  loop_block = l;
}
void WhileLoop::put(FILE *f) {
  fputs("WhileLoop(", f);
  expr->put(stdout);
  fputc(',', stdout);
  loop_block->put(stdout);
  fputc(')', f);
}

UntilLoop::UntilLoop(Block *b, Expression *e, Block *l): Loop(b) {
  expr = e;
  loop_block = l;
}
void UntilLoop::put(FILE *f) {
  fputs("UntilLoop(", f);
  expr->put(stdout);
  fputc(',', stdout);
  loop_block->put(stdout);
  fputc(')', f);
}

ForLoop::ForLoop(Block *b, Identifier *i, Type *t, Block *l): Loop(b) {
  ident = i; 
  type = t;
  loop_block = l;
}
void ForLoop::put(FILE *f) {
  fputs("ForLoop(", f);
  ident->put(stdout);
  fputc(':', stdout);
  type->put(stdout);
  fputc(',', stdout);
  loop_block->put(stdout);
  fputc(')', f);
}

HandlerBlock::HandlerBlock(Block *b): Block(b) {
}
void HandlerBlock::put(FILE *f) {
  fputs("HandlerBlock((", stdout);
  list<Reference*>::iterator ref;
  for (ref = references.begin(); ref != references.end(); ref++) {
    (*ref)->put(stdout);
    fputc(',', stdout);
  }
  fputc(')', stdout);
  ((Block*)this)->put(stdout);
  fputc(')', stdout);
}

Handler::Handler(Block *b, Block *hb): Statement(b) {
  handler_block = hb;
}
void Handler::put(FILE *f) { 
  fputs("Handler(", f);
  list<HandlerBlock*>::iterator hb;
  for (hb = handlerblocks.begin(); hb != handlerblocks.end(); hb++) {
    (*hb)->put(stdout);
    fputc(')', stdout);
  }
  fputc(')', stdout);
}

Raise::Raise(Block *b, Reference *r): Statement(b) {
  ref = r;
}
void Raise::put(FILE *f) { 
  fputs("Raise(", f);
  ref->put(stdout);
  fputc(')', stdout);
}

Procedure::Procedure(Block *b , Reference *r): Statement(b) {
  ref = r;
}
void Procedure::put(FILE *f) {
  fputs("Procedure(", f);
  ref->put(stdout);
  fputc(')', stdout);
}

Assignment::Assignment(Block *b, Reference *r, Expression *e): Statement(b) {
  ref = r; 
  expr = e;
}
void Assignment::put(FILE *f) { 
  fputs("Assignment(", f);
  ref->put(stdout);
  fputc(',', stdout);
  expr->put(stdout);
  fputc(')', stdout);
}

///////////////////////////////////////////////////////////
// FUNCTIONS
///////////////////////////////////////////////////////////

Assignment *parse_assignment(Block *b) {
  Reference* r = parse_reference(b);
  lex_force(PUNCT, PT_EQUALS);
  lex_advance(); //skip equal sign
  Expression *e = parse_expression(b);
  return new Assignment(b,r,e);
}

Procedure *parse_procedure(Block *b) {
  Reference* r = parse_reference(b);
  return new Procedure(b,r);
}

If *parse_if(Block *b) {
  Block *ib, *eb;
  
  lex_force(KEYWORD, KEY_IF);
  lex_advance(); // skip 'if'
  
  Expression *e = parse_expression(b);

  // 'then' is optional
  if lex_is(lex_this, KEYWORD, KEY_THEN) lex_advance();
  ib = parse_block(b); //if block
  
  // else block is optional
  if lex_is(lex_this, KEYWORD, KEY_ELSE) {
    lex_force(KEYWORD, KEY_ELSE);
    lex_advance();
    eb = parse_block(b);
  }
  else {
    eb = NULL;	
  }
  // final 'end' is required
  lex_force(KEYWORD, KEY_END);
  lex_advance();
  return new If(b, e, ib, eb);
}

CaseLabel *parse_label(Block *b) {
  Expression *e1 = new Expression();
  Expression *e2 = new Expression();
  if (in_expr_start_set(lex_this)){
    e1 = parse_expression(b);
  }
  else {
    e1 = NULL;
  }
  if (lex_is(lex_this, PUNCT, PT_ELIPS)) lex_advance();
  if (in_expr_start_set(lex_this)){
    e2 = parse_expression(b);
  }
  else { 
    e2 = NULL; 
  }
  return new CaseLabel(e1,e2);
}

Case *parse_case(Block *b) {
  lex_force(KEYWORD, KEY_SEL);
  lex_advance(); // skip 'select'
  Expression *e = parse_expression(b);
  Case *c = new Case(b, e);
 
  // 'in' is optional
  if (lex_is(lex_this, KEYWORD, KEY_IN)) lex_advance();

  // any number of case blocks
  CaseBlock *cb = new CaseBlock();
  CaseLabel *cl;
  list<CaseLabel*> tempCL;
  while (lex_is(lex_this, KEYWORD, KEY_CASE)){
    lex_force(KEYWORD, KEY_CASE);
    lex_advance(); //skip case
    
    cl = parse_label(b);
    tempCL.push_back(cl);
    
    // each case can contain any number of case labels
    while (in_expr_start_set(lex_this) && ~lex_is(lex_this, PUNCT, PT_COLON)) {
      if (lex_is(lex_this, PUNCT, PT_COMMA)) lex_advance();
      cl = parse_label(b);
      tempCL.push_back(cl);
    }
    if (lex_is(lex_this, PUNCT, PT_COLON)) lex_advance();
    if (in_block_start_set(lex_this)){
      cb->block = parse_block(b);
      cb->caselabels = tempCL;
    }
    else {} // complain
    c->caseblocks.push_back(cb);
  }

  // an else block is optional
  if lex_is(lex_this, KEYWORD, KEY_ELSE) {
    //lex_force(KEYWORD, KEY_ELSE);
    lex_advance();
    Block *eb = parse_block(b);
    c->else_block = eb;
  }
  
  // final 'end' is required
  lex_force(KEYWORD, KEY_END);
  lex_advance();
  
  return c;
}

Raise *parse_raise(Block *b) {
  lex_force(KEYWORD, KEY_RAISE);
  lex_advance(); // skip raise
  Reference *r = parse_reference(b);
  return new Raise(b,r);
}

Handler *parse_handler(Block *b) {
  
  lex_force(KEYWORD, KEY_TRY);
  lex_advance(); // skip try
  list<HandlerBlock*> tempHB;

  Block *b2 = parse_block(b);
  Handler *h = new Handler(b,b2);
  list<Reference*> tempLL;
  while (lex_is(lex_this, KEYWORD, KEY_HAND)){
    lex_force(KEYWORD, KEY_HAND);
    lex_advance();// skip handle
    Reference *r = parse_reference(b);
    tempLL.push_back(r);
    while (in_expr_start_set(lex_this)){
      if(lex_is(lex_this, PUNCT, PT_COMMA)){
        lex_force(PUNCT,PT_COMMA);
        lex_advance();
      }
      Reference *r = parse_reference(b);
      tempLL.push_back(r);
    }
    lex_force(PUNCT,PT_COLON);
    lex_advance();
    Block *hb = parse_block(b);
    HandlerBlock *hab = new HandlerBlock(hb);
    hab->references = tempLL;
    h->handlerblocks.push_back(hab);
  }

  // an else block is optional
  if lex_is(lex_this, KEYWORD, KEY_ELSE) {
    Block *eb = parse_block(b);
    h->else_block = eb;
  }

  // final 'end' is required
  lex_force(KEYWORD, KEY_END);
  lex_advance();

  return h;

}

ForLoop *parse_for(Block *b) {
  lex_force(KEYWORD, KEY_FOR);
  lex_advance();
  Identifier *i = parse_identifier(b);
  lex_force(KEYWORD, KEY_IN);
  lex_advance();
  Type *t = parse_type(b);
  if (lex_is(lex_this, KEYWORD, KEY_DO)) lex_advance();
  Block *b2 = parse_block(b);
  lex_force(KEYWORD, KEY_END);
  lex_advance();

  return new ForLoop(b, i, t, b2);
}

UntilLoop *parse_until(Block *b) {
  lex_force(KEYWORD, KEY_DO);
  lex_advance();
  Block *b2 = parse_block(b);
  lex_force(KEYWORD, KEY_UNTIL);
  lex_advance();
  Expression *e = parse_expression(b);
  return new UntilLoop(b,e,b2);
}

WhileLoop *parse_while(Block *b) {
  lex_force(KEYWORD, KEY_WHILE);
  lex_advance();
  Expression *e = parse_expression(b);
  if (lex_is(lex_this, KEYWORD, KEY_DO)) lex_advance();
  Block *b2 = parse_block(b);
  lex_force(KEYWORD, KEY_END);
  lex_advance();
  return new WhileLoop(b,e,b2);
}

Statement *parse_statement(Block *b) {
  Statement *s;
  if (lex_this.type == IDENT) {
    if (lex_is(lex_next, PUNCT, PT_EQUALS)) 
      s = parse_assignment(b);
    else 
      s = parse_procedure(b);
  }
  else {
    // if lex_this is not an ident, it's a keyword of one of these types
    switch (lex_this.value) {
      case KEY_IF:
        s = parse_if(b);
        break;
      case KEY_SEL:
        s = parse_case(b);
        break;
      case KEY_RAISE:
        s = parse_raise(b);
        break;
      case KEY_TRY:
        s = parse_handler(b);
        break;
      case KEY_FOR:
        s = parse_for(b);
        break;
      case KEY_DO:
        s = parse_until(b);
        break;
      case KEY_WHILE:
        s = parse_while(b);
        break;
    }
  }
  return s;
}

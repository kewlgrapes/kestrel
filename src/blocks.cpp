// blocks.cpp : classes and methods for parsing blocks
//   author : members of the venture group
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <list>
using namespace std;

#include "config.h"
#include "sets.h"
#include "lexall.h"
#include "parseall.h"
#include "locale.h"

///////////////////////////////////////////////////////////
// CLASSES
///////////////////////////////////////////////////////////

BlockElement::BlockElement(Block *b) { 
  block = b;
  b->elements.push_back(this);
}
void BlockElement::put(FILE *f) {}

Block::Block(Block *b) { 
  parent = b;
  
  // initialize empty context
  int i;
  for (i = 0; i < SYMBOL_SIZE; i++) context[i] = NULL;
}

void Block::put(FILE *f) {
  fputs("Block(", f);
  
  list<BlockElement*>::iterator e;
  e = elements.begin();
  if (e != elements.end()) {
    (*e)->put(f);
    while (++e != elements.end()) {
      fputc(',', f);
      (*e)->put(f);
    }
  }

  fputc(')', f);
}

void Block::bind(Declaration *d) {
  symbol_handle s = d->ident->lex.value;
  context[s] = d;
}

Declaration *Block::lookup(Identifier *i) {
  symbol_handle s = i->lex.value;
  
  // look for declaration of s, starting in this block
  Block *cur = this;
  
  // move up to parent blocks if it is not in this block's context
  while (!(cur->context[s]) && parent){
       // printf("%d\n", (int)cur->context[s]);
	cur = parent;
   }
  //will return null if declaration is not found
  return cur->context[s];
}

///////////////////////////////////////////////////////////
// START SETS
///////////////////////////////////////////////////////////
set32_t BLOCK_START_SET = \
  TO_SET32_4(KEY_IF, KEY_SEL, KEY_TRY, KEY_RAISE) |
  TO_SET32_3(KEY_WHILE, KEY_DO, KEY_FOR);

///////////////////////////////////////////////////////////
// PROLOGUE
///////////////////////////////////////////////////////////
/*
char: type NUL .. NUL+255;
int8:   type -128 .. 127;
uint8:  type 0 .. 255;
int16:  type -32768 .. 32767;
uint16: type 0 .. 65535;
int32:  type -16#80000000 .. 16#7FFFFFFF;
uint32: type 0 .. 16#FFFFFFFF;
*/

// Kestrel Prologue definitions
void add_prologue(Block *context) {
  // range exception
  context->bind(new ExceptDeclaration(context, new Identifier("range")));
  
  // char types  
  context->bind(new ConstDeclaration(context, new Identifier("NUL"), new Expression('@'-64)));
  context->bind(new ConstDeclaration(context, new Identifier("SOH"), new Expression('A'-64)));
  context->bind(new ConstDeclaration(context, new Identifier("STX"), new Expression('B'-64)));
  context->bind(new ConstDeclaration(context, new Identifier("ETX"), new Expression('C'-64)));
  context->bind(new ConstDeclaration(context, new Identifier("EOT"), new Expression('D'-64)));
  context->bind(new ConstDeclaration(context, new Identifier("ENQ"), new Expression('E'-64)));
  context->bind(new ConstDeclaration(context, new Identifier("ACK"), new Expression('F'-64)));
  context->bind(new ConstDeclaration(context, new Identifier("BEL"), new Expression('G'-64)));
  context->bind(new ConstDeclaration(context, new Identifier("BS"),  new Expression('H'-64)));
  context->bind(new ConstDeclaration(context, new Identifier("TAB"), new Expression('I'-64)));
  context->bind(new ConstDeclaration(context, new Identifier("LF"),  new Expression('J'-64)));
  context->bind(new ConstDeclaration(context, new Identifier("VT"),  new Expression('K'-64)));
  context->bind(new ConstDeclaration(context, new Identifier("FF"),  new Expression('L'-64)));
  context->bind(new ConstDeclaration(context, new Identifier("CR"),  new Expression('M'-64)));
  context->bind(new ConstDeclaration(context, new Identifier("SO"),  new Expression('N'-64)));
  context->bind(new ConstDeclaration(context, new Identifier("SI"),  new Expression('O'-64)));
  context->bind(new ConstDeclaration(context, new Identifier("DLE"), new Expression('P'-64)));
  context->bind(new ConstDeclaration(context, new Identifier("DC1"), new Expression('Q'-64)));
  context->bind(new ConstDeclaration(context, new Identifier("DC2"), new Expression('R'-64)));
  context->bind(new ConstDeclaration(context, new Identifier("DC3"), new Expression('S'-64)));
  context->bind(new ConstDeclaration(context, new Identifier("DC4"), new Expression('T'-64)));
  context->bind(new ConstDeclaration(context, new Identifier("NAK"), new Expression('U'-64)));
  context->bind(new ConstDeclaration(context, new Identifier("SYN"), new Expression('V'-64)));
  context->bind(new ConstDeclaration(context, new Identifier("ETB"), new Expression('W'-64)));
  context->bind(new ConstDeclaration(context, new Identifier("CAN"), new Expression('X'-64)));
  context->bind(new ConstDeclaration(context, new Identifier("EM"),  new Expression('Y'-64)));
  context->bind(new ConstDeclaration(context, new Identifier("SUB"), new Expression('Z'-64)));
  context->bind(new ConstDeclaration(context, new Identifier("ESC"), new Expression('['-64)));
  context->bind(new ConstDeclaration(context, new Identifier("FS"),  new Expression('\\'-64)));
  context->bind(new ConstDeclaration(context, new Identifier("GS"),  new Expression('['-64)));
  context->bind(new ConstDeclaration(context, new Identifier("RS"),  new Expression('^'-64)));
  context->bind(new ConstDeclaration(context, new Identifier("US"),  new Expression('_'-64)));
  context->bind(new ConstDeclaration(context, new Identifier("DEL"), new Expression( 127)));

  // boolean type

  // char type
   
  // int8 type
   
  // uint8 type

  // int16 type

  // uint16 type
  
  // int32 type

  // uint32 type

  // file type

}

///////////////////////////////////////////////////////////
// FUNCTIONS
///////////////////////////////////////////////////////////

// <block> ::= { <block element> [ ";" ] }
// <block element> ::= <declaration> | <statement>
Block *parse_block(Block *context) {
  Block *b = new Block(context);
  if (!context) add_prologue(b);

  Declaration *d;
  Statement *s;
  while (in_block_start_set(lex_this) ){
    //declarations contain a colon
    if (lex_is(lex_next, PUNCT, PT_COLON)){
      d = parse_declaration(b);
      b->bind(d);
    } 
    else {
      s = parse_statement(b);
    }
    
    // semicolon at end of declaration/statment is optional
    if (lex_is( lex_this, PUNCT, PT_SEMIC))
      lex_advance();
  }
  //printf("%d ", b->elements.size());
  //lex_put(&lex_this, stdout);
  //fputc(' ', stdout);
  //b->put(stdout);
  //fputc('\n', stdout);
  return b;
}

// <kestrel_program> ::= <block> <end of file>
Block *parse_program() {
  Block *p = parse_block(NULL);
  lex_force(ENDFILE);
  return p;
}

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
  //symbol_handle s = d->ident->lex.value;
  //context[s] = d;
}

Declaration *Block::lookup(Identifier *i) {
  //symbol_handle s = i->lex.value;
  
  // look for declaration of s, starting in this block
  Block *cur = this;
  
  // move up to parent blocks if it is not in this block's context
  //while (!(cur->context[s]) && parent){
       // printf("%d\n", (int)cur->context[s]);
	//cur = parent;
   //}
  //will return null if declaration is not found
  //return cur->context[s];
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
  // char types  
  /*
  context->bind(new ConstDeclaration(new Identifier("NUL"), new Expression('@'-64),  context));
  context->bind(new ConstDeclaration(new Identifier("SOH"), new Expression('A'-64),  context));
  context->bind(new ConstDeclaration(new Identifier("STX"), new Expression('B'-64),  context));
  context->bind(new ConstDeclaration(new Identifier("ETX"), new Expression('C'-64),  context));
  context->bind(new ConstDeclaration(new Identifier("EOT"), new Expression('D'-64),  context));
  context->bind(new ConstDeclaration(new Identifier("ENQ"), new Expression('E'-64),  context));
  context->bind(new ConstDeclaration(new Identifier("ACK"), new Expression('F'-64),  context));
  context->bind(new ConstDeclaration(new Identifier("BEL"), new Expression('G'-64),  context));
  context->bind(new ConstDeclaration(new Identifier("BS"),  new Expression('H'-64),  context));
  context->bind(new ConstDeclaration(new Identifier("TAB"), new Expression('I'-64),  context));
  context->bind(new ConstDeclaration(new Identifier("LF"),  new Expression('J'-64),  context));
  context->bind(new ConstDeclaration(new Identifier("VT"),  new Expression('K'-64),  context));
  context->bind(new ConstDeclaration(new Identifier("FF"),  new Expression('L'-64),  context));
  context->bind(new ConstDeclaration(new Identifier("CR"),  new Expression('M'-64),  context));
  context->bind(new ConstDeclaration(new Identifier("SO"),  new Expression('N'-64),  context));
  context->bind(new ConstDeclaration(new Identifier("SI"),  new Expression('O'-64),  context));
  context->bind(new ConstDeclaration(new Identifier("DLE"), new Expression('P'-64),  context));
  context->bind(new ConstDeclaration(new Identifier("DC1"), new Expression('Q'-64),  context));
  context->bind(new ConstDeclaration(new Identifier("DC2"), new Expression('R'-64),  context));
  context->bind(new ConstDeclaration(new Identifier("DC3"), new Expression('S'-64),  context));
  context->bind(new ConstDeclaration(new Identifier("DC4"), new Expression('T'-64),  context));
  context->bind(new ConstDeclaration(new Identifier("NAK"), new Expression('U'-64),  context));
  context->bind(new ConstDeclaration(new Identifier("SYN"), new Expression('V'-64),  context));
  context->bind(new ConstDeclaration(new Identifier("ETB"), new Expression('W'-64),  context));
  context->bind(new ConstDeclaration(new Identifier("CAN"), new Expression('X'-64),  context));
  context->bind(new ConstDeclaration(new Identifier("EM"),  new Expression('Y'-64),  context));
  context->bind(new ConstDeclaration(new Identifier("SUB"), new Expression('Z'-64),  context));
  context->bind(new ConstDeclaration(new Identifier("ESC"), new Expression('['-64),  context));
  context->bind(new ConstDeclaration(new Identifier("FS"),  new Expression('\\'-64), context));
  context->bind(new ConstDeclaration(new Identifier("GS"),  new Expression('['-64),  context));
  context->bind(new ConstDeclaration(new Identifier("RS"),  new Expression('^'-64),  context));
  context->bind(new ConstDeclaration(new Identifier("US"),  new Expression('_'-64),  context));
  context->bind(new ConstDeclaration(new Identifier("DEL"), new Expression( 127),  context));

  // range exception
  context->bind(new ExceptDeclaration(new Identifier("range"), context));

  // boolean type
  Type *Boolean = new EnumType(new Identifier("true"), new Identifier("false"));
  context->bind(new TypeDeclaration(new Identifier("boolean"), Boolean,  context));

  // char type
  Type *Char = new SubrangeType(new Expression(0), new Expression(255));
  context->bind(new TypeDeclaration(new Identifier("char"), Char, context));
   
  // int8 type
  Type *Int8 = new SubrangeType(new Expression(-128), new Expression(127));
  context->bind(new TypeDeclaration(new Identifier("int8"), Int8, context));
   
  // uint8 type
  Type *Uint8 = new SubrangeType(new Expression(0), new Expression(255));
  context->bind(new TypeDeclaration(new Identifier("uint8"), Uint8, context));

  // int16 type
  Type *Int16 = new SubrangeType(new Expression(-32768), new Expression(32767));
  context->bind(new TypeDeclaration(new Identifier("int16"), Int16, context));

  // uint16 type
  Type *Uint16 = new SubrangeType(new Expression(0), new Expression(65535));
  context->bind(new TypeDeclaration(new Identifier("uint16"), Uint16, context));
  
  // int32 type
  SubrangeType *Int32 = new SubrangeType(new Expression(0x80000000), new Expression(0x7FFFFFFF));
  context->bind(new TypeDeclaration(new Identifier("int32"), Int32, context));

  // uint32 type
  Type *Uint32 = new SubrangeType(new Expression(0), new Expression(0xFFFFFFFF));
  context->bind(new TypeDeclaration(new Identifier("uint32"), Uint32, context));

  // file type
  Type *File = new PointerType(new RecordType(NULL));
  context->bind(new TypeDeclaration(new Identifier("file"), File, context));

  // stdin, stdout, stderr
  Declaration *Input = new VarDeclaration(new Identifier("input"), File, context);
  Declaration *Output = new VarDeclaration(new Identifier("output"), File, context);
  Declaration *Errors = new VarDeclaration(new Identifier("errors"), File, context);

  context->bind(Input);
  context->bind(Output);
  context->bind(Errors);

  // functions/procedures
*/
/*
putchar:   procedure( c:char, f:file ) external;
getchar:   function char ( f:file ) external;
eof:       function boolean ( f:file ) external;
putstring: procedure( ref s:array uint16 of char, f:file ) external;
getstring: procedure( ref s:array uint16 of char, f:file ) external;
*/
}

///////////////////////////////////////////////////////////
// FUNCTIONS
///////////////////////////////////////////////////////////

// <block> ::= { <block element> [ ";" ] }
// <block element> ::= <declaration> | <statement>
Block *parse_block(Block *context) {
  Block *b = new Block(context);
  if (!context) add_prologue(b);

  BlockElement *be;
  while (in_block_start_set(lex_this) ){
    //printf("%d ", b->elements.size());
    //lex_put(&lex_this, stdout);
    //fputc(' ', stdout);
    //b->put(stdout);
    //fputc('\n', stdout);

    //declarations contain a colon
    if (lex_is(lex_next, PUNCT, PT_COLON)){
      //create new Identifier object. For now just create a dummy.
      //be = parse_declaration(b);
    // otherwise it must be a statement
    } else {
      //be = parse_statement(b);
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

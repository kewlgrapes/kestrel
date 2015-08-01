// types.cpp : classes and methods for parsing types
//   author  : members of the venture group
using namespace std;
#include <list>

#include <stdint.h>
#include <stdio.h>

#include "config.h"
#include "sets.h"
#include "lexall.h"
#include "parseall.h"
#include "sets.h"

#define TESTING

/////////////////////////////////////////////////////////////
//   SETS
/////////////////////////////////////////////////////////////

//# start set:   <identifier> "@" "array" "enum" "record" "-" "~" <number> "null"
set32_t KEY_START_SET  = TO_SET32_4( KEY_ARRAY, KEY_ENUM, KEY_REC, KEY_NULL );
set32_t PT_START_SET   = TO_SET32_3( PT_ATSIGN, PT_MINUS, PT_TILDE );

//set32_t TYPE_START_SET = TO_SET32_4(KEY_IDENT,PT_ATSIGN,KEY_ARRAY,KEY_ENUM) | TO_SET32_4(KEY_RECORD,PT_MINUS,PT_TILDE,KEY_NULL);

void Type::put( FILE *F )
{
 /* TODO: Finish this later. */
}

//Examples of type types
//TYPE_REF, TYPE_ENUM,
//TYPE_SUB, TYPE_POINT,
//TYPE_ARRAY, TYPE_REC

void Type::defineType( type_of t )
{
  this_type = t;
}

RefType::RefType( Reference *r ) 
{
  ref = r;
  defineType( TYPE_REF );
}
void RefType::put(FILE *f) {fputs("RefType()",f);}

EnumType::EnumType(Identifier *t, Identifier *f)
{
  ident1 = t;
  ident2 = f;
}

EnumType::EnumType( list<Identifier*> list )
{
 // members.push_back( ident );
  defineType( TYPE_ENUM );
  SpliceIdentList( list );
}
void EnumType::put(FILE *f) {fputs("EnumType()",f);}

void EnumType::SpliceIdentList( list<Identifier*> identList ) 
{
  list<Identifier*>::iterator it;
  it = members.begin();
  members.splice( it, identList );
#ifdef TESTING
  fputs("splicing identifier list...\n",stdout);
#endif
}

SubrangeType::SubrangeType( Expression *f, Expression *t )
{
  from = f; 
  to = t;
  defineType( TYPE_SUB );
}
void SubrangeType::put(FILE *f) {fputs("SubrangeType()",f);}

PointerType::PointerType( Type *t )
{
  type = t;
  defineType( TYPE_POINT );
}
void PointerType::put(FILE *f) {fputs("PointerType()",f);}

ArrayType::ArrayType( Type *t, Type *et )
{
  type = t;
  elem_type = et;
  defineType( TYPE_ARRAY );
}
void ArrayType::put(FILE *f) {fputs("ArrayType()",f);}

RecordType::RecordType(Block *b)
{
  block = b;
  defineType( TYPE_REC );
}
void RecordType::put(FILE *f) {fputs("RecordType()",f);}

/////////////////////////////////////////////////////////////
// FUNCTIONS
/////////////////////////////////////////////////////////////

/*  <type> ::= <reference>
          |  <enumeration>
          |  <expression> ".." <expression>
          |  "@" <type>
          |  "array" <type> "of" <type>
          |  <record>                           */
 
Type *parse_type( Block *b ) 
{  
  //Reference
  if ( lex_is_type( lex_this, IDENT ) ) 
  {
    Reference *r = parse_reference(b);
    return new RefType(r); 
  }
  //Enumeration
  else if ( lex_is( lex_this, KEYWORD, KEY_ENUM ) ) 
  {
    return parse_enum(b);
  }
  
  //SubrangeType::= <expressing> ".." <expression>
  else if ( in_expr_start_set( lex_this ) ) 
  {
    return parse_subrange( b );
  }

  //PointerType::= "@" <type>
  else if ( lex_is( lex_this, PUNCT, PT_ATSIGN ) ) 
  {
    lex_advance();
    return new PointerType( parse_type(b) );
  }

  //ArrayType::= "array" <type> "of" <type>
  else if ( lex_is( lex_this,KEYWORD, KEY_ARRAY ) ) 
  {
    return parse_array(b);
  }
  //Record
  else if ( lex_is( lex_this, KEYWORD, KEY_REC ) ) 
  {
    return new RecordType( parse_record(b) ); 
  }
} //end test_parse



/*  <enumeration> ::= "enum" "(" <identifier> <enumeration-a> ")"
                |  "enum" "(" <identifier> ")"
                |  "enum" "[" <identifier> <enumeration-b> "]"
                |  "enum" "[" <identifier> "]"
                |  "enum" "{" <identifier> <enumeration-c> 
                |  "enum" "{" <identifier> "}"                 */
EnumType *parse_enum( Block *b ) 
{
  int identCount = 0;
  lex_advance();
  list<Identifier*> tempList;
  if (   ( lex_is( lex_this, PUNCT, PT_LPAREN ) ) 
      || ( lex_is( lex_this, PUNCT, PT_LBRAKT ) ) 
      || ( lex_is( lex_this, PUNCT, PT_LBRACE ) ) ) 
  {
    punct_type closer = punct_closing_symbol( ( punct_type )lex_this.value );
#ifdef TESTING
      if (closer == 3)
        fputs("parse_enum: <enumeration-a>\n",stdout);
      else if (closer == 5)
        fputs("parse_enum: <enumeration-b>\n",stdout);
      else
        fputs("parse_enum: <enumeration-c>\n",stdout);
#endif
      while (( punct_type )lex_this.value != closer )
      {
        lex_advance();
        if (!lex_is(lex_this,PUNCT,PT_COMMA))
        {
          identCount += 1;
          Identifier *i = parse_identifier(b);
          tempList.push_back(i);
        }
      }  
        
        if ( ( punct_type )lex_this.value == closer) 
        {
#ifdef TESTING
          fprintf(stdout,"parse_enum: creating new EnumType instance with %d identifiers\n",identCount);
#endif
          return new EnumType(tempList);
        }
    }
} //end parse_enum



//SubrangeType::= <expressing> ".." <expression>
SubrangeType *parse_subrange( Block *b ) 
{

  Expression *e_from;
  Expression *e_to;
    
  e_from = parse_expression(b);
 
 if (lex_is(lex_this,PUNCT,PT_ELIPS))
 {
#ifdef TESTING
   fputs("found '..'\n",stdout);
#endif
   lex_advance();
      
   if ( in_expr_start_set( lex_this ) ) 
   {
     e_to = parse_expression(b);
   }

   return new SubrangeType( e_from, e_to );
 }
} //end parse_subrange



//ArrayType::= "array" <type> "of" <type>
ArrayType *parse_array(Block *b)
{
    Type *t_a;
    Type *t_b;
    lex_advance();

      if (    lex_in_set(   lex_this, KEYWORD, KEY_START_SET ) 
          ||  lex_in_set(   lex_this, PUNCT,   PT_START_SET  ) 
          ||  lex_is_type(  lex_this, IDENT ) )
      {
        
        t_a = parse_type(b);

#ifdef TESTING
        fputs("\nparse_array:\n     first type is ",stdout);
        t_a->put(stdout);
        fputs("\n",stdout);
#endif

      }
      if (lex_is(lex_this,KEYWORD,KEY_OF)) 
      {
        lex_advance();
        if (    lex_in_set(   lex_this, KEYWORD, KEY_START_SET ) 
            ||  lex_in_set(   lex_this, PUNCT,   PT_START_SET  )
            ||  lex_is_type(  lex_this, IDENT ))
        {
          t_b = parse_type(b);

#ifdef TESTING
          fputs("     second type is ",stdout);
          t_b->put(stdout);
          fputs("\n",stdout);
#endif
        }
        return new ArrayType( t_a, t_b );
      }
} // end parse_array



//   <record> ::= "record" <block-a> "end"  |  "record" "end"
Block *parse_record( Block *b ) 
{
  if ( lex_is( lex_this, KEYWORD, KEY_REC ) ) 
  {
    lex_advance();
    if ( lex_is( lex_next, KEYWORD, KEY_END ) ) 
    {
      Block *tempBlock = parse_block(b);
      lex_advance();
      return tempBlock;
    }
  }
} //end parse_record



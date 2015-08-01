// types.h : classes and methods for parsing types
//   author : members of the Venture group

// this file declares...
class Type;
class RefType;
class EnumType;
class SubrangeType;
class PointerType;
class ArrayType;

#define in_type_start_set(lex) ( \
  in_expr_start_set(lex) ||      \
  lex_in_set(lex, KEY_START_SET))

typedef enum
{
  TYPE_REF, TYPE_ENUM,
  TYPE_SUB, TYPE_POINT,
  TYPE_ARRAY, TYPE_REC
} type_of;

///////////////////////////////////////////////////////////
// TYPES
///////////////////////////////////////////////////////////
Type          *parse_type(     Block *b  );
EnumType      *parse_enum(     Block *b  );
Block         *parse_record(   Block *b  );
ArrayType     *parse_array(    Block *b  ); 
SubrangeType  *parse_subrange( Block *b  );
  

class Type 
{
  public:
    type_of this_type;
    virtual void put( FILE *f );
    void defineType( type_of t );
};

class RefType : public Type 
{
  public:
    Reference *ref;
    RefType( Reference *r );
    void put(FILE *f);
};

class EnumType : public Type 
{
  public:
    Identifier *ident1;
    Identifier *ident2;
    list<Identifier*> members;
    EnumType( list<Identifier*> list );
    EnumType(Identifier *t, Identifier *f);
    void SpliceIdentList( list<Identifier*> list );
    void put(FILE *f);
};

class SubrangeType : public Type 
{
  public:
    Expression *from;
    Expression *to;
    SubrangeType( Expression *f, Expression *t );
    void put(FILE *f);
};

class PointerType : public Type 
{
  public:
    Type *type;
    PointerType( Type *t );
    void put(FILE *f);
};

class ArrayType : public Type 
{
  public:
    Type *type;
    Type *elem_type;
    ArrayType( Type *t, Type *et );
    void put(FILE *f);
};

class RecordType : public Type 
{
  public:
    Block *block;
    RecordType( Block *b );
    void put(FILE *f);
};

extern Type *CharType;
extern Type *IntegerType;
extern Type *BooleanType;

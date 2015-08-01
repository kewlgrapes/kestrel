// declarations.h : classes and methods for parsing declarations
//   author : members of the Venture group

// this file declares...
class Parameter;
class Declaration;
class ConstDeclaration;
class TypeDeclaration;
class ExceptDeclaration;
class VarDeclaration;
class ProcDeclaration;
class FuncDeclaration;

Declaration *parse_declaration(Block *context);

///////////////////////////////////////////////////////////
// DECLARATIONS
///////////////////////////////////////////////////////////

class Parameter {
  public:
    Identifier *ident;
    Type *type;
    int is_const;
    Parameter(Identifier *i, Type *t, int ic);
    void put(FILE *f);
};

class Declaration : public BlockElement {
  public:
    int is_private;
    Identifier *ident;
    Declaration(Block *b, Identifier *i, int ip=0);
    void virtual put(FILE *f);
};

class ConstDeclaration : public Declaration {
  public:
    Expression *expr;
    ConstDeclaration(Block *b, Identifier *i, Expression *e, int ip=0);
    void put(FILE *f);
};

class TypeDeclaration : public Declaration {
  public: 
    Type *type;
    TypeDeclaration(Block *b, Identifier *i, Type *t, int ip=0);
    void put(FILE *f);
};

class ExceptDeclaration : public Declaration {
  public:
    ExceptDeclaration(Block *b, Identifier *i, int ip=0);  
    void put(FILE *f);
};

class VarDeclaration : public Declaration {
  public:
    Type *type;
    VarDeclaration(Block *b, Identifier *i, Type *t, int ip=0);
    void put(FILE *f);
};

class ProcDeclaration : public Declaration {
  public:
    list<Parameter*> parameters;
    Block *body;
    ProcDeclaration(Block *b, Identifier *i, int ip=0);
    void put(FILE *f);
};

class FuncDeclaration : public ProcDeclaration {
  public:
    Type *type;
    FuncDeclaration(Block *b, Identifier *i, Type *t, int ip=0);
    void put(FILE *f);
};

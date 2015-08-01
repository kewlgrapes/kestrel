// statements.h : classes and methods for parsing statements
//   author : members of the Venture group

// this file declares...
class Statement;
class If;
class CaseLabel;
class Case;
class While;
class Until;
class For;
class Handler;
class Raise;
class Procedure;
class Assignment;

///////////////////////////////////////////////////////////
// CLASSES
///////////////////////////////////////////////////////////

class Statement: public BlockElement {
  public:
    Statement(Block *b);
    virtual void put(FILE *f);
};

class If: public Statement {
  public:
    Expression *expr;
    Block *if_block;
    Block *else_block;
    If(Block *b, Expression *e, Block *ib, Block *eb);
    void put(FILE *f);
};

class CaseLabel {
  public:
    Expression *expr1;
    Expression *expr2;
    CaseLabel(Expression *expr1, Expression *expr2);
    void put(FILE *f);
};

class CaseBlock {
  public:
    Block *block;
    list<CaseLabel*> caselabels;
    CaseBlock();
    void put(FILE *f);
};

class Case: public Statement {
  public: 
    Expression *expr;
    list<CaseBlock*> caseblocks;
    Block *else_block;
    Case(Block *b, Expression *e);
    void put(FILE *f);
};

class Loop: public Statement {
  public:
    Loop(Block *b);
    virtual void put(FILE *f);
};

class WhileLoop: public Loop {
  public:
    Expression *expr;
    Block *loop_block;
    WhileLoop(Block *b, Expression *e, Block *l);
    void put(FILE *f);
};

class UntilLoop: public Loop {
  public:
    Expression *expr;
    Block *loop_block;
    UntilLoop(Block *b, Expression *e, Block *l);
    void put(FILE *f);
};

class ForLoop: public Loop {
  public:
    Identifier *ident;
    Type *type;
    Block *loop_block;
    ForLoop(Block *b, Identifier *i, Type *t, Block *l);
    void put(FILE *f);
};

class HandlerBlock: public Block {
  public:
    list<Reference *> references;
    HandlerBlock(Block *b);
    void put(FILE *f);
};

class Handler: public Statement {
  public:
    Block *handler_block;
    list<HandlerBlock*> handlerblocks;
    Block *else_block;
    Handler(Block *b, Block *hb);
    void put(FILE *f);
};

class Raise: public Statement {
  public:
    Reference *ref;
    Raise(Block *b, Reference *r);
    void put(FILE *f);
};

class Procedure: public Statement {
  public:
    Reference *ref;
    Procedure(Block *b, Reference *r);
    void put(FILE *f);
};

class Assignment: public Statement {
  public:
    Reference *ref;
    Expression *expr;
    Assignment(Block *b, Reference *r, Expression *e);
    void put(FILE *f);
};

/**
FUNCTIONS PRESENT IN STATEMENTS.CPP
*/
Statement *parse_statement(Block *b);
CaseLabel *parse_label(Block *b);
Raise *parse_raise(Block *b);
Handler *parse_handler(Block *b);
ForLoop *parse_for(Block *b);
UntilLoop *parse_until(Block *b);
WhileLoop *parse_while(Block *b);
Case *parse_case(Block *b);
If *parse_if(Block *b);


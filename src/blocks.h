// blocks.h : classes and methods for parsing blocks
//   author : members of the Venture group

// this file declares...
class Block;
class BlockElement;

Block *parse_block(Block *block);
Block *parse_program();

///////////////////////////////////////////////////////////
// SETS
///////////////////////////////////////////////////////////

extern set32_t BLOCK_START_SET;

#define in_block_start_set(lex) (lex_is_type((lex), IDENT) || \
                                 lex_in_set((lex), KEYWORD, BLOCK_START_SET))

///////////////////////////////////////////////////////////
// CLASSES
///////////////////////////////////////////////////////////

// elements of a block
class BlockElement {
  public:
    Block *block;      // the parent block of this block element
    
    // create a new block element with parent block b
    BlockElement(Block *b);

    // Statement and Declaration will handle this
    virtual void put(FILE *f);
};

class Declaration;
class Identifier;

// blocks...
class Block {
  Declaration *context[SYMBOL_SIZE];
  public:
    Block *parent; // parent of this block
    list<BlockElement*> elements; // elements in this block

    // create a new Block with parent block b
    Block(Block *b);
    void put(FILE *f);
    void bind(Declaration *d);
    Declaration *lookup(Identifier *i);
};

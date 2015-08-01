// sets.h -- operators on sets of up to 32 elements
//   by: Douglas W. Jones

// users of this must first include <stdint.h>

// the set data type
typedef uint32_t set32_t;

// set32_t TO_SET32( int i )
#define TO_SET32(i) (((set32_t)1)<<(i))
// constructs a new set from one integer

// bool IN_SET32( int i, set32_t s )
#define IN_SET32(i,s) (TO_SET32(i) & s)
// test for set membership

// set constructors for larger constant-valued sets
#define TO_SET32_2(i,j)     (TO_SET32(i) | TO_SET32(j))
#define TO_SET32_3(i,j,k)   (TO_SET32(i) | TO_SET32_2(j,k))
#define TO_SET32_4(i,j,k,l) (TO_SET32_2(i,j) | TO_SET32_2(k,l))

// on sets, the & operator means set intersection
//          the | operator means set union

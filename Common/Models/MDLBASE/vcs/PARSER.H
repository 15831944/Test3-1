
#ifndef _OPCODE
#define _OPCODE
struct OPCODE
{
 int mnem;
 union
 {
  unsigned int entry;   /* for the GET mnemonic - array oreinted */
  void *index;        /* for the GET mnemonic - pionter oreinted */
  double op_value;
 }x;
};
#endif
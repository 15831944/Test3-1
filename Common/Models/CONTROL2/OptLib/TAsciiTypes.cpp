#include <stdafx.h>
//---------------------------------------------------------------------------
#include "TAsciiTypes.h"

//-----------------------------------------------------------------------------
// atDigits          =0x0001, // 0123456789
// atWhiteSpace      =0x0002, // space and tab
// atLetters         =0x0004, // 'a'-'z','A'-'Z'
// atLowerCase       =0x0008, // 'a'-'z'
// atUpperCase       =0x0010, // 'A'-'Z'
// atControlChars    =0x0020, // 0x0-0x1f(0-31)
// atNonAscii        =0x0040, // 0x7f-0xff(127-255)
// atPunctuation     =0x0080, // !"#$%&'()*+,-./:;<=>?@[\]^_`{|}~
// atPunctNonFileName=0x0100, // Illegal file name characters  \/:*?"<>|
// atPunctFileName   =0x0200, // Legal file name characters    // !#$%&'()+,-.;=@[]^_`{}~
// atPunctNumber     =0x0400, // +-.
// atBrackets        =0x0800, // []{}()
// atPunctText       =0x1000, // :?"!'(),.;`
// atFileNameChars   =0x2000, // digits,letters,atPunctFileName,space
// atCodeIdentifier  =0x4000};// atLetters, atDigits, '_'
//-----------------------------------------------------------------------------

const unsigned int CHAR_TO_ASCII_TYPES[127] =
{
/* '\x00'   */ atControlChars,
/* '\x01'   */ atControlChars,
/* '\x02'   */ atControlChars,
/* '\x03'   */ atControlChars,
/* '\x04'   */ atControlChars,
/* '\x05'   */ atControlChars,
/* '\x06'   */ atControlChars,
/* '\x07'   */ atControlChars,
/* '\x08'   */ atControlChars,
/* '\x09'   */ atControlChars | atWhiteSpace,
/* '\x0a'   */ atControlChars,
/* '\x0b'   */ atControlChars,
/* '\x0c'   */ atControlChars,
/* '\x0d'   */ atControlChars,
/* '\x0e'   */ atControlChars,
/* '\x0f'   */ atControlChars,
/* '\x10'   */ atControlChars,
/* '\x11'   */ atControlChars,
/* '\x12'   */ atControlChars,
/* '\x13'   */ atControlChars,
/* '\x14'   */ atControlChars,
/* '\x15'   */ atControlChars,
/* '\x16'   */ atControlChars,
/* '\x17'   */ atControlChars,
/* '\x18'   */ atControlChars,
/* '\x19'   */ atControlChars,
/* '\x1a'   */ atControlChars,
/* '\x1b'   */ atControlChars,
/* '\x1c'   */ atControlChars,
/* '\x1d'   */ atControlChars,
/* '\x1e'   */ atControlChars,
/* '\x1f'   */ atControlChars,
/* ' ' 0x20 */ atWhiteSpace | atFileNameChars,
/*  !  0x21 */ atPunctuation | atPunctFileName | atPunctText | atFileNameChars,
/*  "  0x22 */ atPunctuation | atPunctNonFileName | atPunctText,
/*  #  0x23 */ atPunctuation | atPunctFileName | atFileNameChars,
/*  $  0x24 */ atPunctuation | atPunctFileName | atFileNameChars,
/*  %  0x25 */ atPunctuation | atPunctFileName | atFileNameChars,
/*  &  0x26 */ atPunctuation | atPunctFileName | atFileNameChars,
/*  '  0x27 */ atPunctuation | atPunctNonFileName | atPunctText,
/*  (  0x28 */ atPunctuation | atPunctFileName | atBrackets | atPunctText | atFileNameChars,
/*  )  0x29 */ atPunctuation | atPunctFileName | atBrackets | atPunctText | atFileNameChars,
/*  *  0x2a */ atPunctuation | atPunctNonFileName,
/*  +  0x2b */ atPunctuation | atPunctFileName | atPunctNumber | atFileNameChars,
/*  ,  0x2c */ atPunctuation | atPunctFileName | atPunctText | atFileNameChars,
/*  -  0x2d */ atPunctuation | atPunctFileName | atPunctNumber | atFileNameChars,
/*  .  0x2e */ atPunctuation | atPunctFileName | atPunctNumber | atPunctText | atFileNameChars,
/*  /  0x2f */ atPunctuation | atPunctNonFileName,
/*  0  0x30 */ atDigits | atFileNameChars | atCodeIdentifier,
/*  1  0x31 */ atDigits | atFileNameChars | atCodeIdentifier,
/*  2  0x32 */ atDigits | atFileNameChars | atCodeIdentifier,
/*  3  0x33 */ atDigits | atFileNameChars | atCodeIdentifier,
/*  4  0x34 */ atDigits | atFileNameChars | atCodeIdentifier,
/*  5  0x35 */ atDigits | atFileNameChars | atCodeIdentifier,
/*  6  0x36 */ atDigits | atFileNameChars | atCodeIdentifier,
/*  7  0x37 */ atDigits | atFileNameChars | atCodeIdentifier,
/*  8  0x38 */ atDigits | atFileNameChars | atCodeIdentifier,
/*  9  0x39 */ atDigits | atFileNameChars | atCodeIdentifier,
/*  :  0x3a */ atPunctuation | atPunctNonFileName | atPunctText,
/*  ;  0x3b */ atPunctuation | atPunctFileName | atPunctText | atFileNameChars,
/*  <  0x3c */ atPunctuation | atPunctNonFileName,
/*  =  0x3d */ atPunctuation | atPunctFileName | atFileNameChars,
/*  >  0x3e */ atPunctuation | atPunctNonFileName,
/*  ?  0x3f */ atPunctuation | atPunctNonFileName | atPunctText,
/*  @  0x40 */ atPunctuation | atPunctFileName | atFileNameChars,
/*  A  0x41 */ atLetters | atUpperCase | atFileNameChars | atCodeIdentifier,
/*  B  0x42 */ atLetters | atUpperCase | atFileNameChars | atCodeIdentifier,
/*  C  0x43 */ atLetters | atUpperCase | atFileNameChars | atCodeIdentifier,
/*  D  0x44 */ atLetters | atUpperCase | atFileNameChars | atCodeIdentifier,
/*  E  0x45 */ atLetters | atUpperCase | atFileNameChars | atCodeIdentifier,
/*  F  0x46 */ atLetters | atUpperCase | atFileNameChars | atCodeIdentifier,
/*  G  0x47 */ atLetters | atUpperCase | atFileNameChars | atCodeIdentifier,
/*  H  0x48 */ atLetters | atUpperCase | atFileNameChars | atCodeIdentifier,
/*  I  0x49 */ atLetters | atUpperCase | atFileNameChars | atCodeIdentifier,
/*  J  0x4a */ atLetters | atUpperCase | atFileNameChars | atCodeIdentifier,
/*  K  0x4b */ atLetters | atUpperCase | atFileNameChars | atCodeIdentifier,
/*  L  0x4c */ atLetters | atUpperCase | atFileNameChars | atCodeIdentifier,
/*  M  0x4d */ atLetters | atUpperCase | atFileNameChars | atCodeIdentifier,
/*  N  0x4e */ atLetters | atUpperCase | atFileNameChars | atCodeIdentifier,
/*  O  0x4f */ atLetters | atUpperCase | atFileNameChars | atCodeIdentifier,
/*  P  0x50 */ atLetters | atUpperCase | atFileNameChars | atCodeIdentifier,
/*  Q  0x51 */ atLetters | atUpperCase | atFileNameChars | atCodeIdentifier,
/*  R  0x52 */ atLetters | atUpperCase | atFileNameChars | atCodeIdentifier,
/*  S  0x53 */ atLetters | atUpperCase | atFileNameChars | atCodeIdentifier,
/*  T  0x54 */ atLetters | atUpperCase | atFileNameChars | atCodeIdentifier,
/*  U  0x55 */ atLetters | atUpperCase | atFileNameChars | atCodeIdentifier,
/*  V  0x56 */ atLetters | atUpperCase | atFileNameChars | atCodeIdentifier,
/*  W  0x57 */ atLetters | atUpperCase | atFileNameChars | atCodeIdentifier,
/*  X  0x58 */ atLetters | atUpperCase | atFileNameChars | atCodeIdentifier,
/*  Y  0x59 */ atLetters | atUpperCase | atFileNameChars | atCodeIdentifier,
/*  Z  0x5a */ atLetters | atUpperCase | atFileNameChars | atCodeIdentifier,
/*  [  0x5b */ atPunctuation | atPunctFileName | atBrackets | atFileNameChars,
/*  \  0x5c */ atPunctuation | atPunctFileName | atFileNameChars,
/*  ]  0x5d */ atPunctuation | atPunctFileName | atBrackets | atFileNameChars,
/*  ^  0x5e */ atPunctuation | atPunctFileName | atFileNameChars,
/*  _  0x5f */ atPunctuation | atPunctFileName | atFileNameChars | atCodeIdentifier,
/*  `  0x60 */ atPunctuation | atPunctFileName | atPunctText | atFileNameChars,
/*  a  0x61 */ atLetters | atLowerCase | atFileNameChars | atCodeIdentifier,
/*  b  0x62 */ atLetters | atLowerCase | atFileNameChars | atCodeIdentifier,
/*  c  0x63 */ atLetters | atLowerCase | atFileNameChars | atCodeIdentifier,
/*  d  0x64 */ atLetters | atLowerCase | atFileNameChars | atCodeIdentifier,
/*  e  0x65 */ atLetters | atLowerCase | atFileNameChars | atCodeIdentifier,
/*  f  0x66 */ atLetters | atLowerCase | atFileNameChars | atCodeIdentifier,
/*  g  0x67 */ atLetters | atLowerCase | atFileNameChars | atCodeIdentifier,
/*  h  0x68 */ atLetters | atLowerCase | atFileNameChars | atCodeIdentifier,
/*  i  0x69 */ atLetters | atLowerCase | atFileNameChars | atCodeIdentifier,
/*  j  0x6a */ atLetters | atLowerCase | atFileNameChars | atCodeIdentifier,
/*  k  0x6b */ atLetters | atLowerCase | atFileNameChars | atCodeIdentifier,
/*  l  0x6c */ atLetters | atLowerCase | atFileNameChars | atCodeIdentifier,
/*  m  0x6d */ atLetters | atLowerCase | atFileNameChars | atCodeIdentifier,
/*  n  0x6e */ atLetters | atLowerCase | atFileNameChars | atCodeIdentifier,
/*  o  0x6f */ atLetters | atLowerCase | atFileNameChars | atCodeIdentifier,
/*  p  0x70 */ atLetters | atLowerCase | atFileNameChars | atCodeIdentifier,
/*  q  0x71 */ atLetters | atLowerCase | atFileNameChars | atCodeIdentifier,
/*  r  0x72 */ atLetters | atLowerCase | atFileNameChars | atCodeIdentifier,
/*  s  0x73 */ atLetters | atLowerCase | atFileNameChars | atCodeIdentifier,
/*  t  0x74 */ atLetters | atLowerCase | atFileNameChars | atCodeIdentifier,
/*  u  0x75 */ atLetters | atLowerCase | atFileNameChars | atCodeIdentifier,
/*  v  0x76 */ atLetters | atLowerCase | atFileNameChars | atCodeIdentifier,
/*  w  0x77 */ atLetters | atLowerCase | atFileNameChars | atCodeIdentifier,
/*  x  0x78 */ atLetters | atLowerCase | atFileNameChars | atCodeIdentifier,
/*  y  0x79 */ atLetters | atLowerCase | atFileNameChars | atCodeIdentifier,
/*  z  0x7a */ atLetters | atLowerCase | atFileNameChars | atCodeIdentifier,
/*  {  0x7b */ atPunctuation | atPunctFileName | atBrackets | atFileNameChars,
/*  |  0x7c */ atPunctuation | atPunctNonFileName,
/*  }  0x7d */ atPunctuation | atPunctFileName | atBrackets | atFileNameChars,
/*  ~  0x7e */ atPunctuation | atPunctFileName | atFileNameChars
};






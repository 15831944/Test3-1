//---------------------------------------------------------------------------
#ifndef T_CHARACTER_TYPES_H
#define T_CHARACTER_TYPES_H
//---------------------------------------------------------------------------
enum TAsciiTypes {atDigits          =0x0001, // 0123456789
                  atWhiteSpace      =0x0002, // space and tab
                  atLetters         =0x0004, // 'a'-'z','A'-'Z'
                  atLowerCase       =0x0008, // 'a'-'z'
                  atUpperCase       =0x0010, // 'A'-'Z'
                  atControlChars    =0x0020, // 0x0-0x1f(0-31)
                  atNonAscii        =0x0040, // 0x7f-0xff(127-255)
                  atPunctuation     =0x0080, // !"#$%&'()*+,-./:;<=>?@[\]^_`{|}~
                  atPunctNonFileName=0x0100, // Illegal file name characters  \/:*?"<>|
                  atPunctFileName   =0x0200, // Legal file name characters    // !#$%&'()+,-.;=@[]^_`{}~
                  atPunctNumber     =0x0400, // +-.
                  atBrackets        =0x0800, // []{}()
                  atPunctText       =0x1000, // :?"!'(),.;`
                  atFileNameChars   =0x2000, // digits,letters,atPunctFileName,space
                  atCodeIdentifier  =0x4000};// atLetters, atDigits, '_'

const int ASCII_TYPES_MASK = 0x7fff;

extern const unsigned int CHAR_TO_ASCII_TYPES[127];

#endif T_CHARACTER_TYPES_H

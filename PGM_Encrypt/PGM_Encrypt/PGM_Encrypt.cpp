// PGM_Encrypt.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include <fstream>
#include <iostream>

int main(int argc, char* argv[])
{
  char* textBuffer;
  char* encBuffer;

  char fn1[1024];
  char fn2[1024];

  if (argc != 2)
  {
    std::cout << ("Usage: PGM_Encrypt original_file");
    return 0;
  }

  strcpy(fn1, argv[1]);
  strcpy(fn2, argv[1]);
  strcat(fn2, ".x");

  std::cout << fn1 << std::endl;
  std::cout << fn2 << std::endl;

  std::fstream fin;
  fin.open(fn1, std::ios::in);
  if (fin.is_open())
  {
    fin.seekg(0, std::ios_base::end);
    int length = fin.tellg();
    fin.seekg(0, std::ios_base::beg);

    textBuffer = new char[length];
    fin.read(textBuffer, length);
    fin.close();

    int count = fin.gcount();
    textBuffer[count] = 71; // 71 is like our password for this decryption...

    encBuffer = new char[count+1];

    for (int i=count-1; i>=0; i--) // decrypt the buffer.
    {
      encBuffer[i] = textBuffer[i] ^ textBuffer[i+1];
    }

    encBuffer[count] = 0;

    std::fstream fout;
    fout.open(fn2, std::ios::out|std::ios::binary);
    if (fout.is_open())
    {
      fout.write(encBuffer, count);
      fout.close();
    }

  }
  return 0;
}

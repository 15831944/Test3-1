#include "stdafx.h"
#include "Scribble.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//===========================================================================
// Set the scribble version data here
// This will be the last items added to the DLL.
// This data will be retrieved to determine which ScibbleRecord
// data structure to read back.

unsigned short CScribble::ver_major = 2;
unsigned short CScribble::ver_minor = 0;
char CScribble::ver_minorchar = 'a';
const int ver_infosize = sizeof(CScribble::ver_major) + sizeof(CScribble::ver_minor) + sizeof(CScribble::ver_minorchar);

//===========================================================================

CScribble::CScribble()
{
}

//---------------------------------------------------------------------------

CScribble::~CScribble()
{
}

//---------------------------------------------------------------------------

const char* CScribble::GetScribbleVer( CString &VerStr )
{
  VerStr.Format("%d.%d%c", ver_major, ver_minor, ver_minorchar);
  return (LPCSTR)VerStr;
}

//---------------------------------------------------------------------------

bool CScribble::AddScribble(CString &fname, bool Replace)
{
  FILE* f;
  fpos_t pos;
  struct ScribbleRecordV1_0 write_buff;
  int i;
  LPTSTR lpsz = new TCHAR[fname.GetLength()+1];
  _tcscpy(lpsz, fname);

  // Open in append and binary mode after EOF
  f = fopen(fname,"ab");

  if ( f != NULL )
  {
    write_buff.major = ver_major;
    write_buff.minor = ver_minor;
    write_buff.minorchar = ver_minorchar;
    write_buff.spare = 0L;

    // If in replacement mode we adjust the file position to
    // the beginning of the existing record. The caller must use
    // ReadScribble to verify that a scribble record actually exists.
    if ( Replace )
    {
      fseek( f, 0 , SEEK_END );
      fgetpos( f, &pos );
      pos = pos - sizeof(struct ScribbleRecordV1_0) - ver_infosize;
      fsetpos( f , &pos );
    }

    // Add the scribble to the license

    // Split the path into components
    char drive[_MAX_DRIVE];
    char dir[_MAX_DIR];
    char fname_nopath[_MAX_FNAME];
    char ext[_MAX_EXT];
    _splitpath( fname, drive, dir, fname_nopath, ext );

    strncpy((char*)&write_buff.filename[0],"123456789012345",15);
    strncpy((char*)&write_buff.filename[0],fname_nopath,15);

    // Shift each byte in filename by 3 bits
    for ( i=0;i<15;i++)
      write_buff.filename[i] = write_buff.filename[i] << 3;

    // Write scribble record buffer
    fwrite( (const void*)&write_buff, sizeof(struct ScribbleRecordV1_0), 1, f );

    // Write the scribble version data as the last 2+2+1 bytes
    fwrite( (const void*)&ver_major, sizeof(unsigned short), 1, f );
    fwrite( (const void*)&ver_minor, sizeof(unsigned short), 1, f );
    fwrite( (const void*)&ver_minorchar, sizeof(char), 1, f );

    // Close the file and return
    fclose(f);


    // Check that we actually wrote the file OK
    if ( ReadScribble(fname) )
      return true;
    else
      return false;
  }
  else
  {
    //MessageBox( NULL , "Could not open file", MB_ICONERROR );
  }
  return false;
}

//---------------------------------------------------------------------------

bool CScribble::ReadScribble(CString &fname)
{
  FILE* f;
  fpos_t pos;
  struct ScribbleRecordV1_0 write_buff;
  struct ScribbleRecordV1_0 read_buff;
  int i;
  unsigned short lver_major, lver_minor1, lver_minor2;
  LPTSTR lpsz = new TCHAR[fname.GetLength()+1];
  _tcscpy(lpsz, fname);


  // Open in append and binary mode
  f = fopen(fname,"rb");

  if ( f != NULL )
  {

    // Read the scribble to the license

    // Split the path into components
    char drive[_MAX_DRIVE];
    char dir[_MAX_DIR];
    char fname_nopath[_MAX_FNAME];
    char ext[_MAX_EXT];
    _splitpath( fname, drive, dir, fname_nopath, ext );

    strncpy((char*)&write_buff.filename[0],"123456789012345",15);
    strncpy((char*)&write_buff.filename[0],fname_nopath,15);

    // Shift each byte by 3 bits
    for ( i=0;i<15;i++)
    {
      write_buff.filename[i] = write_buff.filename[i] << 3;
      read_buff.filename[i] = 0; //Initialise read_buff to rubbish
    }

    // Read Scribble Version Data
    fseek( f, 0 , SEEK_END );
    fgetpos( f, &pos );
    pos = pos - 3*sizeof(unsigned short);
    fsetpos( f , &pos );
    fread( &lver_major, sizeof(unsigned short), 1, f );
    fread( &lver_minor1, sizeof(unsigned short), 1, f );
    fread( &lver_minor2, sizeof(unsigned short), 1, f );

    // Based on the version data above we would then read the ScribbleRecord
    // for the appropriate version
 

    // Read Scribble record to the buffer
    fseek( f, 0 , SEEK_END );
    fgetpos( f, &pos );
    pos = pos - sizeof(struct ScribbleRecordV1_0) - ver_infosize;
    fsetpos( f , &pos );
    fread( &read_buff, sizeof(struct ScribbleRecordV1_0), 1, f );

    // Close the file and return
    fclose(f);

    // Compare the read buffer with what we expected
    // Compare on a byte basis as we want extra info past
    // the null terminated string
    for ( i=0;i<15;i++)
      if ( write_buff.filename[i] != read_buff.filename[i] ) return false;

    // Found matching license
    return true;

  }
  else
  {
    //MessageBox( NULL , "Could not open file", MB_ICONERROR );
  }
  return false;
}

//---------------------------------------------------------------------------

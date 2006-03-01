//================== SysCAD - Copyright Kenwalt (Pty) Ltd ===================
// $Nokeywords: $
//===========================================================================
/* -2 -3 -x */
/*********************************** DMM.C *********************************/
/******************************* DMM routines ******************************/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!                                                          !!!!!!!*/
/*!!!!!!!! (C) Copyright 1989, 1990, 1991 Building Block Software   !!!!!!!*/
/*!!!!!!!!                    All rights reserved                   !!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
#include <qglmem.h>
#include <dmmdefs.h>



#define  DBG_MALLOC   0x0




// save all memory allocations
#include    <malloc.h>

#ifdef _DEBUG
#define _CRTDBG_MAP_ALLOC
#include <crtdbg.h>
#endif

#include    <memory.h>

#if DBG_MALLOC

typedef struct {
                 int   flag;
                 void *addr;
                 char *name;
                 int   size;
                 int   indx;
                 unsigned long s_byte;
                 unsigned long e_byte;
                          long g_byte;
               } dxf_memory_s;

#define  MAX_DXF_MEMORY 100000
static   dxf_memory_s     dxf_memory[MAX_DXF_MEMORY];
static int n_dxf_memory;

unsigned long last_size;
unsigned long current_page_space()
  {
  MEMORYSTATUS MS;
  MS.dwLength=sizeof(MS);
  GlobalMemoryStatus(&MS);
  return MS.dwAvailPageFile;
  }

// add and malloc
int 
realloc_dxf_entry(void *addr,void *mem,char *name,int size,unsigned long s,unsigned long e,long g,long Tindx)
{
int i;

for( i = 0; i < n_dxf_memory; i++ )
  {
  if( mem == dxf_memory[i].addr )
    {
    dxf_memory[i].addr = addr;
    dxf_memory[i].name = name;
    dxf_memory[i].indx = Tindx;
    dxf_memory[i].size = size;
    dxf_memory[i].s_byte = s;
    dxf_memory[i].e_byte = e;
    dxf_memory[i].g_byte = g;
    return 0;
    }
  }
  return -1;
}

int malloc_dxf_entry(void *addr,char *name,int size,unsigned long s,unsigned long e,long g,long Tindx)
{
  if( n_dxf_memory == MAX_DXF_MEMORY )
    {
    return -1;
    }
  dxf_memory[n_dxf_memory].addr = addr;
  dxf_memory[n_dxf_memory].name = name;
  dxf_memory[n_dxf_memory].indx = Tindx;
  dxf_memory[n_dxf_memory].size = size;
  dxf_memory[n_dxf_memory].s_byte = s;
  dxf_memory[n_dxf_memory].e_byte = e;
  dxf_memory[n_dxf_memory].g_byte = g;
  n_dxf_memory++;
  return n_dxf_memory;
}

int free_dxf_entry(void *addr)
{
  int i;

  for( i = 0; i < n_dxf_memory; i++ )
    {
    if( addr == dxf_memory[i].addr )
      {
      n_dxf_memory--;
      if( n_dxf_memory )
        {
        dxf_memory[i].addr = dxf_memory[n_dxf_memory].addr;
        dxf_memory[i].name = dxf_memory[n_dxf_memory].name;
        dxf_memory[i].size = dxf_memory[n_dxf_memory].size;
        dxf_memory[i].indx = dxf_memory[n_dxf_memory].indx;
        }
      return 0;
      }
    }
  return -1;
}


int total_dxf_entry()
{
  int i,tot = 0;

  for( i = 0; i < n_dxf_memory; i++ )
    {
    tot += dxf_memory[i].size;
    }
  return tot;
}

int print_dxf_entry(char *comment,char *nm)
{
  FILE *fp = fopen(nm,"w");
  int tot,j,i;
  (void)fprintf(fp," *********** %s ************\n",comment);
  (void)fprintf(fp," Total size in Bytes: %d\n",total_dxf_entry());
  for( i = 0; i < n_dxf_memory; i++ )
    {
    dxf_memory[i].flag = 0;
    (void)fprintf(fp," Type: %-20s   Size: %-10d    Addr: 0x%8x  Indx: %-5d\n",
        dxf_memory[i].name,dxf_memory[i].size,dxf_memory[i].addr,dxf_memory[i].indx);
    }

  for( i = 0;; i++ )
    {
    while((i < n_dxf_memory) && dxf_memory[i].flag) 
      i++ ;
    if( i >= n_dxf_memory )
      break;
    tot = 0;
    for(j = i; j < n_dxf_memory; j++ )
      {
      if( strcmp(dxf_memory[i].name,dxf_memory[j].name) == 0)
        {
        tot += dxf_memory[j].size; 
        dxf_memory[j].flag = 1;
        }
      }
    (void)fprintf(fp," Type:%-20s   Total Size:%-10d\n",dxf_memory[i].name,tot);
    }
  (void)fclose(fp);
  return 0;
}

int clear_dxf_entry()
{
  n_dxf_memory = 0;
  return 0;
}

#endif

static int malloc_total = 0;

int
zero_malloc_total()
  {
  malloc_total = 0;
  return 0;
  }

int
get_malloc_total()
  {
  return malloc_total;
  }

#if DBG_MALLOC
static int Tindx = 100;
#endif

#ifdef _DEBUG
#define _dbg_dmm_malloc 0 //normally 0
#else
#define _dbg_dmm_malloc 0
#endif

#if _dbg_dmm_malloc
static int dbg_dmm_malloc_cnt = 0;
static int dbg_dmm_StopAtCnt = -1;
static size_t dmm_TestSize = 1;
#endif

/*-------------------------------------------------------------------------*/
BBS_PUBLIC void *dmm_malloc ( size ,name)
/*-------------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!                                                          !!!!!!!*/
/*!!!!!!!! (C) Copyright 1989, 1990, 1991 Building Block Software   !!!!!!!*/
/*!!!!!!!!                    All rights reserved                   !!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
size_t size ;
char *name;
{
  #if _dbg_dmm_malloc
  int b;
  unsigned int requestNumber;
  #endif

  malloc_total += size;
  #if DBG_MALLOC
    unsigned long s,e;
    long g;
    void *ptr;

    if ( size <= 0 ) 
        size = 1 ;
    s = current_page_space();
    g = last_size - s;
    ptr = malloc ( size + 8);
    if( ptr )
      memset(ptr,0,size+8);

    //ptr = malloc ( size );
    
    *(((int *)ptr) + 0 ) = ++Tindx;
    *(((int *)ptr) + 1 ) = 0xc3c3c3c3;

    e = current_page_space();
    malloc_dxf_entry(ptr,name,size,s,e,g,Tindx);
    last_size = e;
    return ((void *)(((char *)ptr) + 8));
    //return ( ptr ) ;
  #else
    {
    char *p;
    if (size<=0)
      p = NULL;
    else
      {
      p = malloc(size);

      #if _dbg_dmm_malloc
      requestNumber=0;
      if (size==dmm_TestSize)
        {
        dbg_dmm_malloc_cnt++;
        b = _CrtIsMemoryBlock((const void *)p, size, &requestNumber, NULL, NULL);
        _CrtDbgReport(_CRT_WARN, __FILE__, __LINE__, "?", "dmm_malloc(%d) Cnt:%d  Reguest:{%d}  [%s]\n", size, dbg_dmm_malloc_cnt, requestNumber, name);
        if (dbg_dmm_malloc_cnt==dbg_dmm_StopAtCnt)
          {
          _CrtDbgBreak();
          }
        }
      #endif

      if( p )
        memset(p,0,size);
      }
    return p;

    }
  #endif
}
                             
/*-------------------------------------------------------------------------*/
BBS_PUBLIC void *dmm_realloc ( mem, size ,name )
/*-------------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!                                                          !!!!!!!*/
/*!!!!!!!! (C) Copyright 1989, 1990, 1991 Building Block Software   !!!!!!!*/
/*!!!!!!!!                    All rights reserved                   !!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
void *mem ;
size_t size ;
char *name;
{
  #if DBG_MALLOC
    unsigned long s,e;
    long g;
    void *ptr;
    
    mem = (void *)(((char *)mem) - 8);

    s = current_page_space();
    g = last_size - s;
    ptr = realloc ( mem, size + 8 );
    *(((int *)ptr) + 0 ) = ++Tindx;
    *(((int *)ptr) + 1 ) = 0xc3c3c3c3;
    e = current_page_space();
    realloc_dxf_entry(ptr,mem,name,size,s,e,g,Tindx);
    last_size = e;
    return ( (void *)(((char *)ptr) + 8)) ;

  #else
    
    return realloc ( mem, size );

  #endif
}

/*-------------------------------------------------------------------------*/
BBS_PUBLIC void dmm_free ( mem )
/*-------------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!                                                          !!!!!!!*/
/*!!!!!!!! (C) Copyright 1989, 1990, 1991 Building Block Software   !!!!!!!*/
/*!!!!!!!!                    All rights reserved                   !!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
void *mem ;
{
  #if DBG_MALLOC
    {
    if( mem )
      {
      mem = (void *)(((char *)mem) - 8);
      free_dxf_entry ( mem );
      free ( mem );
      }
    }
  #else

    if( mem )
      free ( mem ) ;

  #endif
}




/*------------------------------------------------------------------------*/
BBS_PRIVATE void qgl_print_malloc () 
/*-------------------------------------------------------------------------*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
/*!!!!!!!!                                                         !!!!!!!!*/
/*!!!!!!!! (C) Copyright 1989, 1990, 1991 Building Block Software  !!!!!!!!*/
/*!!!!!!!!                    All rights reserved                  !!!!!!!!*/
/*!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!*/
{
}





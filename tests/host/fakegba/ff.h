/* fakegba/ff.h — host-build stand-in for FatFs's ff.h.
 *
 * source/draw.c includes "ezkernel.h", which includes "ff.h" only for a few
 * extern declarations (FATFS / FFOBJID / DWORD / TCHAR).  The real FatFs
 * header lives in source/ff16/ and drags in the whole filesystem layer; this
 * stub supplies just the type names so ezkernel.h parses on the host.
 */
#ifndef FAKEGBA_FF_H
#define FAKEGBA_FF_H

typedef unsigned char  BYTE;
typedef unsigned short WORD;
typedef unsigned int   UINT;
typedef unsigned int   DWORD;
typedef DWORD          FSIZE_t;
typedef char           TCHAR;

typedef struct { int fake; } FATFS;
typedef struct { int fake; } FFOBJID;
typedef struct { int fake; } FIL;
typedef struct { int fake; } DIR;
typedef struct { int fake; } FILINFO;
typedef int FRESULT;

#endif /* FAKEGBA_FF_H */

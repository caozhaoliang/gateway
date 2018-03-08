#ifndef __ZLIB_WRAPPER_H__
#define __ZLIB_WRAPPER_H__

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#ifndef _WIN32
#include <zlib.h>
#else
#include <zlib.h>
#endif

class ZLibWrapper
{
public:
    ZLibWrapper();
    ~ZLibWrapper();

    static bool Unzip(unsigned char * pSrc, 
					  unsigned int unSrcLen, 
                      unsigned char * & pDest, 
					  unsigned int & unDestLen, 
                      unsigned char * pDecompressMem = NULL, 
                      int unDecompressMemLen = 0);

private:
    static void RemoveMem(unsigned char * pMem, unsigned int & unMemLen, unsigned int unRemoveLen);
};

#endif


#include "ZLibWrapper.h"

ZLibWrapper::ZLibWrapper()
{
}

ZLibWrapper::~ZLibWrapper()
{
}

bool ZLibWrapper::Unzip(unsigned char * pSrc,
					    unsigned int unSrcLen,
					    unsigned char * & pDest,
					    unsigned int & unDestLen,
                        unsigned char * pDecompressMem,
					    int unDecompressMemLen)
{
//#ifndef WIN32

    unsigned char * pDecompressedData;
    unsigned int unCompressLen = unSrcLen;
    unsigned int unReservedLen = 0;

    if (NULL == pSrc || unCompressLen < 10)
		return false;

	// 不是zip文件
    if ((0x1F != pSrc[0]) || (0x8B != pSrc[1]) || (8 != pSrc[2]))
		return false;

    char cFlag = pSrc[3];

    RemoveMem(pSrc, unCompressLen, 10);

    if (cFlag & 0x04)
    {
        if (unCompressLen < 2)
			return false;

        unsigned short usLen = *(unsigned short *)pSrc;

        if (unCompressLen < (unsigned int)(usLen + 2))
			return false;

        RemoveMem(pSrc, unCompressLen, usLen + 2);
    }

    if (cFlag & 0x08)
    {
        while (1)
        {
            if (0 == unCompressLen)
				return false;

            int nChar = pSrc[0];

            RemoveMem(pSrc, unCompressLen, 1);

            if (0 == nChar)
				break;
        }
    }

    if (cFlag & 0x10)
    {
        while (1)
        {
            if (0 == unCompressLen)
				return false;

            int nChar = pSrc[0];

            RemoveMem(pSrc, unCompressLen, 1);

            if (0 == nChar)
				break;
        }
    }

    if (cFlag & 0x02)
    {
        if (unCompressLen < 2)
			return false;
        RemoveMem(pSrc, unCompressLen, 2);
    }

    // After removing all the header information from the front,
    // remove the last 8 bytes from the end
    if (unCompressLen <= 8)
		return false;

    unCompressLen -= 8;

    z_stream pStream;
    memset(&pStream, 0, sizeof(pStream));
    if (Z_OK != inflateInit2(&pStream, -MAX_WBITS))
		return false;

    if (NULL == pDecompressMem)
    {
        // gzip的压缩比率最高6倍左右
        unReservedLen = unCompressLen * 6;
        if (NULL == (pDecompressedData = new unsigned char[unReservedLen]))
			return false;
    }
    else
    {
        unReservedLen = unDecompressMemLen;
        pDecompressedData = pDecompressMem;
    }

    // Tell the z_stream structure where to work
    pStream.next_in = pSrc;                 // Decompress the memory here
    pStream.avail_in = unCompressLen;       // There is this much of it
    pStream.next_out = pDecompressedData;   // Write depSrc data here
    pStream.avail_out = unReservedLen;      // Tell ZLib it has this much space, it make this smaller to show how much space is left

    if (Z_STREAM_END == inflate(&pStream, Z_FINISH))
    {
        unDestLen = unReservedLen - pStream.avail_out;

        if ((unDestLen > 0) && (NULL != (pDest = new unsigned char[unDestLen])))
        {
            inflateEnd(&pStream);
            memcpy(pDest, pDecompressedData, unDestLen);
            if (NULL == pDecompressMem)
				delete [] pDecompressedData;
            return true;
        }
    }

    inflateEnd(&pStream);
    if (NULL == pDecompressMem)
		delete [] pDecompressedData;

//#endif // #ifndef WIN32

    return false;
}

void ZLibWrapper::RemoveMem(unsigned char * pMem, unsigned int & unMemLen, unsigned int unRemoveLen)
{
    if ((0 == unRemoveLen) || (unRemoveLen > unMemLen))
		return;

    unMemLen -= unRemoveLen;

    memmove(pMem, pMem + unRemoveLen, unMemLen);
}

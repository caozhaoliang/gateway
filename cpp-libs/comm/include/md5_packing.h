
#ifndef _MD5_H_
#define _MD5_H_

#include <stddef.h>
#include <string>
using namespace std;

class MD5
{
public:
	static const size_t DigestLength = 16;
	// 供外部使用的哈希函数：生成16bytes的hash值
	static void encode(unsigned char* pcDest, const char* pcSrc);

	// 供外部使用的64位哈希函数：生成long long的字符串形式hash值
    static void Hash64(const char* pcSrc, char* pcHashValue);

	// 供外部使用的64位哈希函数：生成long long的hash值
    static void Hash64(const char* pcSrc, long long& ddwHashValue);

	// 供外部使用的64位哈希函数：生成unsigned long long的hash值
    static void Hash64(const char* pcSrc, unsigned long long& ddwHashValue);

	// 供外部使用的64位哈希函数：生成16bytes的hash值
	static void Encode(const void* pcSrc, size_t dwSrcLen, unsigned char* pcDest);

	//生成32位MD5字符串
    static int Encode32(unsigned char* input, unsigned int len, string &outstring);

	// 供外部使用的64位哈希函数：生成long long的字符串形式hash值
    static void Hash64(const void* pcSrc, size_t dwSrcLen, char* pcHashValue);

	// 供外部使用的64位哈希函数：生成long long的hash值
    static void Hash64(const void* pcsrc, size_t dwsrclen, long long& ddwhashvalue);

	// 供外部使用的64位哈希函数：生成unsigned long long的hash值
    static void Hash64(const void* pcSrc, size_t dwsrclen, unsigned long long& ddwHashValue);
#ifdef _LP64 // Unix 64 位，long 是 64 位
	// 供外部使用的64位哈希函数：生成long 的hash值
    static void Hash64(const char* pcSrc, unsigned long & ddwHashValue);
	// 供外部使用的64位哈希函数：生成unsigned long的hash值
    static void Hash64(const void* pcSrc, size_t dwsrclen, unsigned long& ddwHashValue);
#endif
	// 供外部使用的64位哈希函数：生成long long的hash值
    static unsigned long long GetHash64(const char* pcSrc);
    static unsigned long long GetHash64(const void* pcSrc, size_t dwsrclen);
};

#endif//_MD5_H_


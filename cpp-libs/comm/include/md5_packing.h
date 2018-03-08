
#ifndef _MD5_H_
#define _MD5_H_

#include <stddef.h>
#include <string>
using namespace std;

class MD5
{
public:
	static const size_t DigestLength = 16;
	// ���ⲿʹ�õĹ�ϣ����������16bytes��hashֵ
	static void encode(unsigned char* pcDest, const char* pcSrc);

	// ���ⲿʹ�õ�64λ��ϣ����������long long���ַ�����ʽhashֵ
    static void Hash64(const char* pcSrc, char* pcHashValue);

	// ���ⲿʹ�õ�64λ��ϣ����������long long��hashֵ
    static void Hash64(const char* pcSrc, long long& ddwHashValue);

	// ���ⲿʹ�õ�64λ��ϣ����������unsigned long long��hashֵ
    static void Hash64(const char* pcSrc, unsigned long long& ddwHashValue);

	// ���ⲿʹ�õ�64λ��ϣ����������16bytes��hashֵ
	static void Encode(const void* pcSrc, size_t dwSrcLen, unsigned char* pcDest);

	//����32λMD5�ַ���
    static int Encode32(unsigned char* input, unsigned int len, string &outstring);

	// ���ⲿʹ�õ�64λ��ϣ����������long long���ַ�����ʽhashֵ
    static void Hash64(const void* pcSrc, size_t dwSrcLen, char* pcHashValue);

	// ���ⲿʹ�õ�64λ��ϣ����������long long��hashֵ
    static void Hash64(const void* pcsrc, size_t dwsrclen, long long& ddwhashvalue);

	// ���ⲿʹ�õ�64λ��ϣ����������unsigned long long��hashֵ
    static void Hash64(const void* pcSrc, size_t dwsrclen, unsigned long long& ddwHashValue);
#ifdef _LP64 // Unix 64 λ��long �� 64 λ
	// ���ⲿʹ�õ�64λ��ϣ����������long ��hashֵ
    static void Hash64(const char* pcSrc, unsigned long & ddwHashValue);
	// ���ⲿʹ�õ�64λ��ϣ����������unsigned long��hashֵ
    static void Hash64(const void* pcSrc, size_t dwsrclen, unsigned long& ddwHashValue);
#endif
	// ���ⲿʹ�õ�64λ��ϣ����������long long��hashֵ
    static unsigned long long GetHash64(const char* pcSrc);
    static unsigned long long GetHash64(const void* pcSrc, size_t dwsrclen);
};

#endif//_MD5_H_


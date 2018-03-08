#ifndef _BASE64_H_
#define _BASE64_H_

class Base64
{
public:
	Base64(){}
	virtual ~Base64(){}

	// ���κ�����data����Base64��������str��
	size_t encode(char *str, const void *data, int size);

	// �Ѿ���Base64����������str���������data��
	size_t decode( unsigned char * const p_pszDest, const char *p_pszSrc);

private:

	// ������Base64������е�������ĳ���ַ�
	char  base2Chr( unsigned char n );

	// ���ĳ���ַ���Base64������е����
	unsigned char chr2Base( char c );
};

#endif

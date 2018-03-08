#ifndef _BASE64_H_
#define _BASE64_H_

class Base64
{
public:
	Base64(){}
	virtual ~Base64(){}

	// 对任何数据data进行Base64编码后存在str中
	size_t encode(char *str, const void *data, int size);

	// 把经过Base64编码后的数据str经过解码后data中
	size_t decode( unsigned char * const p_pszDest, const char *p_pszSrc);

private:

	// 根据在Base64编码表中的序号求得某个字符
	char  base2Chr( unsigned char n );

	// 求得某个字符在Base64编码表中的序号
	unsigned char chr2Base( char c );
};

#endif

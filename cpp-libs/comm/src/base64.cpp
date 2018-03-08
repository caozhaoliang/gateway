#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <base64.h>

static char base64_code[] = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";

/**
 * @Function: 根据在Base64编码表中的序号求得某个字符
 *            0-63 : A-Z(25) a-z(51), 0-9(61), +(62), /(63)
 * @Param:    unsigned char n
 * @Return:   字符
 */
char  Base64::base2Chr( unsigned char n )
{
	n &= 0x3F;
	if ( n < 26 )
    	return ( char )( n + 'A' );
    else if ( n < 52 )
    	return ( char )( n - 26 + 'a' );
    else if ( n < 62 )
    	return ( char )( n - 52 + '0' );
    else if ( n == 62 )
    	return '+';
    else
    	return '/';
}

/**
 * @function 求得某个字符在Base64编码表中的序号
 * @param    c   字符
 * @return   序号值
 */
unsigned char Base64::chr2Base( char c )
{
	if ( c >= 'A' && c <= 'Z' )
    	return ( unsigned char )( c - 'A' );
    else if ( c >= 'a' && c <= 'z' )
    	return ( unsigned char )( c - 'a' + 26 );
    else if ( c >= '0' && c <= '9' )
    	return ( unsigned char )( c - '0' + 52 );
    else if ( c == '+' )
    	return 62;
    else if ( c == '/' )
    	return 63;
    else
        return 64;  //  无效字符
}

/**
 * @function 对数据进行Base64编码
 * @param   data 要进行编码的数据
 * @param   size         要编码的数据量
 * @param   str       经过Base64编码后的字符串, 这个空间在外部分配，一定要足够大，否则产生异常
 * @return  成功：true
 * @details: 该算法主要是每次取二进制数据的6比特，编码成大小写字母、数字和+=共64个字母
 */
size_t Base64::encode(char *str, const void *data, int size)
{
	// 入口条件检查
	assert((0 != str) && (0 != data) && (0 <= size));

	char *s, *p;
	int c;
	const unsigned char *q;

	p = s = str;
	q = (const unsigned char*)data;

	// size是要经过编码数据的长度
	for(long i = 0; i < size;)
	{
		// 处理的时候，都是把24bit当作一个单位，因为3*8=4*6
		c = q[i++];
		c *= 256;
		if(i < size)
			c += q[i];
		i++;
		c *= 256;
		if(i < size)
			c += q[i];
		i++;

		// 每次取6bit当作一个8bit的char放在p中
		p[0] = base64_code[(c&0x00fc0000) >> 18];
		p[1] = base64_code[(c&0x0003f000) >> 12];
		p[2] = base64_code[(c&0x00000fc0) >> 6];
		p[3] = base64_code[(c&0x0000003f) >> 0];

		// 这里是处理结尾情况
		if(i > size)
			p[3] = '=';
		if(i > size+1)
			p[2] = '=';

		p += 4; // 编码后的数据指针相应的移动
	}
	*p = 0;   // 防野指针

	return strlen(str);
}

/**
 * @Function:对Base64编码后的数据进行解码
 * @Param:   unsigned char * const p_pszDest 经过Base64解码后的数据
 *			 const char *p_pszSrc		     Base64编码数据
 * @Return:  解码后数据长度
 */
size_t Base64::decode(unsigned char * const p_pszDest, const char *p_pszSrc)
{
	// 入口条件检查:指针不能为空且不能相同
	assert((0 != p_pszDest) && (0 != p_pszSrc) && ((const char *)p_pszDest != p_pszSrc));

	unsigned char * p = p_pszDest;
    size_t          n = strlen( p_pszSrc );
    unsigned char   c, t = 0;

    for (size_t i = 0; i < n; i++ )
    {
    	if ( *p_pszSrc == '=' )
        	break;
        do {
        	if ( *p_pszSrc )
		       	c = chr2Base( *p_pszSrc++ );
            else
            	c = 65;  //  字符串结束
        } while ( c == 64 );  //  跳过无效字符，如回车等
        if ( c == 65 )
        	break;
        switch ( i % 4 )
        {
        case 0 :
            t = (unsigned char)(c << 2);
            break;
        case 1 :
            *p++ = ( unsigned char )( t | ( c >> 4 ) );
            t = ( unsigned char )( c << 4 );
            break;
        case 2 :
            *p++ = ( unsigned char )( t | ( c >> 2 ) );
            t = ( unsigned char )( c << 6 );
            break;
        case 3 :
            *p++ = ( unsigned char )( t | c );
            break;
		default:
			break;
        }
    }
	return ( p - p_pszDest );
}


//#define BASE64_MAIN
#ifdef BASE64_MAIN
main()
{
	Base64 oBase64;
	const char * pcBefore = "hello world";
	char *pcAfter = new char[strlen(pcBefore)*4/3+5];

	oBase64.encode(&pcAfter, (const void*)pcBefore, strlen(pcBefore));

}
#endif
